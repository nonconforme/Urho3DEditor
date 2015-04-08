/*!
 * \file ToolBarUI.h
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
	class ScrollBar;
	class CheckBox;
}

namespace Prime
{
	namespace UI
	{
		/// \todo use dirty masks
		class ToolBarUI : public Urho3D::BorderImage
		{
			OBJECT(ToolBarUI);
		public:
			ToolBarUI(Urho3D::Context* context);
			virtual ~ToolBarUI();
			static void RegisterObject(Urho3D::Context* context);
			static ToolBarUI* Create(Urho3D::UIElement* context, const Urho3D::String& idname, Urho3D::XMLFile* iconStyle, const Urho3D::String& baseStyle = "ToolBarToggle", int width = 0, int height = 41, Urho3D::XMLFile* defaultstyle = NULL);

			UIElement*	CreateGroup(const Urho3D::String& name, Urho3D::LayoutMode layoutmode);
			Urho3D::CheckBox*	CreateToolBarToggle(const Urho3D::String& groupname, const Urho3D::String& title);
			Urho3D::CheckBox*	CreateToolBarToggle(const Urho3D::String& title);
			UIElement*	CreateToolBarIcon(Urho3D::UIElement* element);
			UIElement*	CreateToolTip(Urho3D::UIElement* parent, const Urho3D::String& title, const Urho3D::IntVector2& offset);
			UIElement*  CreateToolBarSpacer(int width);
			void SetIconStyle(Urho3D::XMLFile* iconStyle) { iconStyle_ = iconStyle; }
			void SetBaseStyle(const Urho3D::String& baseStyle) { baseStyle_ = baseStyle; }
		protected:
			void FinalizeGroupHorizontal(UIElement* group, const Urho3D::String& baseStyle);

			Urho3D::SharedPtr< Urho3D::XMLFile> iconStyle_;
			/// Horizontal scroll bar.
			Urho3D::SharedPtr<Urho3D::ScrollBar> horizontalScrollBar_;
			Urho3D::String baseStyle_;
		private:
		};
	}

}