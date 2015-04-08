/*!
 * \file MiniToolBarUI.h
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
		/// \todo use dirty masks
		class MiniToolBarUI : public Urho3D::BorderImage
		{
			OBJECT(MiniToolBarUI);
		public:
			MiniToolBarUI(Urho3D::Context* context);
			virtual ~MiniToolBarUI();
			static void RegisterObject(Urho3D::Context* context);
			static MiniToolBarUI* Create(Urho3D::UIElement* context, const Urho3D::String& idname, Urho3D::XMLFile* iconStyle, int width = 28, int height = 0, Urho3D::XMLFile* defaultstyle = NULL);


			void SetIconStyle(Urho3D::XMLFile* iconStyle) { iconStyle_ = iconStyle; }
			Urho3D::UIElement* CreateSmallToolBarButton(const Urho3D::String& title, const Urho3D::String& toolTipTitle = Urho3D::String::EMPTY);
			Urho3D::UIElement* CreateSmallToolBarSpacer(unsigned int width);
		protected:
			void CreateSmallToolBarIcon(UIElement* element);
			UIElement* CreateToolTip(UIElement* parent, const Urho3D::String& title, const Urho3D::IntVector2& offset);
			Urho3D::SharedPtr< Urho3D::XMLFile> iconStyle_;

		private:
		};
	}
}
