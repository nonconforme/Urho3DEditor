#include "MenuBarUI.h"
#include "UIGlobals.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/Math/Rect.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/UIEvents.h>

namespace Prime
{
	namespace UI
	{
		MenuBarUI* MenuBarUI::Create(UIElement* parent, const Urho3D::String& idname, int width/*=0*/, int height /*= 21*/, Urho3D::XMLFile* defaultstyle /*= NULL*/)
		{
			MenuBarUI* menubar = parent->CreateChild<MenuBarUI>(idname);
			//menubar->SetStyle("Window",styleFile);
			if (defaultstyle)
				menubar->SetDefaultStyle(defaultstyle);
			menubar->SetStyleAuto();
			if (width > 0)
				menubar->SetFixedWidth(width);
			else
				menubar->SetFixedWidth(parent->GetWidth());
			menubar->SetFixedHeight(height);

			return menubar;
		}

		void MenuBarUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<MenuBarUI>();
			COPY_BASE_ATTRIBUTES(BorderImage);
			UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);

		}

		MenuBarUI::~MenuBarUI()
		{

		}

		MenuBarUI::MenuBarUI(Urho3D::Context* context) : BorderImage(context)
		{
			bringToFront_ = true;
			clipChildren_ = true;
			SetEnabled(true);
			SetLayout(Urho3D::LM_HORIZONTAL);
			SetAlignment(Urho3D::HA_LEFT, Urho3D::VA_TOP);
		}

		Urho3D::Menu* MenuBarUI::CreateMenu(const Urho3D::String& title)
		{
			Urho3D::Menu* menu = (Urho3D::Menu*)GetChild(title);
			if (menu)
				return menu;

			menu = new Urho3D::Menu(context_);
			menu->SetName(title);
			menu->SetStyleAuto(GetDefaultStyle());
			menu->SetLayout(Urho3D::LM_HORIZONTAL, 0, Urho3D::IntRect(8, 2, 8, 2));

			// Create Text Label
			Urho3D::Text* menuText = new Urho3D::Text(context_);
			menuText->SetName(title + "_text");
			menu->AddChild(menuText);
			menuText->SetStyle("EditorMenuText");
			menuText->SetText(title);

			// set menubutton size
			menu->SetFixedWidth(menu->GetWidth());

			// create popup
			Urho3D::Window* popup = new  Urho3D::Window(context_);
			popup->SetName(title + "_popup");
			popup->SetLayout(Urho3D::LM_VERTICAL, 1, Urho3D::IntRect(2, 6, 2, 6));
			popup->SetStyleAuto(GetDefaultStyle());
			menu->SetPopup(popup);
			menu->SetPopupOffset(Urho3D::IntVector2(0, menu->GetHeight()));

			AddChild(menu);
			return menu;
		}

		Urho3D::Menu* MenuBarUI::CreateMenuItem(const Urho3D::String& menuTitle, const Urho3D::String& title, const Urho3D::StringHash& action, int accelKey, int accelQual, bool addToQuickMenu, Urho3D::String quickMenuText)
		{
			Urho3D::Menu* menu = (Urho3D::Menu*)GetChild(menuTitle);
			if (!menu)
				return NULL;

			Urho3D::Window*  popup = (Urho3D::Window*)menu->GetPopup();
			if (!popup)
				return NULL;

			// create Menu item
			Urho3D::Menu* menuItem = new Urho3D::Menu(context_);
			menuItem->SetName(title);
			menuItem->SetStyleAuto(GetDefaultStyle());
			menuItem->SetLayout(Urho3D::LM_HORIZONTAL, 0, Urho3D::IntRect(8, 2, 8, 2));
			if (action != Urho3D::StringHash::ZERO)
			{
				menuItem->SetVar(ACTION_VAR, action);
			}
			if (accelKey > 0)
				menuItem->SetAccelerator(accelKey, accelQual);

			// Create Text Label
			Urho3D::Text* menuText = new Urho3D::Text(context_);
			menuText->SetName(title + "_text");
			menuItem->AddChild(menuText);
			menuText->SetStyle("EditorMenuText");
			menuText->SetText(title);

			if (accelKey != 0)
			{
				UIElement* spacer = new UIElement(context_);
				spacer->SetMinWidth(menuText->GetIndentSpacing());
				spacer->SetHeight(menuText->GetHeight());

				menuItem->AddChild(spacer);
				menuItem->AddChild(CreateAccelKeyText(accelKey, accelQual));
			}

			popup->AddChild(menuItem);
			if (action != Urho3D::StringHash::ZERO)
				SubscribeToEvent(menuItem, Urho3D::E_MENUSELECTED, HANDLER(MenuBarUI, HandleMenuSelected));
			/// \todo use dirty masks
			FinalizedPopupMenu(popup);

			return menuItem;
		}

		Urho3D::Window* MenuBarUI::CreatePopupMenu(Urho3D::Menu* menu)
		{
			if (!menu)
			{
				return NULL;
			}
			// create popup
			Urho3D::Window* popup = new  Urho3D::Window(context_);

			popup->SetLayout(Urho3D::LM_VERTICAL, 1, Urho3D::IntRect(2, 6, 2, 6));
			popup->SetDefaultStyle(GetDefaultStyle());
			popup->SetStyleAuto();
			menu->SetPopup(popup);
			menu->SetPopupOffset(Urho3D::IntVector2(0, menu->GetHeight()));
			return popup;
		}

		Urho3D::Menu* MenuBarUI::CreatePopupMenuItem(Urho3D::Window* window, const Urho3D::String& title, const Urho3D::StringHash& action /*= StringHash::ZERO*/, int accelKey /*= 0*/, int accelQual /*= 0*/, bool addToQuickMenu /*= true*/, Urho3D::String quickMenuText /*= ""*/)
		{
			if (!window)
			{
				return NULL;
			}

			// create Menu item
			Urho3D::Menu* menuItem = new Urho3D::Menu(context_);
			menuItem->SetName(title);
			menuItem->SetStyleAuto(GetDefaultStyle());
			menuItem->SetLayout(Urho3D::LM_HORIZONTAL, 0, Urho3D::IntRect(8, 2, 8, 2));
			if (action != Urho3D::StringHash::ZERO)
			{
				menuItem->SetVar(ACTION_VAR, action);
			}
			if (accelKey > 0)
				menuItem->SetAccelerator(accelKey, accelQual);

			// Create Text Label
			Urho3D::Text* menuText = new Urho3D::Text(context_);
			menuText->SetName(title + "_text");
			menuItem->AddChild(menuText);
			menuText->SetStyle("EditorMenuText");
			menuText->SetText(title);

			if (accelKey != 0)
			{
				UIElement* spacer = new UIElement(context_);
				spacer->SetMinWidth(menuText->GetIndentSpacing());
				spacer->SetHeight(menuText->GetHeight());

				menuItem->AddChild(spacer);
				menuItem->AddChild(CreateAccelKeyText(accelKey, accelQual));
			}

			window->AddChild(menuItem);
			if (action != Urho3D::StringHash::ZERO)
				SubscribeToEvent(menuItem, Urho3D::E_MENUSELECTED, HANDLER(MenuBarUI, HandleMenuSelected));
			/// \todo use dirty masks
			FinalizedPopupMenu(window);
			return menuItem;
		}

		void MenuBarUI::FinalizedPopupMenu(Urho3D::Window* popup)
		{
			// Find the maximum menu text width
			Urho3D::Vector<Urho3D::SharedPtr<Urho3D::UIElement> > children = popup->GetChildren();

			int maxWidth = 0;

			for (unsigned int i = 0; i < children.Size(); ++i)
			{
				UIElement* element = children[i];
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
				UIElement* element = children[i];
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

		Urho3D::Text* MenuBarUI::CreateAccelKeyText(int accelKey, int accelQual)
		{
			Urho3D::Text* accelKeyText = new Urho3D::Text(context_);
			accelKeyText->SetDefaultStyle(GetDefaultStyle());
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
			if ((accelQual & Urho3D::QUAL_ALT) > 0)
				text = "Alt+" + text;
			if ((accelQual & Urho3D::QUAL_SHIFT) > 0)
				text = "Shift+" + text;
			if ((accelQual & Urho3D::QUAL_CTRL) > 0)
				text = "Ctrl+" + text;
			accelKeyText->SetText(text);

			return accelKeyText;
		}

		void MenuBarUI::HandleMenuSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MenuSelected;

			UIElement* element = static_cast<UIElement*>(eventData[P_ELEMENT].GetPtr());
			if (element && element->GetType() == MENU_TYPE)
			{
				const Urho3D::Variant& action = element->GetVar(ACTION_VAR);
				if (action != Urho3D::Variant::EMPTY)
				{
					using namespace MenuBarAction;

					Urho3D::VariantMap& newEventData = GetEventDataMap();
					newEventData[P_ACTION] = action;
					newEventData[P_UINAME] = element->GetName();
					SendEvent(E_MENUBAR_ACTION, newEventData);
				}

			}

		}
	}
}