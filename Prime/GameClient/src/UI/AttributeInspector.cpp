//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "AttributeInspector.h"
#include "AttributeVariable.h"
#include "AttributeVariableEvents.h"
#include "ResourcePicker.h"
#include "AttributeContainer.h"
#include "UIGlobals.h"
#include "UIUtils.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/ScrollBar.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/DebugNew.h>

namespace Prime
{
	namespace UI
	{
		void AttributeInspector::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<AttributeInspector>();
		}

		AttributeInspector::~AttributeInspector()
		{
		}

		AttributeInspector::AttributeInspector(Urho3D::Context* context) : Object(context)
		{
			parentContainer_ = NULL;
			editorResourcePicker_ = NULL;
			applyMaterialList_ = true;
			attributesDirty_ = false;
			attributesFullDirty_ = false;

			inLoadAttributeEditor_ = false;
			inEditAttribute_ = false;
			showNonEditableAttribute_ = false;
			numEditableComponentsPerNode_ = 1;

			normalTextColor_ = Urho3D::Color(1.0f, 1.0f, 1.0f);
			modifiedTextColor_ = Urho3D::Color(1.0f, 0.8f, 0.5f);
			nonEditableTextColor_ = Urho3D::Color(0.7f, 0.7f, 0.7f);

			// Exceptions for string attributes that should not be continuously edited
			noTextChangedAttrs_.Push("Script File");
			noTextChangedAttrs_.Push("Class Name");
			noTextChangedAttrs_.Push("Script Object Type");
			noTextChangedAttrs_.Push("Script File Name");
		}

		Urho3D::UIElement* AttributeInspector::Create()
		{
			cache_ = GetSubsystem<Urho3D::ResourceCache>();
			fileSystem_ = GetSubsystem<Urho3D::FileSystem>();


			styleFile_ = cache_->GetResource<Urho3D::XMLFile>("UI/IDEStyle.xml");
			iconStyle_ = cache_->GetResource<Urho3D::XMLFile>("UI/EditorIcons.xml");
			editorResourcePicker_ = GetSubsystem<ResourcePickerManager>();
			editorResourcePicker_->Init();


			attributewindow_ = new Urho3D::Window(context_);
			attributewindow_->SetName("attributewindow");
			attributewindow_->LoadXML(cache_->GetResource<Urho3D::XMLFile>("UI/EditorInspectorWindow.xml")->GetRoot(), styleFile_);

			parentContainer_ = attributewindow_->GetChild("ParentContainer", true);

			Urho3D::String resources[] = { "UI/EditorInspector_Attribute.xml", "UI/EditorInspector_Variable.xml", "UI/EditorInspector_Style.xml" };
			for (unsigned int i = 0; i < 3; ++i)
				xmlResources_.Push(Urho3D::SharedPtr<Urho3D::XMLFile>(cache_->GetResource<Urho3D::XMLFile>(resources[i])));

			Update();

			SubscribeToEvent(dynamic_cast<Urho3D::Button*>(attributewindow_->GetChild("CloseButton", true)), Urho3D::E_RELEASED, HANDLER(AttributeInspector, HideWindow));

			SubscribeToEvent(AEE_PICKRESOURCE, HANDLER(AttributeInspector, PickResource));
			SubscribeToEvent(AEE_OPENRESOURCE, HANDLER(AttributeInspector, OpenResource));
			SubscribeToEvent(AEE_EDITRESOURCE, HANDLER(AttributeInspector, EditResource));
			SubscribeToEvent(AEE_TESTRESOURCE, HANDLER(AttributeInspector, TestResource));

			return attributewindow_;
		}

		bool AttributeInspector::DeleteNodeContainer(Urho3D::Serializable* serializable)
		{
			if (!serializable)
				return NULL;
			Urho3D::HashMap< Urho3D::StringHash, Urho3D::SharedPtr<AttributeContainer> >::Iterator it = nodeContainers_.Find(serializable->GetType());
			if (it != nodeContainers_.End())
			{
				nodeContainers_.Erase(it);
				return true;
			}
			return false;
		}

		AttributeContainer* AttributeInspector::CreateNodeContainer(Urho3D::Serializable* serializable)
		{
			if (!serializable)
				return NULL;
			Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<AttributeContainer> >::Iterator it = nodeContainers_.Find(serializable->GetType());
			if (it != nodeContainers_.End())
				return it->second_;

			AttributeContainer* nodeContainer = new AttributeContainer(context_);
			nodeContainer->SetTitle("Select editable objects");
			nodeContainer->SetNoTextChangedAttrs(noTextChangedAttrs_);
			nodeContainer->LoadChildXML(xmlResources_[1]->GetRoot(), styleFile_);

			nodeContainer->SetIcon(iconStyle_, Urho3D::Node::GetTypeNameStatic());

			SubscribeToEvent(nodeContainer->GetChild("ResetToDefault", true), Urho3D::E_RELEASED, HANDLER(AttributeInspector, HandleResetToDefault));
			SubscribeToEvent(nodeContainer->GetChild("NewVarDropDown", true), Urho3D::E_ITEMSELECTED, HANDLER(AttributeInspector, CreateNodeVariable));
			SubscribeToEvent(nodeContainer->GetChild("DeleteVarButton", true), Urho3D::E_RELEASED, HANDLER(AttributeInspector, DeleteNodeVariable));

			// Resize the node editor according to the number of variables, up to a certain maximum
			unsigned int maxAttrs = Urho3D::Clamp((int)nodeContainer->GetAttributeList()->GetContentElement()->GetNumChildren(), MIN_NODE_ATTRIBUTES, MAX_NODE_ATTRIBUTES);
			nodeContainer->GetAttributeList()->SetFixedHeight(maxAttrs * ATTR_HEIGHT + 4);
			nodeContainer->SetFixedHeight(maxAttrs * ATTR_HEIGHT + 58);

			parentContainer_->AddChild(nodeContainer);

			nodeContainer->SetStyleAuto();

			nodeContainers_[serializable->GetType()] = nodeContainer;

			return nodeContainer;
		}

		AttributeContainer* AttributeInspector::CreateComponentContainer(Urho3D::Serializable* serializable)
		{
			if (!serializable)
				return NULL;
			Urho3D::HashMap< Urho3D::StringHash, Urho3D::SharedPtr<AttributeContainer> >::Iterator it = componentContainers_.Find(serializable->GetType());
			if (it != componentContainers_.End())
				return it->second_;

			Urho3D::SharedPtr<AttributeContainer> componentContainer(new AttributeContainer(context_));

			componentContainer->SetStyleAuto(styleFile_);
			componentContainer->SetNoTextChangedAttrs(noTextChangedAttrs_);
			componentContainer->SetTitle(serializable->GetTypeName());
			componentContainer->SetSerializableAttributes(serializable);

			componentContainers_[serializable->GetType()] = componentContainer;

			// Resize the node editor according to the number of variables, up to a certain maximum
			unsigned int maxAttrs = componentContainer->GetAttributeList()->GetContentElement()->GetNumChildren();
			componentContainer->GetAttributeList()->SetHeight(maxAttrs * ATTR_HEIGHT + 4);
			componentContainer->SetHeight(maxAttrs * ATTR_HEIGHT + 58);

			parentContainer_->AddChild(componentContainer);
			componentContainer->SetIcon(iconStyle_, serializable->GetTypeName());

			parentContainer_->UpdateLayout();

			return componentContainer;
		}

		void AttributeInspector::Update(bool fullUpdate /*= true*/)
		{
			attributesDirty_ = false;
			if (fullUpdate)
			{
				attributesFullDirty_ = false;
				componentContainers_.Clear();
			}

			DisableAllContainers();

			if (!editNodes_.Empty())
			{
				//	Vector<Serializable*> nodes = UIUtils::ToSerializableArray(editorData_->GetEditNodes());
				AttributeContainer* nodeContainer = CreateNodeContainer(editNodes_[0]);
				nodeContainer->SetVisible(true);
				nodeContainer->SetEnabled(true);

				Urho3D::Node* editNode = editNodes_[0];
				if (editNode != NULL)
				{
					Urho3D::String idStr;
					if (editNode->GetID() >= Urho3D::FIRST_LOCAL_ID)
						idStr = " (Local ID " + Urho3D::String(editNode->GetID()) + ")";
					else
						idStr = " (ID " + Urho3D::String(editNode->GetID()) + ")";

					nodeContainer->SetTitle(editNode->GetTypeName() + idStr);
				}
				else
				{
					nodeContainer->SetTitle(editNodes_[0]->GetTypeName() + " (ID -- : " + Urho3D::String(editNodes_.Size()) + "x)");
				}

				nodeContainer->SetSerializableAttributes(editNodes_[0]);
			}

			if (!editComponents_.Empty())
			{

				for (unsigned int j = 0; j < editComponents_.Size(); ++j)
				{
					Urho3D::Component* comp = editComponents_[j];

					AttributeContainer* container = CreateComponentContainer(comp);

					container->SetVisible(true);
					container->SetEnabled(true);

					container->SetTitle(UIUtils::GetComponentTitle(comp));

					container->SetSerializableAttributes(comp);
				}
			}

			if (parentContainer_->GetNumChildren() == 0)
			{
				// No editables, insert a dummy component container to show the information
			}
		}

		void AttributeInspector::PickResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace PickResource;
			//TODO:
			ResourceRefAttributeUI* attrEdit = dynamic_cast<ResourceRefAttributeUI*>(eventData[P_ATTEDIT].GetPtr());

			Urho3D::Vector<Urho3D::Serializable*> targets = GetAttributeEditorTargets(attrEdit);
			if (targets.Empty())
				return;

			editorResourcePicker_->SetresourcePickIndex(attrEdit->GetIndex());
			editorResourcePicker_->SetresourcePickSubIndex(attrEdit->GetSubIndex());
			Urho3D::AttributeInfo info = targets[0]->GetAttributes()->At(editorResourcePicker_->GetresourcePickIndex());

			Urho3D::StringHash resourceType;
			if (info.type_ == Urho3D::VAR_RESOURCEREF)
				resourceType = targets[0]->GetAttribute(editorResourcePicker_->GetresourcePickIndex()).GetResourceRef().type_;
			else if (info.type_ == Urho3D::VAR_RESOURCEREFLIST)
				resourceType = targets[0]->GetAttribute(editorResourcePicker_->GetresourcePickIndex()).GetResourceRefList().type_;
			else if (info.type_ == Urho3D::VAR_VARIANTVECTOR)
				resourceType = targets[0]->GetAttribute(editorResourcePicker_->GetresourcePickIndex()).GetVariantVector()[editorResourcePicker_->GetresourcePickSubIndex()].GetResourceRef().type_;

			editorResourcePicker_->SetCurrentResourcePicker(editorResourcePicker_->GetResourcePicker(resourceType));
			ResourcePicker* picker = editorResourcePicker_->GetCurrentResourcePicker();
			if (picker == NULL)
				return;

			editorResourcePicker_->GetresourceTargets().Clear();
			for (unsigned int i = 0; i < targets.Size(); ++i)
				editorResourcePicker_->GetresourceTargets().Push(targets[i]);

			Urho3D::String lastPath = picker->lastPath;
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			if (lastPath.Empty())
				lastPath = cache->GetResourceDirs()[0];



			uiFileSelector_ = new  Urho3D::FileSelector(context_);
			uiFileSelector_->SetDefaultStyle(cache_->GetResource<Urho3D::XMLFile>("UI/DefaultStyle.xml"));
			uiFileSelector_->SetTitle("Pick " + picker->typeName);
			uiFileSelector_->SetPath(lastPath);
			uiFileSelector_->SetButtonTexts("OK", "Cancel");
			uiFileSelector_->SetFilters(picker->filters, picker->lastFilter);

			Urho3D::IntVector2 size = uiFileSelector_->GetWindow()->GetSize();
			Urho3D::Graphics* graphics = GetSubsystem<Urho3D::Graphics>();
			uiFileSelector_->GetWindow()->SetPosition((graphics->GetWidth() - size.x_) / 2, (graphics->GetHeight() - size.y_) / 2);

			SubscribeToEvent(uiFileSelector_, Urho3D::E_FILESELECTED, HANDLER(AttributeInspector, PickResourceDone));
		}

		void AttributeInspector::EditAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void AttributeInspector::OpenResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void AttributeInspector::EditResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void AttributeInspector::TestResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void AttributeInspector::HandleResetToDefault(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void AttributeInspector::CreateNodeVariable(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (editNodes_.Empty())
				return;
			Urho3D::LineEdit* editName = NULL;
			Urho3D::String newName = ExtractVariableName(eventData, editName);
			if (newName.Empty())
				return;

			// Create scene variable
			editNodes_[0]->GetScene()->RegisterVar(newName);


			Urho3D::Variant newValue = ExtractVariantType(eventData);

			// If we overwrite an existing variable, must recreate the attribute-editor(s) for the correct type
			bool overwrite = false;
			for (unsigned int i = 0; i < editNodes_.Size(); ++i)
			{
				overwrite = overwrite || editNodes_[i]->GetVars().Contains(newName);
				editNodes_[i]->SetVar(newName, newValue);
			}

			AttributeContainer* nodeContainer = CreateNodeContainer(editNodes_[0]);
			nodeContainer->UpdateVariantMap(editNodes_[0]);

			if (editName)
				editName->SetText("");
		}

		void AttributeInspector::DeleteNodeVariable(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (editNodes_.Empty())
				return;
			Urho3D::LineEdit* editName = NULL;

			Urho3D::String delName = ExtractVariableName(eventData, editName);
			if (delName.Empty())
				return;

			// Note: intentionally do not unregister the variable name here as the same variable name may still be used by other attribute list

			bool erased = false;
			for (unsigned int i = 0; i < editNodes_.Size(); ++i)
			{
				// \todo Should first check whether var in question is editable
				//	erased = editorData_->GetEditNodes()[i].GetVars().Erase(delName) || erased;
			}
			if (editName)
				editName->SetText("");
		}

		void AttributeInspector::HideWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			attributewindow_->SetVisible(false);
		}

		void AttributeInspector::DisableAllContainers()
		{
			for (unsigned int i = 0; i < parentContainer_->GetNumChildren(); i++)
			{
				Urho3D::UIElement* e = parentContainer_->GetChild(i);
				if (e)
				{
					e->SetVisible(false);
					e->SetEnabled(false);
				}
			}
		}



		Urho3D::Window* AttributeInspector::GetAttributewindow()
		{
			return attributewindow_;

		}

		Urho3D::Vector<Urho3D::Node*>& AttributeInspector::GetEditNodes()
		{
			return editNodes_;
		}

		Urho3D::Vector<Urho3D::Component*>& AttributeInspector::GetEditComponents()
		{
			return editComponents_;
		}

		Urho3D::Vector<Urho3D::UIElement*>& AttributeInspector::GetEditUIElements()
		{
			return editUIElements_;
		}

		Urho3D::String AttributeInspector::ExtractVariableName(Urho3D::VariantMap& eventData, Urho3D::LineEdit* nameEdit)
		{
			Urho3D::UIElement* element = (Urho3D::UIElement*)eventData[Urho3D::ItemSelected::P_ELEMENT].GetPtr();
			Urho3D::UIElement* parent = element->GetParent();
			nameEdit = (Urho3D::LineEdit*)parent->GetChild("VarNameEdit", true);
			return nameEdit->GetText().Trimmed();
		}

		Urho3D::Variant AttributeInspector::ExtractVariantType(Urho3D::VariantMap& eventData)
		{
			Urho3D::DropDownList* dropDown = (Urho3D::DropDownList*)eventData[Urho3D::ItemSelected::P_ELEMENT].GetPtr();
			switch (dropDown->GetSelection())
			{
			case 0:
				return Urho3D::Variant(0);
			case 1:
				return Urho3D::Variant(false);
			case 2:
				return Urho3D::Variant(0.0f);
			case 3:
				return Urho3D::Variant(Urho3D::String());
			case 4:
				return Urho3D::Variant(Urho3D::Vector3());
			case 5:
				return Urho3D::Variant(Urho3D::Color());
			}

			return Urho3D::Variant::EMPTY;   // This should not happen
		}

		Urho3D::Vector<Urho3D::Serializable*> AttributeInspector::GetAttributeEditorTargets(BasicAttributeUI* attrEdit)
		{
			Urho3D::Vector<Urho3D::Serializable*> ret;


			Urho3D::UIElement* elpar = attrEdit->GetParent()->GetParent()->GetParent()->GetParent();
			AttributeContainer* acon = dynamic_cast<AttributeContainer*>(elpar);
			if (acon)
				ret.Push(acon->GetSerializable());

			// 		const Vector<unsigned int>& ids = attrEdit->GetIDs();
			// 		if (attrEdit->GetIDType() == NODE_IDS_VAR)
			// 		{
			// 			for (unsigned int i = 0; i < ids.Size(); ++i)
			// 			{
			// 				Node* node = editorData_->GetCurrentScene()->GetNode(ids[i]);
			// 				if (node != NULL)
			// 					ret.Push(node);
			// 			}
			// 		}
			// 		else if (attrEdit->GetIDType() == COMPONENT_IDS_VAR)
			// 		{
			// 			for (unsigned int i = 0; i < ids.Size(); ++i)
			// 			{
			// 				Component* component = editorData_->GetCurrentScene()->GetComponent(ids[i]);
			// 				if (component != NULL)
			// 					ret.Push(component);
			// 			}
			// 		}
			// 		else	if (attrEdit->GetIDType() == UI_ELEMENT_IDS_VAR)
			// 		{
			// 			for (unsigned int i = 0; i < ids.Size(); ++i)
			// 			{
			// 				// TODO:
			// 				// 							UIElement* element = editorUIElement.GetChild(UI_ELEMENT_ID_VAR, ids[i], true);
			// 				// 							if (element != NULL)
			// 				// 								ret.Push(element);
			// 			}
			// 		}

			return ret;
		}

		void AttributeInspector::PickResourceDone(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			StoreResourcePickerPath();
			uiFileSelector_ = NULL;


			if (!eventData["OK"].GetBool())
			{
				editorResourcePicker_->GetresourceTargets().Clear();
				editorResourcePicker_->SetCurrentResourcePicker(NULL);
				return;
			}

			if (editorResourcePicker_->GetCurrentResourcePicker() == NULL)
				return;

			// Validate the resource. It must come from within a registered resource directory, and be loaded successfully
			Urho3D::String resourceName = eventData["FileName"].GetString();
			Urho3D::Resource* res = GetPickedResource(resourceName);
			if (res == NULL)
			{
				editorResourcePicker_->SetCurrentResourcePicker(NULL);
				return;
			}

			// Store old values so that PostEditAttribute can create undo actions
			Urho3D::Vector<Urho3D::Variant> oldValues;
			for (unsigned int i = 0; i < editorResourcePicker_->GetresourceTargets().Size(); ++i)
				oldValues.Push(editorResourcePicker_->GetresourceTargets()[i]->GetAttribute(editorResourcePicker_->GetresourcePickIndex()));

			for (unsigned int i = 0; i < editorResourcePicker_->GetresourceTargets().Size(); ++i)
			{
				Urho3D::Serializable* target = editorResourcePicker_->GetresourceTargets()[i];

				Urho3D::AttributeInfo info = target->GetAttributes()->At(editorResourcePicker_->GetresourcePickIndex());
				if (info.type_ == Urho3D::VAR_RESOURCEREF)
				{
					Urho3D::ResourceRef ref = target->GetAttribute(editorResourcePicker_->GetresourcePickIndex()).GetResourceRef();
					ref.type_ = res->GetType();
					ref.name_ = res->GetName();
					target->SetAttribute(editorResourcePicker_->GetresourcePickIndex(), Urho3D::Variant(ref));
					target->ApplyAttributes();
				}
				else if (info.type_ == Urho3D::VAR_RESOURCEREFLIST)
				{
					Urho3D::ResourceRefList refList = target->GetAttribute(editorResourcePicker_->GetresourcePickIndex()).GetResourceRefList();
					if (editorResourcePicker_->GetresourcePickSubIndex() < refList.names_.Size())
					{
						refList.names_[editorResourcePicker_->GetresourcePickSubIndex()] = res->GetName();
						target->SetAttribute(editorResourcePicker_->GetresourcePickIndex(), Urho3D::Variant(refList));
						target->ApplyAttributes();
					}
				}
				else if (info.type_ == Urho3D::VAR_VARIANTVECTOR)
				{
					Urho3D::Vector<Urho3D::Variant> attrs = target->GetAttribute(editorResourcePicker_->GetresourcePickIndex()).GetVariantVector();
					Urho3D::ResourceRef ref = attrs[editorResourcePicker_->GetresourcePickSubIndex()].GetResourceRef();
					ref.type_ = res->GetType();
					ref.name_ = res->GetName();
					attrs[editorResourcePicker_->GetresourcePickSubIndex()] = ref;
					target->SetAttribute(editorResourcePicker_->GetresourcePickIndex(), Urho3D::Variant(attrs));
					target->ApplyAttributes();
				}
			}

			PostEditAttribute(editorResourcePicker_->GetresourceTargets(), editorResourcePicker_->GetresourcePickIndex(), oldValues);
			Update(true);

			editorResourcePicker_->GetresourceTargets().Clear();
			editorResourcePicker_->SetCurrentResourcePicker(NULL);
		}

		void AttributeInspector::StoreResourcePickerPath()
		{
			// Store filter and directory for next time
			if (editorResourcePicker_->GetCurrentResourcePicker() != NULL && uiFileSelector_.NotNull())
			{
				editorResourcePicker_->GetCurrentResourcePicker()->lastPath = uiFileSelector_->GetPath();
				editorResourcePicker_->GetCurrentResourcePicker()->lastFilter = uiFileSelector_->GetFilterIndex();
			}
		}

		Urho3D::Resource* AttributeInspector::GetPickedResource(Urho3D::String resourceName)
		{
			resourceName = GetResourceNameFromFullName(resourceName);
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			Urho3D::Resource* res = cache->GetResource(editorResourcePicker_->GetCurrentResourcePicker()->typeName, resourceName);

			if (res == NULL)
				LOGWARNINGF("Cannot find resource type: %s  Name: %s ", editorResourcePicker_->GetCurrentResourcePicker()->typeName.CString(), resourceName.CString());

			return res;
		}

		Urho3D::String AttributeInspector::GetResourceNameFromFullName(const Urho3D::String& resourceName)
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			const Urho3D::Vector<Urho3D::String>& resourceDirs = cache->GetResourceDirs();

			for (unsigned int i = 0; i < resourceDirs.Size(); ++i)
			{
				if (!resourceName.ToLower().StartsWith(resourceDirs[i].ToLower()))
					continue;
				return resourceName.Substring(resourceDirs[i].Length());
			}

			return ""; // Not found
		}

		void AttributeInspector::PostEditAttribute(Urho3D::Serializable* serializable, unsigned int index)
		{
			// If a StaticModel/AnimatedModel/Skybox model was changed, apply a possibly different material list
			if (applyMaterialList_ && serializable->GetAttributes()->At(index).name_ == "Model")
			{
				Urho3D::StaticModel* staticModel = dynamic_cast<Urho3D::StaticModel*>(serializable);
				if (staticModel != NULL)
					staticModel->ApplyMaterialList();
			}
		}

		void AttributeInspector::PostEditAttribute(Urho3D::Vector<Urho3D::Serializable*>& serializables, unsigned int index, const Urho3D::Vector<Urho3D::Variant>& oldValues)
		{
			// Create undo actions for the edits
			// 		EditActionGroup group;
			// 		for (uint i = 0; i < serializables.length; ++i)
			// 		{
			// 			EditAttributeAction action;
			// 			action.Define(serializables[i], index, oldValues[i]);
			// 			group.actions.Push(action);
			// 		}
			// 		SaveEditActionGroup(group);

			// If a UI-element changing its 'Is Modal' attribute, clear the hierarchy list selection
			// 		int itemType = UIUtils::GetType(serializables[0]);
			// 		if (itemType == ITEM_UI_ELEMENT && serializables[0].attributeInfos[index].name == "Is Modal")
			// 			hierarchyList.ClearSelection();

			for (unsigned int i = 0; i < serializables.Size(); ++i)
			{
				PostEditAttribute(serializables[i], index);
				// 			if (itemType == ITEM_UI_ELEMENT)
				// 				SetUIElementModified(serializables[i]);
			}

			// 		if (itemType != ITEM_UI_ELEMENT)
			// 			SetSceneModified();
		}

	}
}