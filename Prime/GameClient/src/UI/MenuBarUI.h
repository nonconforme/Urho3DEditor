/*!
 * \file MenuBarUI.h
 *
 * \author vitali
 * \date Februar 2015
 *
 *
 */

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/BorderImage.h>

namespace Urho3D
{
	class BorderImage;
	class XMLFile;
	class Menu;
	class Window;
	class Text;
}

namespace Prime
{
	namespace UI
	{
		/// Menu selected.
		EVENT(E_MENUBAR_ACTION, MenuBarAction)
		{
			PARAM(P_ACTION, Action);              // stringhash 
			PARAM(P_UINAME, UIName);              // string
		}

		/// \todo use dirty masks
		class MenuBarUI : public Urho3D::BorderImage
		{
			OBJECT(MenuBarUI);
		public:
			MenuBarUI(Urho3D::Context* context);
			virtual ~MenuBarUI();
			static void RegisterObject(Urho3D::Context* context);
			static MenuBarUI* Create(UIElement* context, const Urho3D::String& idname, int width = 0, int height = 21, Urho3D::XMLFile* defaultstyle = NULL);

			Urho3D::Menu* CreateMenu(const Urho3D::String& title);
			Urho3D::Menu* CreateMenuItem(const Urho3D::String& menuTitle, const Urho3D::String& title, const Urho3D::StringHash& action = Urho3D::StringHash::ZERO, int accelKey = 0, int accelQual = 0, bool addToQuickMenu = true, Urho3D::String quickMenuText = "");
			Urho3D::Window* CreatePopupMenu(Urho3D::Menu* menu);
			Urho3D::Menu* CreatePopupMenuItem(Urho3D::Window* window, const Urho3D::String& title, const Urho3D::StringHash& action = Urho3D::StringHash::ZERO, int accelKey = 0, int accelQual = 0, bool addToQuickMenu = true, Urho3D::String quickMenuText = "");
		protected:
			void FinalizedPopupMenu(Urho3D::Window* popup);
			Urho3D::Text* CreateAccelKeyText(int accelKey, int accelQual);

			void HandleMenuSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		private:
		};
	}
}