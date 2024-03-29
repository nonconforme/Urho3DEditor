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

#include "AttributeContainer.h"
#include "AttributeVariableEvents.h"
#include "AttributeVariable.h"
#include "ResourcePicker.h"
#include "UIGlobals.h"

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
#include <Urho3D/DebugNew.h>

namespace Prime
{
	namespace UI
	{
		void AttributeContainer::RegisterObject(Urho3D::Context* context)
		{
			using namespace Urho3D;
			context->RegisterFactory<AttributeContainer>();
			COPY_BASE_ATTRIBUTES(Urho3D::UIElement);
			ATTRIBUTE("Show Non Editable", bool, showNonEditableAttribute_, false, Urho3D::AM_FILE);
			ATTRIBUTE("Attr Name Width", int, attrNameWidth_, 150, Urho3D::AM_FILE);
			ATTRIBUTE("Attr Height", int, attrHeight_, 19, Urho3D::AM_FILE);
		}

		AttributeContainer::~AttributeContainer()
		{
		}

		AttributeContainer::AttributeContainer(Urho3D::Context* context) : UIElement(context)
		{
			editorResourcePicker_ = NULL;
			serializable_ = NULL;

			attrNameWidth_ = 150;
			attrHeight_ = 19;
			showNonEditableAttribute_ = false;
			SetName("AttributeContainer");
			SetEnabled(true);

			SetLayout(Urho3D::LM_VERTICAL, 4);

			titleText_ = CreateChild<Urho3D::Text>("AI_TitleText");
			titleText_->SetInternal(true);

			iconsPanel_ = titleText_->CreateChild<UIElement>("AI_IconsPanel");
			iconsPanel_->SetInternal(true);
			iconsPanel_->SetLayout(Urho3D::LM_HORIZONTAL, 2);
			iconsPanel_->SetHorizontalAlignment(Urho3D::HA_RIGHT);

			resetToDefault_ = iconsPanel_->CreateChild<Urho3D::Button>("AI_ResetToDefault");
			resetToDefault_->SetInternal(true);
			resetToDefault_->SetLayout(Urho3D::LM_HORIZONTAL);
			//resetToDefault_->SetHorizontalAlignment(HA_RIGHT);

			Urho3D::BorderImage* img = resetToDefault_->CreateChild<Urho3D::BorderImage>();
			img->SetInternal(true);

			attributeList_ = CreateChild<Urho3D::ListView>("AI_AttributeList");
			attributeList_->SetInternal(true);
		}

		void AttributeContainer::SetTitle(const Urho3D::String& title)
		{
			titleText_->SetText(title);
		}

		void AttributeContainer::OnResize()
		{
			if (iconsPanel_ && attributeList_)
			{
				//	iconsPanel_->SetWidth(GetWidth());
			}
		}

		void AttributeContainer::SetSerializableAttributes(Serializable* serializable, bool createNew)
		{
			serializable_ = serializable;
			if (serializableType_ != serializable->GetType() || createNew)
			{
				editorResourcePicker_ = GetSubsystem<ResourcePickerManager>();
				attributeList_->RemoveAllItems();
				attributes_.Clear();
				serializableType_ = serializable->GetType();
				CreateSerializableAttributes(serializable);
			}
			else if (serializableType_ == serializable->GetType())
			{
				UpdateSerializableAttributes(serializable);
			}
		}

		void AttributeContainer::CreateSerializableAttributes(Serializable* serializable)
		{
			//for (auto itr = attributes_.Begin(); itr != attributes_.End(); itr++)
			//{
			//	Urho3D::Vector<BasicAttributeUI*>& attribute = (*itr);
			//	attribute.Clear();
			//	for (auto attributeItr = attribute.Begin(); attributeItr != attribute.End(); attributeItr++)
			//	{
			//	}
			//}
			//attributes_.Clear();

			attributes_.Resize(serializable->GetNumAttributes());

			if (serializable->GetTypeName() == "ParticleEmitter")
				attributes_.Resize(serializable->GetNumAttributes());

			for (unsigned int i = 0; i < serializable->GetNumAttributes(); ++i)
			{
				Urho3D::AttributeInfo info = serializable->GetAttributes()->At(i);

				if (!showNonEditableAttribute_ && ((info.mode_ & Urho3D::AM_NOEDIT) != 0))
					continue;

				// Use the default value (could be instance's default value) of the first serializable as the default for all
				info.defaultValue_ = serializable->GetAttributeDefault(i);

				CreateAttribute(serializable, info, i, 0);
			}
		}

		void AttributeContainer::UpdateSerializableAttributes(Serializable* serializable)
		{
			for (unsigned int i = 0; i < attributes_.Size(); i++)
			{
				UpdateVariantMap(serializable, i);
				Urho3D::Vector<BasicAttributeUI*>& attrVector = attributes_[i];
				for (unsigned int j = 0; j < attrVector.Size(); j++)
				{
					BasicAttributeUI* attr = attrVector[j];
					attr->UpdateVar(serializable);

				}
			}
		}

		Urho3D::UIElement* AttributeContainer::CreateAttribute(Serializable* serializable, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex, bool suppressedSeparatedLabel)
		{
			UIElement* parent = NULL;

			Urho3D::VariantType type = info.type_;
			if (type == Urho3D::VAR_STRING || type == Urho3D::VAR_BUFFER)
			{
				StringAttributeUI* attr = StringAttributeUI::Create(serializable, info.name_, index, GetDefaultStyle());
				attributeList_->AddItem(attr);
				//	attr->SetStyle("StringAttributeUI");
				attr->SetSubIndex(subIndex);

				//	attr->SetDragDropMode(DD_TARGET);
				parent = attr;

				// Do not subscribe to continuous edits of certain attributes (script class names) to prevent unnecessary errors getting printed
				if (noTextChangedAttrs_.Find(info.name_) == noTextChangedAttrs_.End())
					attr->GetVarValueUI()->UnsubscribeFromEvent(Urho3D::E_TEXTCHANGED);

				SubscribeToEvent(attr, AEE_STRINGVARCHANGED, HANDLER(AttributeContainer, EditStringAttribute));
				attributes_[index].Insert(subIndex, attr);
			}
			else if (type == Urho3D::VAR_BOOL)
			{
				bool isUIElement = dynamic_cast<UIElement*>(serializable) != NULL;
				BoolAttributeUI* attr = BoolAttributeUI::Create(serializable, info.name_, index, GetDefaultStyle());
				parent = attr;
				attr->SetSubIndex(subIndex);
				if (info.name_ == (isUIElement ? "Is Visible" : "Is Enabled"))
				{
					SubscribeToEvent(attr, AEE_BOOLVARCHANGED, HANDLER(AttributeContainer, EditEnabledAttribute));

					attr->GetVarNameUI()->SetVisible(false);
					iconsPanel_->AddChild(attr);
				}
				else
				{
					SubscribeToEvent(attr, AEE_BOOLVARCHANGED, HANDLER(AttributeContainer, EditBoolAttribute));
					attributeList_->AddItem(attr);
				}

				//attr->SetStyle("BoolAttributeUI");


				attributes_[index].Insert(subIndex, attr);
			}

			else if ((type >= Urho3D::VAR_FLOAT && type <= Urho3D::VAR_VECTOR4) || type == Urho3D::VAR_QUATERNION || type == Urho3D::VAR_COLOR || type == Urho3D::VAR_INTVECTOR2 || type == Urho3D::VAR_INTRECT)
			{
				NumberAttributeUI* attr = NumberAttributeUI::Create(serializable, info.name_, index, type, GetDefaultStyle());
				attributeList_->AddItem(attr);
				//attr->SetStyle("BasicAttributeUI");
				for (int i = 0; i < attr->GetNumCoords(); ++i)
				{
					attr->GetVarValueUI()[i]->SetStyle("EditorAttributeEdit");
				}
				attr->SetSubIndex(subIndex);

				parent = attr;
				SubscribeToEvent(attr, AEE_NUMBERVARCHANGED, HANDLER(AttributeContainer, EditNumberAttribute));
				attributes_[index].Insert(subIndex, attr);
			}

			else if (type == Urho3D::VAR_INT)
			{
				Urho3D::Vector<Urho3D::String> enumnames;

				// get  enums names
				if (info.enumNames_ != NULL)
				{
					const char** enumPtr = info.enumNames_;
					while (*enumPtr)
					{
						enumnames.Push(Urho3D::String(*enumPtr));
						++enumPtr;
					}
				}
				// Check for enums
				if (enumnames.Empty())
				{
					// No enums, create a numeric editor
					NumberAttributeUI* attr = NumberAttributeUI::Create(serializable, info.name_, index, type, GetDefaultStyle());
					attributeList_->AddItem(attr);
					attr->SetStyle("BasicAttributeUI");
					for (unsigned int i = 0; i < attr->GetNumCoords(); ++i)
					{
						attr->GetVarValueUI()[i]->SetStyle("EditorAttributeEdit");
					}
					attr->SetSubIndex(subIndex);


					// 				 // If the attribute is a node ID, make it a drag/drop target
					// 				 if (info.name_.Contains("NodeID", false) || info.name_.Contains("Node ID", false) || (info.mode_ & AM_NODEID) != 0)
					// 					 attrEdit->SetDragDropMode(DD_TARGET);

					parent = attr;
					SubscribeToEvent(attr, AEE_NUMBERVARCHANGED, HANDLER(AttributeContainer, EditNumberAttribute));
					attributes_[index].Insert(subIndex, attr);
				}
				else
				{
					EnumAttributeUI* attr = EnumAttributeUI::Create(serializable, info.name_, index, enumnames, attributeList_->GetDefaultStyle());
					attributeList_->AddItem(attr);
					attr->SetSubIndex(subIndex);

					parent = attr;

					SubscribeToEvent(attr, AEE_ENUMVARCHANGED, HANDLER(AttributeContainer, EditEnumAttribute));

					attributes_[index].Insert(subIndex, attr);
				}
			}
			else if (type == Urho3D::VAR_RESOURCEREF)
			{
				if (!editorResourcePicker_)
					return NULL;
				Urho3D::StringHash resourceType;

				// Get the real attribute info from the serializable for the correct resource type
				Urho3D::AttributeInfo attrInfo = serializable->GetAttributes()->At(index);
				if (attrInfo.type_ == Urho3D::VAR_RESOURCEREF)
					resourceType = serializable->GetAttribute(index).GetResourceRef().type_;
				else if (attrInfo.type_ == Urho3D::VAR_RESOURCEREFLIST)
					resourceType = serializable->GetAttribute(index).GetResourceRefList().type_;
				else if (attrInfo.type_ == Urho3D::VAR_VARIANTVECTOR)
					resourceType = serializable->GetAttribute(index).GetVariantVector()[subIndex].GetResourceRef().type_;

				ResourcePicker* picker = editorResourcePicker_->GetResourcePicker(resourceType);

				if (!picker)
				{
					LOGERROR("No resource picker for type " + Urho3D::String(resourceType));
					return NULL;
				}

				ResourceRefAttributeUI* attr = ResourceRefAttributeUI::Create(serializable, info.name_, attrInfo.type_,
					resourceType, index, subIndex, attributeList_->GetDefaultStyle(), picker->actions);
				attributeList_->AddItem(attr);


				SubscribeToEvent(attr, AEE_RESREFVARCHANGED, HANDLER(AttributeContainer, EditResRefAttribute));
				parent = attr;
				attributes_[index].Insert(subIndex, attr);
			}
			else if (type == Urho3D::VAR_RESOURCEREFLIST)
			{
				unsigned int numRefs = serializable->GetAttribute(index).GetResourceRefList().names_.Size();

				// Straightly speaking the individual resource reference in the list is not an attribute of the serializable
				// However, the AttributeInfo structure is used here to reduce the number of parameters being passed in the function
				Urho3D::AttributeInfo refInfo;
				refInfo.name_ = info.name_;
				refInfo.type_ = Urho3D::VAR_RESOURCEREF;
				for (unsigned int i = 0; i < numRefs; ++i)
					CreateAttribute(serializable, refInfo, index, i, i > 0);
			}
			else if (type == Urho3D::VAR_VARIANTVECTOR)
			{
				if (editorResourcePicker_)
				{
					VectorStruct* vectorStruct = editorResourcePicker_->GetVectorStruct(serializable, index);
					if (vectorStruct == NULL)
						return NULL;
					unsigned int nameIndex = 0;

					Urho3D::Vector<Urho3D::Variant> vector = serializable->GetAttribute(index).GetVariantVector();
					for (unsigned int i = 0; i < vector.Size(); ++i)
					{
						// The individual variant in the vector is not an attribute of the serializable, the structure is reused for convenience
						Urho3D::AttributeInfo vectorInfo;
						vectorInfo.name_ = vectorStruct->variableNames[nameIndex];
						vectorInfo.type_ = vector[i].GetType();
						CreateAttribute(serializable, vectorInfo, index, i);
						++nameIndex;
						if (nameIndex >= vectorStruct->variableNames.Size())
							nameIndex = vectorStruct->restartIndex;
					}
				}
			}
			else if (type == Urho3D::VAR_VARIANTMAP)
			{
				Urho3D::Vector< BasicAttributeUI* >& varMap = attributes_[index];

				for (unsigned int j = 0; j < varMap.Size(); ++j)
					varMap[j]->Remove();

				varMap.Clear();

				Urho3D::VariantMap map = serializable->GetAttribute(index).GetVariantMap();
				Urho3D::Vector<Urho3D::StringHash> keys = map.Keys();
				for (unsigned int i = 0; i < keys.Size(); ++i)
				{
					Urho3D::String varName = GetVariableName(serializable, keys[i]);
					Urho3D::Variant value = map[keys[i]];

					// The individual variant in the map is not an attribute of the serializable, the structure is reused for convenience
					Urho3D::AttributeInfo mapInfo;
					mapInfo.name_ = varName + " (Var)";
					mapInfo.type_ = value.GetType();
					parent = CreateAttribute(serializable, mapInfo, index, i);
					// Add the variant key to the parent. We may fail to add the editor in case it is unsupported
					if (parent != NULL)
					{
						parent->SetVar("Key", keys[i].Value());
						// If variable name is not registered (i.e. it is an editor->IsInternal() variable) then hide it
						if (varName.Empty())
							parent->SetVisible(false);
					}
				}
			}

			return parent;
		}

		void AttributeContainer::UpdateAttribute(Serializable* serializable, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex, bool suppressedSeparatedLabel /*= false*/)
		{
			BasicAttributeUI* attr = attributes_[index][subIndex];
			if (attr)
			{
				attr->UpdateVar(serializable);
			}
		}


		Urho3D::ListView* AttributeContainer::GetAttributeList()
		{
			return attributeList_;
		}

		void AttributeContainer::SetIcon(Urho3D::XMLFile* iconStyle_, const Urho3D::String& iconType)
		{
			if (!iconStyle_ || iconType.Empty())
				return;

			// Check if the icon has been created before
			Urho3D::BorderImage* icon = (Urho3D::BorderImage*)titleText_->GetChild(Urho3D::String("Icon"));

			// If iconType is empty, it is a request to remove the existing icon
			if (iconType.Empty())
			{
				// Remove the icon if it exists
				if (icon != NULL)
					icon->Remove();

				// Revert back the indent but only if it is indented by this function
				if (titleText_->GetVar(INDENT_MODIFIED_BY_ICON_VAR).GetBool())
					titleText_->SetIndent(0);

				return;
			}

			// The UI element must itself has been indented to reserve the space for the icon
			if (titleText_->GetIndent() == 0)
			{
				titleText_->SetIndent(1);
				titleText_->SetVar(INDENT_MODIFIED_BY_ICON_VAR, true);
			}

			// If no icon yet then create one with the correct indent and size in respect to the UI element
			if (icon == NULL)
			{
				// The icon is placed at one indent level less than the UI element
				icon = new Urho3D::BorderImage(context_);
				icon->SetName("Icon");
				icon->SetIndent(titleText_->GetIndent() - 1);
				icon->SetFixedSize(titleText_->GetIndentWidth() - 2, 14);
				titleText_->InsertChild(0, icon);   // Ensure icon is added as the first child
			}

			// Set the icon type
			if (!icon->SetStyle(iconType, iconStyle_))
				icon->SetStyle("Unknown", iconStyle_);    // If fails then use an 'unknown' icon type
			icon->SetColor(Urho3D::Color(1, 1, 1, 1)); // Reset to enabled color
		}

		void AttributeContainer::EditStringAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			StringAttributeUI* attr = (StringAttributeUI*)eventData[StringVarChanged::P_ATTEDIT].GetPtr();
			if (attr && serializable_)
			{
				serializable_->SetAttribute(attr->GetIndex(), attr->GetVariant());
			}
		}

		void AttributeContainer::EditBoolAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			BoolAttributeUI* attr = (BoolAttributeUI*)eventData[BoolVarChanged::P_ATTEDIT].GetPtr();
			if (attr && serializable_)
			{
				serializable_->SetAttribute(attr->GetIndex(), attr->GetVariant());
			}
		}

		void AttributeContainer::EditEnumAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			EnumAttributeUI* attr = (EnumAttributeUI*)eventData[EnumVarChanged::P_ATTEDIT].GetPtr();
			if (attr && serializable_)
			{
				serializable_->SetAttribute(attr->GetIndex(), attr->GetVariant());
			}
		}

		void AttributeContainer::EditNumberAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			NumberAttributeUI* attr = (NumberAttributeUI*)eventData[NumberVarChanged::P_ATTEDIT].GetPtr();
			if (attr && serializable_)
			{
				serializable_->SetAttribute(attr->GetIndex(), attr->GetVariant());
			}
		}

		void AttributeContainer::EditEnabledAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			BoolAttributeUI* attr = (BoolAttributeUI*)eventData[BoolVarChanged::P_ATTEDIT].GetPtr();
			if (attr && serializable_)
			{
				serializable_->SetAttribute(attr->GetIndex(), attr->GetVariant());
				Urho3D::BorderImage* icon = (Urho3D::BorderImage*)titleText_->GetChild(Urho3D::String("Icon"));
				if (icon)
				{
					if (attr->GetVarValue())
						icon->SetColor(Urho3D::Color(1, 1, 1, 1)); // Reset to enabled color
					else
						icon->SetColor(Urho3D::Color(1, 0, 0, 1)); // disabled color
				}
			}
		}

		void AttributeContainer::SetNoTextChangedAttrs(const Urho3D::Vector<Urho3D::String>& noTextChangedAttrs)
		{
			noTextChangedAttrs_ = noTextChangedAttrs;
		}

		void AttributeContainer::EditResRefAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			ResourceRefAttributeUI* attr = (ResourceRefAttributeUI*)eventData[ResourceRefVarChanged::P_ATTEDIT].GetPtr();
			if (attr && serializable_)
			{
				serializable_->SetAttribute(attr->GetIndex(), attr->GetVariant());
				SetSerializableAttributes(serializable_, true);
			}
		}

		Urho3D::String AttributeContainer::GetVariableName(Urho3D::Serializable* serializable, Urho3D::StringHash hash)
		{
			// First try to get it from scene
			Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(serializable);

			if (node)
			{
				Urho3D::String name = node->GetScene()->GetVarName(hash);
				return name;
			}

			// Then from the UIElement variable names
			// 			if (name.Empty() && uiElementVarNames_.Contains(hash))
			// 				name = uiElementVarNames[hash].ToString();

			return Urho3D::String::EMPTY;   // Since this is a reverse mapping, it does not really matter from which side the name is retrieved back
		}
		void AttributeContainer::UpdateVariantMap(Serializable* serializable, unsigned int index)
		{
			Urho3D::AttributeInfo info = serializable->GetAttributes()->At(index);

			if (info.type_ == Urho3D::VAR_VARIANTMAP)
			{
				Urho3D::VariantMap map = serializable->GetAttribute(index).GetVariantMap();
				Urho3D::Vector<Urho3D::StringHash> keys = map.Keys();

				Urho3D::Vector< BasicAttributeUI* >& varMap = attributes_[index];

				for (unsigned int j = 0; j < varMap.Size(); ++j)
					varMap[j]->Remove();

				varMap.Clear();

				for (unsigned int j = 0; j < keys.Size(); ++j)
				{
					Urho3D::String varName = GetVariableName(serializable, keys[j]);
					Urho3D::Variant value = map[keys[j]];

					// The individual variant in the map is not an attribute of the serializable, the structure is reused for convenience
					Urho3D::AttributeInfo mapInfo;
					mapInfo.name_ = varName + " (Var)";
					mapInfo.type_ = value.GetType();
					UIElement* parent = CreateAttribute(serializable, mapInfo, index, j);
					// Add the variant key to the parent. We may fail to add the editor in case it is unsupported
					if (parent != NULL)
					{
						parent->SetVar("Key", keys[j].Value());
						// If variable name is not registered (i.e. it is an editor->IsInternal() variable) then hide it
						if (varName.Empty())
							parent->SetVisible(false);
					}
				}
			}
		}
		void AttributeContainer::UpdateVariantMap(Serializable* serializable)
		{
			if (!serializable)
				return;

			for (unsigned int i = 0; i < serializable->GetNumAttributes(); ++i)
			{
				Urho3D::AttributeInfo info = serializable->GetAttributes()->At(i);

				if (info.type_ == Urho3D::VAR_VARIANTMAP)
				{
					UpdateVariantMap(serializable, i);
				}
			}
		}


		Urho3D::Serializable* AttributeContainer::GetSerializable()
		{
			return serializable_;
		}

	}
}