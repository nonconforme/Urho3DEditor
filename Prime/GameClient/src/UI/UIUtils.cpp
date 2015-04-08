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

#include "UIUtils.h"
#include "UIGlobals.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/DebugNew.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Scene/Serializable.h>

namespace Prime
{
	namespace UI
	{

		namespace UIUtils
		{

			// UIElement does not have unique ID, so use a running number to generate a new ID each time an item is inserted into hierarchy list
			static unsigned int g_uiElementNextID = UI_ELEMENT_BASE_ID;


			void CreateDir(const Urho3D::Object* obj, const Urho3D::String& pathName, Urho3D::String baseDir)
			{
				Urho3D::FileSystem* filesystem = obj->GetSubsystem<Urho3D::FileSystem>();
				if (baseDir.Empty())
				{
					baseDir = filesystem->GetUserDocumentsDir();
				}

				Urho3D::Vector<Urho3D::String> dirs = pathName.Split('/');
				Urho3D::String subdir = baseDir;
				for (unsigned int i = 0; i < dirs.Size(); ++i)
				{
					subdir += dirs[i] + "/";
					filesystem->CreateDir(subdir);
				}
			}

			Urho3D::Menu* CreateMenu(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, const Urho3D::String& title)
			{
				Urho3D::Menu* menu = CreateMenuItem(context, uiStyle, title);
				menu->SetFixedWidth(menu->GetWidth());
				CreatePopup(context, uiStyle, menu);

				return menu;
			}

			Urho3D::Menu* CreateMenuItem(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, const Urho3D::String& title, Urho3D::EventHandler* handler /*= NULL*/, int accelKey /*= 0*/, int accelQual /*= 0*/, bool addToQuickMenu /*= true*/, Urho3D::String quickMenuText /*= ""*/)
			{
				Urho3D::Menu* menu = new Urho3D::Menu(context);
				menu->SetName(title);
				menu->SetDefaultStyle(uiStyle);
				menu->SetStyle(AUTO_STYLE);
				menu->SetLayout(Urho3D::LM_HORIZONTAL, 0, Urho3D::IntRect(8, 2, 8, 2));

				if (accelKey > 0)
					menu->SetAccelerator(accelKey, accelQual);
				if (handler != NULL)
				{
					// TODO:
					// 			Variant callb(menuCallbacks_.Size());
					// 			menu->SetVar(CALLBACK_VAR, callb);
					// 			menuCallbacks_.Push(callback);
				}

				Urho3D::Text* menuText = new Urho3D::Text(context);
				menu->AddChild(menuText);
				menuText->SetStyle("EditorMenuText");
				menuText->SetText(title);
				//TODO:
				// 		if (addToQuickMenu)
				// 			AddQuickMenuItem(callback, quickMenuText.Empty() ? title : quickMenuText);

				if (accelKey != 0)
				{
					Urho3D::UIElement* spacer = new Urho3D::UIElement(context);
					spacer->SetMinWidth(menuText->GetIndentSpacing());
					spacer->SetHeight(menuText->GetHeight());

					menu->AddChild(spacer);
					menu->AddChild(CreateAccelKeyText(context, uiStyle, accelKey, accelQual));
				}

				return menu;
			}

			Urho3D::Text* CreateAccelKeyText(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, int accelKey, int accelQual)
			{
				Urho3D::Text* accelKeyText = new Urho3D::Text(context);
				accelKeyText->SetDefaultStyle(uiStyle);
				accelKeyText->SetStyle("EditorMenuText");
				accelKeyText->SetTextAlignment(Urho3D::HA_RIGHT);

				Urho3D::String text;
				if (accelKey == Urho3D::KEY_DELETE)
					text = "Del";
				else if (accelKey == Urho3D::KEY_SPACE)
					text = "Space";
				// Cannot use range as the key constants below do not appear to be in sequence
				else if (accelKey == Urho3D::KEY_F1)
					text = "F1";
				else if (accelKey == Urho3D::KEY_F2)
					text = "F2";
				else if (accelKey == Urho3D::KEY_F3)
					text = "F3";
				else if (accelKey == Urho3D::KEY_F4)
					text = "F4";
				else if (accelKey == Urho3D::KEY_F5)
					text = "F5";
				else if (accelKey == Urho3D::KEY_F6)
					text = "F6";
				else if (accelKey == Urho3D::KEY_F7)
					text = "F7";
				else if (accelKey == Urho3D::KEY_F8)
					text = "F8";
				else if (accelKey == Urho3D::KEY_F9)
					text = "F9";
				else if (accelKey == Urho3D::KEY_F10)
					text = "F10";
				else if (accelKey == Urho3D::KEY_F11)
					text = "F11";
				else if (accelKey == Urho3D::KEY_F12)
					text = "F12";
				else if (accelKey == SHOW_POPUP_INDICATOR)
					text = ">";
				else
					text.AppendUTF8(accelKey);
				if (accelQual && Urho3D::QUAL_ALT > 0)
					text = "Alt+" + text;
				if (accelQual && Urho3D::QUAL_SHIFT > 0)
					text = "Shift+" + text;
				if (accelQual && Urho3D::QUAL_CTRL > 0)
					text = "Ctrl+" + text;
				accelKeyText->SetText(text);

				return accelKeyText;
			}

			Urho3D::Window* CreatePopup(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, Urho3D::Menu* baseMenu)
			{
				Urho3D::Window* popup = new  Urho3D::Window(context);
				popup->SetDefaultStyle(uiStyle);
				popup->SetStyle(AUTO_STYLE);
				popup->SetLayout(Urho3D::LM_VERTICAL, 1, Urho3D::IntRect(2, 6, 2, 6));
				baseMenu->SetPopup(popup);
				baseMenu->SetPopupOffset(Urho3D::IntVector2(0, baseMenu->GetHeight()));

				return popup;
			}

			void FinalizedPopupMenu(Urho3D::Window* popup)
			{
				// Find the maximum menu text width
				Urho3D::Vector<Urho3D::SharedPtr<Urho3D::UIElement> > children = popup->GetChildren();

				int maxWidth = 0;

				for (unsigned int i = 0; i < children.Size(); ++i)
				{
					Urho3D::UIElement* element = children[i];
					if (element->GetType() != MENU_TYPE)    // Skip if not menu item
						continue;

					int width = element->GetChild(0)->GetWidth();
					if (width > maxWidth)
						maxWidth = width;
				}

				// Adjust the indent spacing to slightly wider than the maximum width
				maxWidth += 20;
				for (unsigned int i = 0; i < children.Size(); ++i)
				{
					Urho3D::UIElement* element = children[i];
					if (element->GetType() != MENU_TYPE)
						continue;
					Urho3D::Menu* menu = (Urho3D::Menu*)element;

					Urho3D::Text* menuText = (Urho3D::Text*)menu->GetChild(0);
					if (menuText->GetNumChildren() == 1)    // Skip if menu text does not have accel
						menuText->GetChild(0)->SetIndentSpacing(maxWidth);

					// Adjust the popup offset taking the indentation into effect
					if (menu->GetPopup() != NULL)
						menu->SetPopupOffset(Urho3D::IntVector2(menu->GetWidth(), 0));
				}
			}

			void CreateChildDivider(Urho3D::UIElement* parent)
			{
				Urho3D::BorderImage* divider = parent->CreateChild<Urho3D::BorderImage>("Divider");
				divider->SetStyle("EditorDivider");
			}

			Urho3D::UIElement* CreateGroup(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, const Urho3D::String& title, Urho3D::LayoutMode layoutMode)
			{
				Urho3D::UIElement* group = new Urho3D::UIElement(context);
				group->SetName(title);
				group->SetDefaultStyle(uiStyle);
				group->SetLayoutMode(layoutMode);
				return group;
			}

			Urho3D::CheckBox* CreateToolBarToggle(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_, const Urho3D::String& title)
			{
				Urho3D::CheckBox* toggle = new Urho3D::CheckBox(context);
				toggle->SetName(title);
				toggle->SetDefaultStyle(uiStyle);
				toggle->SetStyle("ToolBarToggle");

				CreateToolBarIcon(context, iconStyle_, toggle);
				CreateToolTip(toggle, title, Urho3D::IntVector2(toggle->GetWidth() + 10, toggle->GetHeight() - 10));

				return toggle;
			}

			void CreateToolBarIcon(Urho3D::Context* context, Urho3D::XMLFile* iconStyle_, Urho3D::UIElement* element)
			{
				Urho3D::BorderImage* icon = new Urho3D::BorderImage(context);
				icon->SetName("Icon");
				icon->SetDefaultStyle(iconStyle_);
				icon->SetStyle(element->GetName());
				icon->SetFixedSize(30, 30);
				element->AddChild(icon);
			}

			Urho3D::UIElement* CreateToolTip(Urho3D::UIElement* parent, const Urho3D::String& title, const Urho3D::IntVector2& offset)
			{
				Urho3D::ToolTip* toolTip = parent->CreateChild<Urho3D::ToolTip>("ToolTip");
				toolTip->SetPosition(offset);

				Urho3D::BorderImage* textHolder = toolTip->CreateChild<Urho3D::BorderImage>("BorderImage");
				textHolder->SetStyle("ToolTipBorderImage");

				Urho3D::Text* toolTipText = textHolder->CreateChild<Urho3D::Text>("Text");
				toolTipText->SetStyle("ToolTipText");
				toolTipText->SetText(title);

				return toolTip;
			}

			void FinalizeGroupHorizontal(Urho3D::UIElement* group, const Urho3D::String& baseStyle)
			{
				for (unsigned int i = 0; i < group->GetNumChildren(); ++i)
				{
					Urho3D::UIElement* child = group->GetChild(i);

					if (i == 0 && i < group->GetNumChildren() - 1)
						child->SetStyle(baseStyle + "GroupLeft");
					else if (i < group->GetNumChildren() - 1)
						child->SetStyle(baseStyle + "GroupMiddle");
					else
						child->SetStyle(baseStyle + "GroupRight");
				}
				group->SetMaxSize(group->GetSize());
			}

			Urho3D::Button* CreateSmallToolBarButton(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_, const Urho3D::String& title, Urho3D::String toolTipTitle /*= ""*/)
			{
				Urho3D::Button* button = new Urho3D::Button(context);
				button->SetName(title);
				button->SetDefaultStyle(uiStyle);
				button->SetStyle("ToolBarButton");
				button->SetFixedSize(20, 20);
				CreateSmallToolBarIcon(context, iconStyle_, button);

				if (toolTipTitle.Empty())
					toolTipTitle = title;
				CreateToolTip(button, toolTipTitle, Urho3D::IntVector2(button->GetWidth() + 10, button->GetHeight() - 10));

				return button;
			}

			void CreateSmallToolBarIcon(Urho3D::Context* context, Urho3D::XMLFile* iconStyle_, Urho3D::UIElement* element)
			{
				Urho3D::BorderImage* icon = new Urho3D::BorderImage(context);
				icon->SetName("Icon");
				icon->SetDefaultStyle(iconStyle_);
				icon->SetStyle(element->GetName());
				icon->SetFixedSize(14, 14);
				element->AddChild(icon);
			}

			Urho3D::String GetUIElementTitle(Urho3D::UIElement* element)
			{
				Urho3D::String ret;

				// Only top level UI-element has this variable
				Urho3D::String modifiedStr = element->GetVar(MODIFIED_VAR).GetBool() ? "*" : "";
				ret = (element->GetName().Empty() ? element->GetTypeName() : element->GetName()) + modifiedStr + " [" + GetUIElementID(element).ToString() + "]";

				if (element->IsTemporary())
					ret += " (Temp)";

				return ret;
			}

			Urho3D::String GetComponentTitle(Urho3D::Component* component)
			{
				Urho3D::String ret = component->GetTypeName();

				if (component->GetID() >= Urho3D::FIRST_LOCAL_ID)
					ret += " (Local)";

				if (component->IsTemporary())
					ret += " (Temp)";

				return ret;
			}

			Urho3D::String GetNodeTitle(Urho3D::Node* node)
			{
				Urho3D::String ret;

				if (node->GetName().Empty())
					ret = node->GetTypeName();
				else
					ret = node->GetName();

				if (node->GetID() >= Urho3D::FIRST_LOCAL_ID)
					ret += " (Local " + Urho3D::String(node->GetID()) + ")";
				else
					ret += " (" + Urho3D::String(node->GetID()) + ")";

				if (node->IsTemporary())
					ret += " (Temp)";

				return ret;
			}

			Urho3D::Variant GetUIElementID(Urho3D::UIElement* element)
			{
				Urho3D::Variant elementID = element->GetVar(UI_ELEMENT_ID_VAR);
				if (elementID.IsEmpty())
				{
					// Generate new ID
					elementID = g_uiElementNextID++;
					// Store the generated ID
					element->SetVar(UI_ELEMENT_ID_VAR, elementID);
				}

				return elementID;
			}

			unsigned int GetID(Urho3D::Serializable* serializable, int itemType /*= ITEM_NONE*/)
			{
				// If item type is not provided, auto detect it
				if (itemType == ITEM_NONE)
					itemType = GetType(serializable);

				switch (itemType)
				{
				case ITEM_NODE:
					return static_cast<Urho3D::Node*>(serializable)->GetID();

				case ITEM_COMPONENT:
					return static_cast<Urho3D::Component*>(serializable)->GetID();

				case ITEM_UI_ELEMENT:
					return GetUIElementID(static_cast<Urho3D::UIElement*>(serializable)).GetUInt();
				}

				return Urho3D::M_MAX_UNSIGNED;
			}

			int GetType(Urho3D::Serializable* serializable)
			{
				if (dynamic_cast<Urho3D::Node*>(serializable) != NULL)
					return ITEM_NODE;
				else if (dynamic_cast<Urho3D::Component*>(serializable) != NULL)
					return ITEM_COMPONENT;
				else if (dynamic_cast<Urho3D::UIElement*>(serializable) != NULL)
					return ITEM_UI_ELEMENT;
				else
					return ITEM_NONE;
			}

			bool MatchID(Urho3D::UIElement* element, const Urho3D::Variant& id, int itemType)
			{
				return element->GetVar(TYPE_VAR).GetInt() == itemType && element->GetVar(ID_VARS[itemType]) == id;
			}

			void SetIconEnabledColor(Urho3D::UIElement* element, bool enabled, bool partial /*= false*/)
			{
				Urho3D::BorderImage* icon = (Urho3D::BorderImage*)element->GetChild(Urho3D::String("Icon"));
				if (icon != NULL)
				{
					if (partial)
					{
						icon->SetColor(Urho3D::C_TOPLEFT, Urho3D::Color(1, 1, 1, 1));
						icon->SetColor(Urho3D::C_BOTTOMLEFT, Urho3D::Color(1, 1, 1, 1));
						icon->SetColor(Urho3D::C_TOPRIGHT, Urho3D::Color(1, 0, 0, 1));
						icon->SetColor(Urho3D::C_BOTTOMRIGHT, Urho3D::Color(1, 0, 0, 1));
					}
					else
						icon->SetColor(enabled ? Urho3D::Color(1, 1, 1, 1) : Urho3D::Color(1, 0, 0, 1));
				}
			}

			void IconizeUIElement(Urho3D::Context* context, Urho3D::XMLFile* iconStyle_, Urho3D::UIElement* element, const Urho3D::String& iconType)
			{
				// Check if the icon has been created before
				Urho3D::BorderImage* icon = (Urho3D::BorderImage*)element->GetChild(Urho3D::String("Icon"));

				// If iconType is empty, it is a request to remove the existing icon
				if (iconType.Empty())
				{
					// Remove the icon if it exists
					if (icon != NULL)
						icon->Remove();

					// Revert back the indent but only if it is indented by this function
					if (element->GetVar(INDENT_MODIFIED_BY_ICON_VAR).GetBool())
						element->SetIndent(0);

					return;
				}

				// The UI element must itself has been indented to reserve the space for the icon
				if (element->GetIndent() == 0)
				{

					element->SetIndent(1);
					element->SetVar(INDENT_MODIFIED_BY_ICON_VAR, true);
				}

				// If no icon yet then create one with the correct indent and size in respect to the UI element
				if (icon == NULL)
				{
					// The icon is placed at one indent level less than the UI element
					icon = new Urho3D::BorderImage(context);
					icon->SetName("Icon");
					icon->SetIndent(element->GetIndent() - 1);
					icon->SetFixedSize(element->GetIndentWidth() - 2, 14);
					element->InsertChild(0, icon);   // Ensure icon is added as the first child
				}

				// Set the icon type
				if (!icon->SetStyle(iconType, iconStyle_))
					icon->SetStyle("Unknown", iconStyle_);    // If fails then use an 'unknown' icon type
				icon->SetColor(Urho3D::Color(1, 1, 1, 1)); // Reset to enabled color
			}

			Urho3D::Vector<Urho3D::Serializable*> ToSerializableArray(Urho3D::Vector<Urho3D::Node*> nodes)
			{
				Urho3D::Vector<Urho3D::Serializable*> serializables;
				for (unsigned int i = 0; i < nodes.Size(); ++i)
					serializables.Push(nodes[i]);
				return serializables;
			}



			Urho3D::UIElement* CreateAttributeEditorParent(Urho3D::ListView* list, const Urho3D::String& name, unsigned int index, unsigned int subIndex)
			{
				Urho3D::UIElement* editorParent = new Urho3D::UIElement(list->GetContext());
				editorParent->SetName("Edit" + Urho3D::String(index) + "_" + Urho3D::String(subIndex));
				editorParent->SetVar("Index", index);
				editorParent->SetVar("SubIndex", subIndex);
				editorParent->SetLayout(Urho3D::LM_HORIZONTAL);
				editorParent->SetFixedHeight(ATTR_HEIGHT);
				list->AddItem(editorParent);

				Urho3D::Text* attrNameText = new Urho3D::Text(list->GetContext());
				editorParent->AddChild(attrNameText);
				attrNameText->SetStyle("EditorAttributeText");
				attrNameText->SetText(name);
				attrNameText->SetFixedWidth(ATTRNAME_WIDTH);

				return editorParent;
			}

			Urho3D::UIElement* CreateAttributeEditorParentAsListChild(Urho3D::ListView* list, const Urho3D::String& name, unsigned int index, unsigned int subIndex)
			{
				Urho3D::UIElement* editorParent = new Urho3D::UIElement(list->GetContext());
				editorParent->SetName("Edit" + Urho3D::String(index) + "_" + Urho3D::String(subIndex));
				editorParent->SetVar("Index", index);
				editorParent->SetVar("SubIndex", subIndex);
				editorParent->SetLayout(Urho3D::LM_HORIZONTAL);
				list->AddChild(editorParent);

				Urho3D::UIElement* placeHolder = new Urho3D::UIElement(list->GetContext());
				placeHolder->SetName(name);
				editorParent->AddChild(placeHolder);

				return editorParent;
			}

			Urho3D::LineEdit* CreateAttributeLineEdit(Urho3D::UIElement* parent, Urho3D::Vector<Urho3D::Serializable*>& serializables, unsigned int index, unsigned int subIndex)
			{
				Urho3D::LineEdit* attrEdit = new Urho3D::LineEdit(parent->GetContext());
				parent->AddChild((Urho3D::UIElement*)attrEdit);
				attrEdit->SetStyle("EditorAttributeEdit");
				attrEdit->SetFixedHeight(ATTR_HEIGHT - 2);
				attrEdit->SetVar("Index", index);
				attrEdit->SetVar("SubIndex", subIndex);
				/// \todo 
				//	SetAttributeEditorID(attrEdit, serializables);

				return attrEdit;
			}

			Urho3D::UIElement* CreateBoolAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex)
			{
				bool isUIElement = dynamic_cast<Urho3D::UIElement*>(serializables[0]) != NULL;
				Urho3D::UIElement* parent;
				if (info.name_ == (isUIElement ? "Is Visible" : "Is Enabled"))
					parent = CreateAttributeEditorParentAsListChild(list, info.name_, index, subIndex);
				else
					parent = CreateAttributeEditorParent(list, info.name_, index, subIndex);

				Urho3D::CheckBox* attrEdit = new Urho3D::CheckBox(list->GetContext());
				parent->AddChild(attrEdit);
				attrEdit->SetStyle(AUTO_STYLE);
				attrEdit->SetVar("Index", index);
				attrEdit->SetVar("SubIndex", subIndex);

				/// \todo 
				// 			SetAttributeEditorID(attrEdit, serializables);
				// 			SubscribeToEvent(attrEdit, E_TOGGLED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));

				return parent;
			}

			Urho3D::UIElement* CreateStringAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex)
			{
				Urho3D::UIElement* parent = CreateAttributeEditorParent(list, info.name_, index, subIndex);
				Urho3D::LineEdit* attrEdit = CreateAttributeLineEdit(parent, serializables, index, subIndex);
				attrEdit->SetDragDropMode(Urho3D::DD_TARGET);
				/// \todo 
				// Do not subscribe to continuous edits of certain attributes (script class names) to prevent unnecessary errors getting printed
				// 			if (noTextChangedAttrs.Find(info.name_) == noTextChangedAttrs.End())
				// 				SubscribeToEvent(attrEdit, E_TEXTCHANGED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));
				// 			SubscribeToEvent(attrEdit, E_TEXTFINISHED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));

				return parent;
			}

			Urho3D::UIElement* CreateNumAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex)
			{
				Urho3D::UIElement* parent = CreateAttributeEditorParent(list, info.name_, index, subIndex);
				Urho3D::VariantType type = info.type_;
				unsigned int numCoords = type - Urho3D::VAR_FLOAT + 1;
				if (type == Urho3D::VAR_QUATERNION)
					numCoords = 3;
				else if (type == Urho3D::VAR_COLOR || type == Urho3D::VAR_INTRECT)
					numCoords = 4;
				else if (type == Urho3D::VAR_INTVECTOR2)
					numCoords = 2;

				// 			for (unsigned int i = 0; i < numCoords; ++i)
				// 			{
				// 				LineEdit* attrEdit = CreateAttributeLineEdit(parent, serializables, index, subIndex);
				// 				attrEdit->SetVar("Coordinate", i);
				// 				SubscribeToEvent(attrEdit, E_TEXTCHANGED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));
				// 				SubscribeToEvent(attrEdit, E_TEXTFINISHED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));
				// 			}

				return parent;
			}

			Urho3D::UIElement* CreateIntAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex)
			{
				Urho3D::UIElement* parent = CreateAttributeEditorParent(list, info.name_, index, subIndex);
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
					Urho3D::LineEdit* attrEdit = CreateAttributeLineEdit(parent, serializables, index, subIndex);
					//	SubscribeToEvent(attrEdit, E_TEXTCHANGED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));
					//	SubscribeToEvent(attrEdit, E_TEXTFINISHED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));
					// If the attribute is a node ID, make it a drag/drop target
					if (info.name_.Contains("NodeID", false) || info.name_.Contains("Node ID", false) || (info.mode_ & Urho3D::AM_NODEID) != 0)
						attrEdit->SetDragDropMode(Urho3D::DD_TARGET);
				}
				else
				{
					Urho3D::DropDownList* attrEdit = new Urho3D::DropDownList(list->GetContext());
					parent->AddChild(attrEdit);
					attrEdit->SetStyle(AUTO_STYLE);
					attrEdit->SetFixedHeight(ATTR_HEIGHT - 2);
					attrEdit->SetResizePopup(true);
					attrEdit->SetPlaceholderText(STRIKED_OUT);
					attrEdit->SetVar("Index", index);
					attrEdit->SetVar("SubIndex", subIndex);
					attrEdit->SetLayout(Urho3D::LM_HORIZONTAL, 0, Urho3D::IntRect(4, 1, 4, 1));
					//	SetAttributeEditorID(attrEdit, serializables);

					for (unsigned i = 0; i < enumnames.Size(); i++)
					{
						Urho3D::Text* choice = new Urho3D::Text(list->GetContext());
						attrEdit->AddItem(choice);
						choice->SetStyle("EditorEnumAttributeText");
						choice->SetText(enumnames[i]);

					}

					// 				for (unsigned int i = 0; i < info.enumNames.Size(); ++i)
					// 				{
					// 					Text* choice = Text();
					// 					attrEdit.AddItem(choice);
					// 					choice.style = "EditorEnumAttributeText";
					// 					choice.text = info.enumNames[i];
					// 				}
					//SubscribeToEvent(attrEdit, E_ITEMSELECTED, HANDLER(EditorAttributeInspectorWindow, EditAttribute));
				}

				return parent;
			}

			Urho3D::String ExtractFileName(Urho3D::VariantMap& eventData, bool forSave /*= false*/)
			{
				using namespace Urho3D::FileSelected;
				Urho3D::String fileName;

				// Check for OK
				if (eventData[P_OK].GetBool())
				{
					Urho3D::String filter = eventData[P_FILTER].GetString();
					fileName = eventData[P_FILENAME].GetString();
					// Add default extension for saving if not specified
					if (GetExtension(fileName).Empty() && forSave && filter != "*.*")
						fileName = fileName + filter.Substring(1);
				}
				return fileName;
			}

			void SetAttributeEditorID(Urho3D::UIElement* attrEdit, Urho3D::Vector<Urho3D::Serializable*>& serializables)
			{
				if (serializables.Size() == 0)
					return;

				// All target serializables must be either nodes, ui-elements, or components
				Urho3D::Vector<Urho3D::Variant> ids;
				switch (UIUtils::GetType(serializables[0]))
				{
				case ITEM_NODE:
					for (unsigned int i = 0; i < serializables.Size(); ++i)
						ids.Push(dynamic_cast<Urho3D::Node*>(serializables[i])->GetID());
					attrEdit->SetVar(NODE_IDS_VAR, ids);
					break;

				case ITEM_COMPONENT:
					for (unsigned int i = 0; i < serializables.Size(); ++i)
						ids.Push(dynamic_cast<Urho3D::Component*>(serializables[i])->GetID());
					attrEdit->SetVar(COMPONENT_IDS_VAR, ids);
					break;

				case ITEM_UI_ELEMENT:
					for (unsigned int i = 0; i < serializables.Size(); ++i)
						ids.Push(UIUtils::GetUIElementID(dynamic_cast<Urho3D::UIElement*>(serializables[i])));
					attrEdit->SetVar(UI_ELEMENT_IDS_VAR, ids);
					break;

				default:
					break;
				}
			}

			void SetAttributeEditorID(Urho3D::UIElement* attrEdit, Urho3D::Serializable* serializables)
			{
				if (serializables == NULL)
					return;

				// All target serializables must be either nodes, ui-elements, or components
				Urho3D::Vector<Urho3D::Variant> ids;
				switch (UIUtils::GetType(serializables))
				{
				case ITEM_NODE:

					ids.Push(dynamic_cast<Urho3D::Node*>(serializables)->GetID());
					attrEdit->SetVar(NODE_IDS_VAR, ids);
					break;

				case ITEM_COMPONENT:

					ids.Push(dynamic_cast<Urho3D::Component*>(serializables)->GetID());
					attrEdit->SetVar(COMPONENT_IDS_VAR, ids);
					break;

				case ITEM_UI_ELEMENT:

					ids.Push(UIUtils::GetUIElementID(dynamic_cast<Urho3D::UIElement*>(serializables)));
					attrEdit->SetVar(UI_ELEMENT_IDS_VAR, ids);
					break;

				default:
					break;
				}
			}



		}

	}

}