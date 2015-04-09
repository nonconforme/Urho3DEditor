#include "EditorSelection.h"
#include "EditorState.h"

#include "../UI/UIGlobals.h"
#include "../UI/MenuBarUI.h"
#include "../UI/ToolBarUI.h"
#include "../UI/MiniToolBarUI.h"
#include "../UI/HierarchyWindow.h"
#include "../UI/AttributeInspector.h"
#include "../UI/ResourcePicker.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/IOEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/ScrollBar.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Texture.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Camera.h>

namespace Prime
{
	namespace Editor
	{

		EditorSelection::EditorSelection(Urho3D::Context* context, EditorState* editor) : Object(context),
			editUIElement_(NULL),
			editNode_(NULL),
			numEditableComponentsPerNode_(1),
			editor_(editor)
		{

		}

		EditorSelection::~EditorSelection()
		{

		}

		void EditorSelection::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<EditorSelection>();
		}

		void EditorSelection::ClearSelection()
		{
			selectedNodes_.Clear();
			selectedComponents_.Clear();
			selectedUIElements_.Clear();

			editUIElement_ = NULL;
			editNode_ = NULL;

			editNodes_.Clear();
			editComponents_.Clear();
			editUIElements_.Clear();

			numEditableComponentsPerNode_ = 1;
		}

		void EditorSelection::AddSelectedComponent(Urho3D::Component* comp)
		{
			if (comp != NULL)
				selectedComponents_.Push(Urho3D::WeakPtr<Urho3D::Component>(comp));
		}

		void EditorSelection::AddSelectedNode(Urho3D::Node* node)
		{
			if (node != NULL)
				selectedNodes_.Push(Urho3D::WeakPtr<Urho3D::Node>(node));
		}

		void EditorSelection::AddEditComponent(Urho3D::Component* comp)
		{
			if (comp != NULL)
				editComponents_.Push(Urho3D::WeakPtr<Urho3D::Component>(comp));
		}

		void EditorSelection::AddEditNode(Urho3D::Node* node)
		{
			if (node != NULL)
				editNodes_.Push(Urho3D::WeakPtr<Urho3D::Node>(node));
		}

		void EditorSelection::AddSelectedUIElement(Urho3D::UIElement* element)
		{
			if (element != NULL)
				selectedUIElements_.Push(Urho3D::WeakPtr<Urho3D::UIElement>(element));
		}

		unsigned EditorSelection::GetNumSelectedUIElements()
		{
			return selectedUIElements_.Size();
		}

		unsigned EditorSelection::GetNumSelectedComponents()
		{
			return selectedComponents_.Size();
		}

		unsigned EditorSelection::GetNumSelectedNodes()
		{
			return selectedNodes_.Size();
		}

		unsigned EditorSelection::GetNumEditComponents()
		{
			return editComponents_.Size();
		}

		unsigned EditorSelection::GetNumEditNodes()
		{
			return editNodes_.Size();
		}

		Urho3D::Node* EditorSelection::GetEditNode()
		{
			return editNode_;
		}

		void EditorSelection::SetEditNode(Urho3D::Node* node)
		{
			editNode_ = node;
		}

		void EditorSelection::SetEditUIElement(Urho3D::UIElement* element)
		{
			editUIElement_ = element;
		}

		Urho3D::UIElement* EditorSelection::GetEditUIElement()
		{
			return editUIElement_;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::UIElement>> EditorSelection::GetEditUIElements()
		{
			return editUIElements_;
		}

		void EditorSelection::SetNumEditableComponentsPerNode(unsigned int num)
		{
			numEditableComponentsPerNode_ = num;
		}

		unsigned int EditorSelection::GetNumEditableComponentsPerNode()
		{
			return numEditableComponentsPerNode_;
		}

		void EditorSelection::AddEditUIElement(Urho3D::UIElement* element)
		{
			if (element != NULL)
				editUIElements_.Push(Urho3D::WeakPtr<Urho3D::UIElement>(element));
		}

		void EditorSelection::SetCopiedNodes(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>& nodes)
		{
			_copiedNodes = nodes;
		}

		void EditorSelection::SetCopiedComponents(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& comps)
		{
			_copiedComponents = comps;
		}

		void EditorSelection::SetSelectedNodes(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>& nodes)
		{
			selectedNodes_ = nodes;
		}

		void EditorSelection::SetSelectedComponents(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& comps)
		{
			selectedComponents_ = comps;
		}

		void EditorSelection::SetSelectedUIElements(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::UIElement>>& elemets)
		{
			selectedUIElements_ = elemets;
		}

		void EditorSelection::SetEditNodes(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>& nodes)
		{
			editNodes_ = nodes;
		}

		void EditorSelection::SetEditComponents(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& comps)
		{
			editComponents_ = comps;
		}

		void EditorSelection::SetEditUIElements(Urho3D::Vector<Urho3D::WeakPtr<Urho3D::UIElement>>& elements)
		{
			editUIElements_ = elements;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>& EditorSelection::GetCopiedNodes()
		{
			return _copiedNodes;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& EditorSelection::GetCopiedComponents()
		{
			return _copiedComponents;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>& EditorSelection::GetSelectedNodes()
		{
			return selectedNodes_;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& EditorSelection::GetSelectedComponents()
		{
			return selectedComponents_;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::UIElement>>& EditorSelection::GetSelectedUIElements()
		{
			return selectedUIElements_;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>& EditorSelection::GetEditNodes()
		{
			return editNodes_;
		}

		Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& EditorSelection::GetEditComponents()
		{
			return editComponents_;
		}

		void EditorSelection::SetGlobalVarNames(const Urho3D::String& name)
		{
			globalVarNames_[name] = name;
		}

		const Urho3D::Variant& EditorSelection::GetGlobalVarNames(Urho3D::StringHash& name)
		{
			return globalVarNames_[name];
		}

		void EditorSelection::OnHierarchyListSelectionChange(const Urho3D::PODVector<Urho3D::UIElement*>& items, const Urho3D::PODVector<unsigned>& indices)
		{
			ClearSelection();

			for (unsigned int i = 0; i < indices.Size(); ++i)
			{
				unsigned int index = indices[i];
				Urho3D::UIElement* item = items[index];
				int type = item->GetVar(UI::TYPE_VAR).GetInt();
				if (type == UI::ITEM_COMPONENT)
				{
					Urho3D::Component* comp = editor_->GetListComponent(item);

					AddSelectedComponent(comp);
				}
				else if (type == UI::ITEM_NODE)
				{
					Urho3D::Node* node = editor_->GetListNode(item);

					AddSelectedNode(node);
				}
				else if (type == UI::ITEM_UI_ELEMENT)
				{
					Urho3D::UIElement* element = editor_->GetListUIElement(item);
					AddSelectedUIElement(element);
				}
			}

			// If only one node/UIElement selected, use it for editing
			if (GetNumSelectedNodes() == 1)
				editNode_ = selectedNodes_[0];


			if (GetNumSelectedUIElements() == 1)
				editUIElement_ = selectedUIElements_[0];


			// If selection contains only components, and they have a common node, use it for editing
			if (selectedNodes_.Empty() && !selectedComponents_.Empty())
			{
				Urho3D::Node* commonNode = NULL;
				for (unsigned int i = 0; i < GetNumSelectedComponents(); ++i)
				{
					if (i == 0)
						commonNode = GetSelectedComponents()[i]->GetNode();
					else
					{
						if (selectedComponents_[i]->GetNode() != commonNode)
							commonNode = NULL;
					}
				}
				editNode_ = commonNode;
			}

			// Now check if the component(s) can be edited. If many selected, must have same type or have same edit node
			if (!selectedComponents_.Empty())
			{
				if (editNode_ == NULL)
				{
					Urho3D::StringHash compType = selectedComponents_[0]->GetType();
					bool sameType = true;
					for (unsigned int i = 1; i < GetNumSelectedComponents(); ++i)
					{
						if (selectedComponents_[i]->GetType() != compType)
						{
							sameType = false;
							break;
						}
					}
					if (sameType)
						editComponents_ = selectedComponents_;
				}
				else
				{
					editComponents_ = selectedComponents_;
					numEditableComponentsPerNode_ = GetNumSelectedComponents();
				}
			}

			// If just nodes selected, and no components, show as many matching components for editing as possible
			if (!selectedNodes_.Empty() && selectedComponents_.Empty() && selectedNodes_[0]->GetNumComponents() > 0)
			{
				unsigned int count = 0;
				for (unsigned int j = 0; j < selectedNodes_[0]->GetNumComponents(); ++j)
				{
					Urho3D::StringHash compType = selectedNodes_[0]->GetComponents()[j]->GetType();
					bool sameType = true;
					for (unsigned int i = 1; i < GetNumSelectedNodes(); ++i)
					{
						if (selectedNodes_[i]->GetNumComponents() <= j || selectedNodes_[i]->GetComponents()[j]->GetType() != compType)
						{
							sameType = false;
							break;
						}
					}

					if (sameType)
					{
						++count;
						for (unsigned int i = 0; i < GetNumSelectedNodes(); ++i)
							AddEditComponent(selectedNodes_[i]->GetComponents()[j]);
					}
				}
				if (count > 1)
					numEditableComponentsPerNode_ = count;
			}

			if (selectedNodes_.Empty() && editNode_ != NULL)
				AddEditNode(editNode_);
			else
			{
				editNodes_ = selectedNodes_;

				// Cannot multi-edit on scene and node(s) together as scene and node do not share identical attributes,
				// editing via gizmo does not make too much sense either
				if (editNodes_.Size() > 1 && editNodes_[0] == editor_->GetScene())
					editNodes_.Erase(0);
			}

			if (selectedUIElements_.Empty() && editUIElement_ != NULL)
				AddEditUIElement(editUIElement_);
			else
				editUIElements_ = selectedUIElements_;


		}

	}
}