#include "HierarchyWindow.h"
#include "UIUtils.h"
#include "InputActionSystem.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UIElement.h>

namespace Prime
{
	namespace UI
	{
		HierarchyWindow::HierarchyWindow(Urho3D::Context* context) : Window(context)
		{
			normalTextColor_ = Urho3D::Color(1.0f, 1.0f, 1.0f);
			modifiedTextColor_ = Urho3D::Color(1.0f, 0.8f, 0.5f);
			nonEditableTextColor_ = Urho3D::Color(0.7f, 0.7f, 0.7f);
			nodeTextColor_ = Urho3D::Color(1.0f, 1.0f, 1.0f);
			componentTextColor_ = Urho3D::Color(0.7f, 1.0f, 0.7f);
			showInternalUIElement_ = false;
			showTemporaryObject_ = false;
			suppressSceneChanges_ = false;
			suppressUIElementChanges_ = false;

			SetLayout(Urho3D::LM_VERTICAL, 4, Urho3D::IntRect(6, 6, 6, 6));
			SetResizeBorder(Urho3D::IntRect(6, 6, 6, 6));
			SetResizable(true);
			SetName("HierarchyWindow");

			titleBar_ = CreateChild<UIElement>("HW_TitleBar");
			titleBar_->SetInternal(true);
			titleBar_->SetFixedHeight(16);
			titleBar_->SetLayoutMode(Urho3D::LM_HORIZONTAL);

			titleText_ = titleBar_->CreateChild<Urho3D::Text>("HW_TitleText");
			titleText_->SetInternal(true);
			titleText_->SetText("Hierarchy Window");

			closeButton_ = titleBar_->CreateChild<Urho3D::Button>("HW_CloseButton");
			closeButton_->SetInternal(true);

			img_ = CreateChild<BorderImage>();
			img_->SetInternal(true);

			UIElement* toolBar = CreateChild<UIElement>("HW_ToolBar");
			toolBar->SetInternal(true);
			toolBar->SetFixedHeight(17);
			toolBar->SetLayoutMode(Urho3D::LM_HORIZONTAL);
			toolBar->SetLayoutSpacing(4);

			expandButton_ = toolBar->CreateChild<Urho3D::Button>("HW_ExpandButton");
			expandButton_->SetInternal(true);
			expandButton_->SetFixedHeight(17);
			expandButton_->SetMinWidth(60);
			expandButton_->SetMaxWidth(70);
			expandButton_->SetLayoutMode(Urho3D::LM_HORIZONTAL);
			expandButton_->SetLayoutBorder(Urho3D::IntRect(1, 1, 1, 1));
			Urho3D::Text* label = expandButton_->CreateChild<Urho3D::Text>("HW_ExpandText");
			label->SetInternal(true);
			label->SetText("Expand");

			collapseButton_ = toolBar->CreateChild<Urho3D::Button>("HW_CollapseButton");
			collapseButton_->SetInternal(true);
			collapseButton_->SetFixedHeight(17);
			collapseButton_->SetMinWidth(60);
			collapseButton_->SetMaxWidth(70);
			collapseButton_->SetLayoutMode(Urho3D::LM_HORIZONTAL);
			collapseButton_->SetLayoutBorder(Urho3D::IntRect(1, 1, 1, 1));
			label = collapseButton_->CreateChild<Urho3D::Text>("HW_CollapseText");
			label->SetInternal(true);
			label->SetText("Collapse");

			allCheckBox_ = toolBar->CreateChild<Urho3D::CheckBox>("HW_AllCheckBox");
			allCheckBox_->SetInternal(true);
			allCheckBox_->SetFixedHeight(15);
			allCheckBox_->SetMinWidth(25);
			allCheckBox_->SetMaxWidth(45);
			allCheckBox_->SetLayoutMode(Urho3D::LM_HORIZONTAL);
			allCheckBox_->SetLayoutBorder(Urho3D::IntRect(5, 1, 1, 1));
			allCheckBox_->SetIndentSpacing(30);
			allCheckBox_->SetIndent(1);
			label = allCheckBox_->CreateChild<Urho3D::Text>("HW_AllText");
			label->SetInternal(true);
			label->SetText("All");

			hierarchyList_ = CreateChild<Urho3D::ListView>("HW_ListView");
			hierarchyList_->SetInternal(true);
			hierarchyList_->SetName("HierarchyList");
			hierarchyList_->SetHighlightMode(Urho3D::HM_ALWAYS);
			hierarchyList_->SetMultiselect(true);

			// Set selection to happen on click end, so that we can drag nodes to the inspector without resetting the inspector view
			hierarchyList_->SetSelectOnClickEnd(true);

			// Set drag & drop target mode on the node list background, which is used to parent nodes back to the root node
			hierarchyList_->GetContentElement()->SetDragDropMode(Urho3D::DD_TARGET);
			hierarchyList_->GetScrollPanel()->SetDragDropMode(Urho3D::DD_TARGET);

			SubscribeToEvent(closeButton_, Urho3D::E_RELEASED, HANDLER(HierarchyWindow, HideHierarchyWindow));
			SubscribeToEvent(expandButton_, Urho3D::E_RELEASED, HANDLER(HierarchyWindow, ExpandCollapseHierarchy));
			SubscribeToEvent(collapseButton_, Urho3D::E_RELEASED, HANDLER(HierarchyWindow, ExpandCollapseHierarchy));

			SubscribeToEvent(hierarchyList_, Urho3D::E_SELECTIONCHANGED, HANDLER(HierarchyWindow, HandleHierarchyListSelectionChange));
			SubscribeToEvent(hierarchyList_, Urho3D::E_ITEMDOUBLECLICKED, HANDLER(HierarchyWindow, HandleHierarchyListDoubleClick));

			SubscribeToEvent(Urho3D::E_DRAGDROPTEST, HANDLER(HierarchyWindow, HandleDragDropTest));
			SubscribeToEvent(Urho3D::E_DRAGDROPFINISH, HANDLER(HierarchyWindow, HandleDragDropFinish));
			SubscribeToEvent(Urho3D::E_TEMPORARYCHANGED, HANDLER(HierarchyWindow, HandleTemporaryChanged));
		}

		HierarchyWindow::~HierarchyWindow()
		{

		}

		void HierarchyWindow::RegisterObject(Urho3D::Context* context)
		{
			using namespace Urho3D;
			context->RegisterFactory<HierarchyWindow>();
			COPY_BASE_ATTRIBUTES(Urho3D::Window);
			ACCESSOR_ATTRIBUTE("Suppress UIElement Changes", GetSuppressUIElementChanges, SetSuppressUIElementChanges, bool, false, Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Suppress Scene Changes", GetSuppressSceneChanges, SetSuppressSceneChanges, bool, false, Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Show Temporary Object", GetShowTemporaryObject, SetShowTemporaryObject, bool, false, Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Show Internal UIElement", GetShowInternalUIElement, SetShowInternalUIElement, bool, false, Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Component Text Color", GetComponentTextColor, SetComponentTextColor, Urho3D::Color, Urho3D::Color(0.7f, 1.0f, 0.7f), Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Node Text Color", GetNodeTextColor, SetNodeTextColor, Urho3D::Color, Urho3D::Color(1.0f, 1.0f, 1.0f), Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Modified Text Color", GetModifiedTextColor, SetModifiedTextColor, Urho3D::Color, Urho3D::Color(1.0f, 0.8f, 0.5f), Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Non Editable Text Color", GetNonEditableTextColor, SetNonEditableTextColor, Urho3D::Color, Urho3D::Color(0.7f, 0.7f, 0.7f), Urho3D::AM_DEFAULT);
			ACCESSOR_ATTRIBUTE("Normal Text Color", GetNormalTextColor, SetNormalTextColor, Urho3D::Color, Urho3D::Color(1.0f, 1.0f, 1.0f), Urho3D::AM_DEFAULT);

		}

		void HierarchyWindow::HideHierarchyWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			SetVisible(false);
		}

		void HierarchyWindow::ExpandCollapseHierarchy(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Released;

			Urho3D::Button* button = dynamic_cast<Urho3D::Button*>(eventData[P_ELEMENT].GetPtr());
			bool enable = button == expandButton_;

			bool all = allCheckBox_->IsChecked();
			allCheckBox_->SetChecked(false);    // Auto-reset

			const Urho3D::PODVector<unsigned int> selections = hierarchyList_->GetSelections();
			for (unsigned int i = 0; i < selections.Size(); ++i)
				hierarchyList_->Expand(selections[i], enable, all);
		}

		void HierarchyWindow::HandleHierarchyListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			EnableToolButtons(hierarchyList_->GetSelections().Size() > 0);
			//editor_->OnHierarchyListSelectionChange(hierarchyList_->GetItems(), hierarchyList_->GetSelections());
		}

		void HierarchyWindow::HandleHierarchyListDoubleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ItemDoubleClicked;

			// 		UIElement* item = dynamic_cast<UIElement*>(eventData[P_ITEM].GetPtr());
			// 		editor_->OnHierarchyListDoubleClick(item);
		}

		void HierarchyWindow::HandleDragDropTest(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::DragDropTest;

			// 		UIElement* source = dynamic_cast<UIElement*>(eventData[P_SOURCE].GetPtr());
			// 		UIElement* target = dynamic_cast<UIElement*>(eventData[P_TARGET].GetPtr());
			// 		int itemType;
			// 		eventData[P_ACCEPT] = TestDragDrop(source, target, itemType);
		}

		void HierarchyWindow::HandleDragDropFinish(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::DragDropFinish;
		}

		void HierarchyWindow::HandleTemporaryChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::TemporaryChanged;
		}

		bool HierarchyWindow::TestDragDrop(UIElement* source, UIElement* target, int& itemType)
		{

			return false;
		}

		void HierarchyWindow::EnableToolButtons(bool enable)
		{
			expandButton_->SetEnabled(enable);
			collapseButton_->SetEnabled(enable);
			allCheckBox_->SetEnabled(enable);

			if (enable)
			{
				expandButton_->GetChild(0)->SetColor(normalTextColor_);
				collapseButton_->GetChild(0)->SetColor(normalTextColor_);
				allCheckBox_->GetChild(0)->SetColor(normalTextColor_);
			}
			else
			{
				expandButton_->GetChild(0)->SetColor(nonEditableTextColor_);
				collapseButton_->GetChild(0)->SetColor(nonEditableTextColor_);
				allCheckBox_->GetChild(0)->SetColor(nonEditableTextColor_);
			}
		}

		void HierarchyWindow::SetVisibleToolButtons(bool visible)
		{
			expandButton_->SetVisible(visible);
			collapseButton_->SetVisible(visible);
			allCheckBox_->SetVisible(visible);
			if (visible)
			{
				SubscribeToEvent(closeButton_, Urho3D::E_RELEASED, HANDLER(HierarchyWindow, HideHierarchyWindow));
				SubscribeToEvent(expandButton_, Urho3D::E_RELEASED, HANDLER(HierarchyWindow, ExpandCollapseHierarchy));
				SubscribeToEvent(collapseButton_, Urho3D::E_RELEASED, HANDLER(HierarchyWindow, ExpandCollapseHierarchy));
			}
			else
			{
				UnsubscribeFromEvent(closeButton_, Urho3D::E_RELEASED);
				UnsubscribeFromEvent(expandButton_, Urho3D::E_RELEASED);
				UnsubscribeFromEvent(collapseButton_, Urho3D::E_RELEASED);
			}
		}
		void HierarchyWindow::HandleNodeAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::NodeAdded;

			if (suppressSceneChanges_)
				return;

			Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(eventData[P_NODE].GetPtr());
			if (showTemporaryObject_ || !node->IsTemporary())
				UpdateHierarchyItem(node);
		}

		void HierarchyWindow::HandleNodeRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::NodeRemoved;
			if (suppressSceneChanges_)
				return;
			Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(eventData[P_NODE].GetPtr());
			unsigned int index = GetListIndex(node);
			UpdateHierarchyItem(index, NULL, NULL);
		}

		void HierarchyWindow::HandleComponentAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ComponentAdded;
			if (suppressSceneChanges_)
				return;
			// Insert the newly added component at last component position but before the first child node position of the parent node
			Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(eventData[P_NODE].GetPtr());
			Urho3D::Component* component = dynamic_cast<Urho3D::Component*>(eventData[P_COMPONENT].GetPtr());

			if (showTemporaryObject_ || !component->IsTemporary())
			{
				unsigned int nodeIndex = GetListIndex(node);
				if (nodeIndex != NO_ITEM)
				{
					unsigned int index = node->GetNumChildren() > 0 ? GetListIndex(node->GetChildren()[0]) : Urho3D::M_MAX_UNSIGNED;
					UpdateHierarchyItem(index, component, hierarchyList_->GetItems()[nodeIndex]);
				}
			}
		}

		void HierarchyWindow::HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ComponentRemoved;
			if (suppressSceneChanges_)
				return;
			Urho3D::Component* component = dynamic_cast<Urho3D::Component*>(eventData[P_COMPONENT].GetPtr());
			unsigned int index = GetComponentListIndex(component);
			if (index != NO_ITEM)
				hierarchyList_->RemoveItem(index);
		}

		void HierarchyWindow::HandleNodeNameChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::NodeNameChanged;

			if (suppressSceneChanges_)
				return;

			Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(eventData[P_NODE].GetPtr());
			UpdateHierarchyItemText(GetListIndex(node), node->IsEnabled(), UIUtils::GetNodeTitle(node));
		}

		void HierarchyWindow::HandleNodeEnabledChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::NodeEnabledChanged;
			if (suppressSceneChanges_)
				return;

			Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(eventData[P_NODE].GetPtr());
			UpdateHierarchyItemText(GetListIndex(node), node->IsEnabled());

		}

		void HierarchyWindow::HandleComponentEnabledChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ComponentEnabledChanged;
			if (suppressSceneChanges_)
				return;

			Urho3D::Component* component = dynamic_cast<Urho3D::Component*>(eventData[P_COMPONENT].GetPtr());
			UpdateHierarchyItemText(GetComponentListIndex(component), component->IsEnabledEffective());

		}

		void HierarchyWindow::HandleUIElementNameChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::NameChanged;

			if (suppressUIElementChanges_)
				return;
			UIElement* element = dynamic_cast<UIElement*>(eventData[P_ELEMENT].GetPtr());
			UpdateHierarchyItemText(GetListIndex(element), element->IsVisible(), UIUtils::GetUIElementTitle(element));
		}

		void HierarchyWindow::HandleUIElementVisibilityChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::VisibleChanged;
			if (suppressUIElementChanges_)
				return;
			UIElement* element = dynamic_cast<UIElement*>(eventData[P_ELEMENT].GetPtr());
			UpdateHierarchyItemText(GetListIndex(element), element->IsVisible());

		}

		void HierarchyWindow::HandleUIElementAttributeChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

			// Do not refresh the attribute inspector while the attribute is being edited via the attribute-editors
			if (suppressUIElementChanges_)
				return;
			Urho3D::UIElement* element = dynamic_cast<UIElement*>(eventData[Urho3D::Positioned::P_ELEMENT].GetPtr());
			// 		for (uint i = 0; i < editUIElements.length; ++i)
			// 		{
			// 			if (editUIElements[i] is element)
			// 				attributesDirty = true;
			// 		}
		}

		void HierarchyWindow::HandleUIElementAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ElementAdded;
			if (suppressUIElementChanges_)
				return;
			UIElement* element = dynamic_cast<UIElement*>(eventData[P_ELEMENT].GetPtr());
			if ((showInternalUIElement_ || !element->IsInternal()) && (showTemporaryObject_ || !element->IsTemporary()))
				UpdateHierarchyItem(element);
		}

		void HierarchyWindow::HandleUIElementRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ElementRemoved;
			if (suppressUIElementChanges_)
				return;
			UIElement* element = (UIElement*)eventData[P_ELEMENT].GetPtr();
			UpdateHierarchyItem(GetListIndex(element), NULL, NULL);
		}

		void HierarchyWindow::UpdateHierarchyItemText(unsigned int itemIndex, bool iconEnabled, const Urho3D::String& textTitle /*= NO_CHANGE*/)
		{
			Urho3D::Text* text = (Urho3D::Text*)hierarchyList_->GetItem(itemIndex);
			if (text == NULL)
				return;

			UIUtils::SetIconEnabledColor(text, iconEnabled);

			if (textTitle != NO_CHANGE)
				text->SetText(textTitle);
		}
		void HierarchyWindow::UpdateHierarchyItem(Urho3D::Serializable* serializable, bool clear /*= false*/)
		{
			if (clear)
			{

				hierarchyList_->ClearSelection();
				/// \todo 
				// Remove the current selection before updating the list item (in turn trigger an update on the attribute editor)
				// Clear copybuffer when whole window refreshed

			}

			// In case of item's parent is not found in the hierarchy list then the item will be inserted at the list root level
			Urho3D::Serializable* parent;
			switch (UIUtils::GetType(serializable))
			{
			case ITEM_NODE:
				parent = static_cast<Urho3D::Node*>(serializable)->GetParent();
				break;

			case ITEM_COMPONENT:
				parent = static_cast<Urho3D::Component*>(serializable)->GetNode();
				break;

			case ITEM_UI_ELEMENT:
				parent = static_cast<Urho3D::UIElement*>(serializable)->GetParent();
				break;

			default:
				break;
			}
			UIElement* parentItem = hierarchyList_->GetItem(GetListIndex(parent));
			UpdateHierarchyItem(GetListIndex(serializable), serializable, parentItem);
		}

		void HierarchyWindow::SetTitleBarVisible(bool show)
		{
			img_->SetVisible(show);
			titleBar_->SetVisible(show);
		}

		void HierarchyWindow::ClearListView()
		{
			hierarchyList_->RemoveAllItems();
		}

		void HierarchyWindow::SetTitle(const Urho3D::String& title)
		{
			titleText_->SetText(title);
		}

		unsigned int HierarchyWindow::UpdateHierarchyItem(unsigned int itemIndex, Serializable* serializable, UIElement* parentItem)
		{
			// Whenever we're updating, disable layout update to optimize speed
			hierarchyList_->GetContentElement()->DisableLayoutUpdate();

			if (serializable == NULL)
			{
				hierarchyList_->RemoveItem(itemIndex);
				hierarchyList_->GetContentElement()->EnableLayoutUpdate();
				hierarchyList_->GetContentElement()->UpdateLayout();
				return itemIndex;
			}

			int itemType = UIUtils::GetType(serializable);
			Urho3D::Variant id = UIUtils::GetID(serializable, itemType);

			// Remove old item if exists
			if (itemIndex < hierarchyList_->GetNumItems() && UIUtils::MatchID(hierarchyList_->GetItem(itemIndex), id, itemType))
				hierarchyList_->RemoveItem(itemIndex);

			Urho3D::Text* text = new Urho3D::Text(context_);
			hierarchyList_->InsertItem(itemIndex, text, parentItem);
			text->SetStyle("FileSelectorListText");

			if (serializable->GetType() == SCENE_TYPE || serializable == mainUI_.Get())
				// The root node (scene) and editor's root UIElement cannot be moved by drag and drop
				text->SetDragDropMode(Urho3D::DD_TARGET);
			else
				// Internal UIElement is not able to participate in drag and drop action
				text->SetDragDropMode(itemType == ITEM_UI_ELEMENT && static_cast<UIElement*>(serializable)->IsInternal() ? Urho3D::DD_DISABLED : Urho3D::DD_SOURCE_AND_TARGET);

			// Advance the index for the child items
			if (itemIndex == Urho3D::M_MAX_UNSIGNED)
				itemIndex = hierarchyList_->GetNumItems();
			else
				++itemIndex;

			Urho3D::String iconType = serializable->GetTypeName();
			if (serializable == mainUI_.Get())
				iconType = "Root" + iconType;

			if (iconStyle_)
				UIUtils::IconizeUIElement(context_, iconStyle_, text, iconType);


			SetID(text, serializable, itemType);
			switch (itemType)
			{
			case ITEM_NODE:
			{
				Urho3D::Node* node = static_cast<Urho3D::Node*>(serializable);

				text->SetText(UIUtils::GetNodeTitle(node));
				text->SetColor(nodeTextColor_);
				UIUtils::SetIconEnabledColor(text, node->IsEnabled());

				// Update components first
				for (unsigned int i = 0; i < node->GetNumComponents(); ++i)
				{
					Urho3D::Component* component = node->GetComponents()[i];
					if (showTemporaryObject_ || !component->IsTemporary())
						AddComponentItem(itemIndex++, component, text);
				}

				// Then update child nodes recursively
				for (unsigned int i = 0; i < node->GetNumChildren(); ++i)
				{
					Urho3D::Node* childNode = node->GetChildren()[i];
					if (showTemporaryObject_ || !childNode->IsTemporary())
						itemIndex = UpdateHierarchyItem(itemIndex, childNode, text);
				}

				break;
			}

			case ITEM_COMPONENT:
			{
				Urho3D::Component* component = static_cast<Urho3D::Component*>(serializable);
				text->SetText(UIUtils::GetComponentTitle(component));
				text->SetColor(componentTextColor_);
				UIUtils::SetIconEnabledColor(text, component->IsEnabledEffective());
				break;
			}

			case ITEM_UI_ELEMENT:
			{
				UIElement* element = static_cast<UIElement*>(serializable);

				text->SetText(UIUtils::GetUIElementTitle(element));
				UIUtils::SetIconEnabledColor(text, element->IsVisible());

				// Update child elements recursively
				for (unsigned int i = 0; i < element->GetNumChildren(); ++i)
				{
					UIElement* childElement = element->GetChildren()[i];
					if ((showInternalUIElement_ || !childElement->IsInternal()) && (showTemporaryObject_ || !childElement->IsTemporary()))
						itemIndex = UpdateHierarchyItem(itemIndex, childElement, text);
				}

				break;
			}

			default:
				break;
			}

			// Re-enable layout update (and do manual layout) now
			hierarchyList_->GetContentElement()->EnableLayoutUpdate();
			hierarchyList_->GetContentElement()->UpdateLayout();

			return itemIndex;
		}

		void HierarchyWindow::UpdateDirtyUI()
		{
			// Perform hierarchy selection latently after the new selections are finalized (used in undo/redo action)
			if (!hierarchyUpdateSelections_.Empty())
			{
				hierarchyList_->SetSelections(hierarchyUpdateSelections_);
				hierarchyUpdateSelections_.Clear();
			}
		}

		void HierarchyWindow::SetID(Urho3D::Text* text, Urho3D::Serializable* serializable, int itemType /*= ITEM_NONE*/)
		{
			// If item type is not provided, auto detect it
			if (itemType == ITEM_NONE)
				itemType = UIUtils::GetType(serializable);

			text->SetVar(TYPE_VAR, Urho3D::Variant(itemType));

			text->SetVar(ID_VARS[itemType], UIUtils::GetID(serializable, itemType));

			// Set node ID as drag and drop content for node ID editing
			if (itemType == ITEM_NODE)
				text->SetVar(DRAGDROPCONTENT_VAR, Urho3D::String(text->GetVar(NODE_ID_VAR).GetUInt()));

			switch (itemType)
			{
			case ITEM_COMPONENT:
				text->SetVar(NODE_ID_VAR, static_cast<Urho3D::Component*>(serializable)->GetNode()->GetID());
				break;

			case ITEM_UI_ELEMENT:
				// Subscribe to UI-element events
				SubscribeToEvent(serializable, Urho3D::E_NAMECHANGED, HANDLER(HierarchyWindow, HandleUIElementNameChanged));
				SubscribeToEvent(serializable, Urho3D::E_VISIBLECHANGED, HANDLER(HierarchyWindow, HandleUIElementVisibilityChanged));
				SubscribeToEvent(serializable, Urho3D::E_RESIZED, HANDLER(HierarchyWindow, HandleUIElementAttributeChanged));
				SubscribeToEvent(serializable, Urho3D::E_POSITIONED, HANDLER(HierarchyWindow, HandleUIElementAttributeChanged));
				break;

			default:
				break;
			}
		}
		void HierarchyWindow::SetScene(Urho3D::Scene* scene)
		{
			if (scene != NULL)
			{
				UpdateHierarchyItem(scene);
				SubscribeToEvent(scene, Urho3D::E_NODEADDED, HANDLER(HierarchyWindow, HandleNodeAdded));
				SubscribeToEvent(scene, Urho3D::E_NODEREMOVED, HANDLER(HierarchyWindow, HandleNodeRemoved));
				SubscribeToEvent(scene, Urho3D::E_COMPONENTADDED, HANDLER(HierarchyWindow, HandleComponentAdded));
				SubscribeToEvent(scene, Urho3D::E_COMPONENTREMOVED, HANDLER(HierarchyWindow, HandleComponentRemoved));
				SubscribeToEvent(scene, Urho3D::E_NODENAMECHANGED, HANDLER(HierarchyWindow, HandleNodeNameChanged));
				SubscribeToEvent(scene, Urho3D::E_NODEENABLEDCHANGED, HANDLER(HierarchyWindow, HandleNodeEnabledChanged));
				SubscribeToEvent(scene, Urho3D::E_COMPONENTENABLEDCHANGED, HANDLER(HierarchyWindow, HandleComponentEnabledChanged));
			}
			else
			{
				UnsubscribeFromEvent(scene_, Urho3D::E_NODEADDED);
				UnsubscribeFromEvent(scene_, Urho3D::E_NODEREMOVED);
				UnsubscribeFromEvent(scene_, Urho3D::E_COMPONENTADDED);
				UnsubscribeFromEvent(scene_, Urho3D::E_COMPONENTREMOVED);
				UnsubscribeFromEvent(scene_, Urho3D::E_NODENAMECHANGED);
				UnsubscribeFromEvent(scene_, Urho3D::E_NODEENABLEDCHANGED);
				UnsubscribeFromEvent(scene_, Urho3D::E_COMPONENTENABLEDCHANGED);
				unsigned int index = GetListIndex(scene_);
				UpdateHierarchyItem(index, NULL, NULL);
			}
			scene_ = scene;
		}

		void HierarchyWindow::SetUIElement(UIElement* rootui)
		{
			if (rootui != NULL)
			{
				UpdateHierarchyItem(rootui);
				// Subscribe to UI-element events
				SubscribeToEvent(rootui, Urho3D::E_NAMECHANGED, HANDLER(HierarchyWindow, HandleUIElementNameChanged));
				SubscribeToEvent(rootui, Urho3D::E_VISIBLECHANGED, HANDLER(HierarchyWindow, HandleUIElementVisibilityChanged));
				SubscribeToEvent(rootui, Urho3D::E_RESIZED, HANDLER(HierarchyWindow, HandleUIElementAttributeChanged));
				SubscribeToEvent(rootui, Urho3D::E_POSITIONED, HANDLER(HierarchyWindow, HandleUIElementAttributeChanged));

				// This is needed to distinguish our own element events from Editor's UI element events
				rootui->SetElementEventSender(true);
				SubscribeToEvent(rootui, Urho3D::E_ELEMENTADDED, HANDLER(HierarchyWindow, HandleUIElementAdded));
				SubscribeToEvent(rootui, Urho3D::E_ELEMENTREMOVED, HANDLER(HierarchyWindow, HandleUIElementRemoved));
			}
			else
			{
				if (mainUI_.Get())
					mainUI_->SetElementEventSender(false);
				UnsubscribeFromEvent(mainUI_, Urho3D::E_ELEMENTADDED);
				UnsubscribeFromEvent(mainUI_, Urho3D::E_ELEMENTREMOVED);
				UnsubscribeFromEvent(mainUI_, Urho3D::E_NAMECHANGED);
				UnsubscribeFromEvent(mainUI_, Urho3D::E_VISIBLECHANGED);
				UnsubscribeFromEvent(mainUI_, Urho3D::E_RESIZED);
				UnsubscribeFromEvent(mainUI_, Urho3D::E_POSITIONED);
				unsigned int index = GetListIndex(mainUI_);
				UpdateHierarchyItem(index, NULL, NULL);
			}
			mainUI_ = rootui;
		}

		void HierarchyWindow::SetIconStyle(Urho3D::XMLFile* iconstyle)
		{
			iconStyle_ = iconstyle;
		}

		const Urho3D::String& HierarchyWindow::GetTitle()
		{
			return titleText_->GetText();
		}

		unsigned int HierarchyWindow::GetListIndex(Serializable* serializable)
		{
			if (serializable == NULL)
				return NO_ITEM;

			int itemType = UIUtils::GetType(serializable);
			Urho3D::Variant id = UIUtils::GetID(serializable, itemType);

			unsigned int numItems = hierarchyList_->GetNumItems();
			for (unsigned int i = 0; i < numItems; ++i)
			{
				if (UIUtils::MatchID(hierarchyList_->GetItem(i), id, itemType))
					return i;
			}

			return NO_ITEM;
		}

		unsigned int HierarchyWindow::GetComponentListIndex(Urho3D::Component* component)
		{
			if (component == NULL)
				return NO_ITEM;

			unsigned int numItems = hierarchyList_->GetNumItems();
			for (unsigned int i = 0; i < numItems; ++i)
			{
				UIElement* item = hierarchyList_->GetItems()[i];
				if (item->GetVar(TYPE_VAR).GetInt() == ITEM_COMPONENT && item->GetVar(COMPONENT_ID_VAR).GetUInt() == component->GetID())
					return i;
			}

			return NO_ITEM;
		}

		Urho3D::Scene* HierarchyWindow::GetScene()
		{
			return scene_;
		}

		Urho3D::UIElement* HierarchyWindow::GetUIElement()
		{
			return mainUI_;
		}

		Urho3D::XMLFile* HierarchyWindow::GetIconStyle()
		{
			return iconStyle_;
		}

		Urho3D::ListView* HierarchyWindow::GetHierarchyList()
		{
			return hierarchyList_;
		}

		Urho3D::UIElement* HierarchyWindow::GetTitleBar()
		{
			return titleBar_;
		}

		void HierarchyWindow::AddComponentItem(unsigned int compItemIndex, Urho3D::Component* component, Urho3D::UIElement* parentItem)
		{
			Urho3D::Text* text = new Urho3D::Text(context_);
			hierarchyList_->InsertItem(compItemIndex, text, parentItem);
			text->SetStyle("FileSelectorListText");
			text->SetVar(TYPE_VAR, ITEM_COMPONENT);
			text->SetVar(NODE_ID_VAR, component->GetNode()->GetID());
			text->SetVar(COMPONENT_ID_VAR, component->GetID());
			text->SetText(UIUtils::GetComponentTitle(component));
			text->SetColor(componentTextColor_);
			// Components currently act only as drag targets
			text->SetDragDropMode(Urho3D::DD_TARGET);
			if (iconStyle_)
				UIUtils::IconizeUIElement(context_, iconStyle_, text, component->GetTypeName());
			UIUtils::SetIconEnabledColor(text, component->IsEnabledEffective());
		}
	}
}