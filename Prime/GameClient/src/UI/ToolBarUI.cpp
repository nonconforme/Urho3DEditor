#include "ToolBarUI.h"

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

#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/ScrollBar.h>

namespace Prime
{
	namespace UI
	{
		ToolBarUI* ToolBarUI::Create(UIElement* parent, const Urho3D::String& idname, Urho3D::XMLFile* iconStyle, const Urho3D::String& baseStyle, int width/*=0*/, int height /*= 21*/, Urho3D::XMLFile* defaultstyle /*= NULL*/)
		{
			ToolBarUI* menubar = parent->CreateChild<ToolBarUI>(idname);
			//menubar->SetStyle("Window",styleFile);
			if (defaultstyle)
				menubar->SetDefaultStyle(defaultstyle);
			menubar->SetStyleAuto();
			if (width > 0)
				menubar->SetFixedWidth(width);
			else
				menubar->SetFixedWidth(parent->GetMinWidth());
			menubar->SetFixedHeight(height);
			menubar->iconStyle_ = iconStyle;
			menubar->baseStyle_ = baseStyle;
			return menubar;
		}

		void ToolBarUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<ToolBarUI>();
			COPY_BASE_ATTRIBUTES(BorderImage);
			UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);

		}

		ToolBarUI::~ToolBarUI()
		{

		}

		ToolBarUI::ToolBarUI(Urho3D::Context* context) : BorderImage(context)
		{
			bringToFront_ = true;
			clipChildren_ = true;
			SetEnabled(true);
			SetLayout(Urho3D::LM_HORIZONTAL, 4, Urho3D::IntRect(8, 4, 4, 8));
			SetAlignment(Urho3D::HA_LEFT, Urho3D::VA_TOP);
			// 		horizontalScrollBar_ = CreateChild<ScrollBar>("TB_HorizontalScrollBar");
			// 		horizontalScrollBar_->SetInternal(true);
			// 		horizontalScrollBar_->SetAlignment(HA_LEFT, VA_BOTTOM);
			// 		horizontalScrollBar_->SetOrientation(O_HORIZONTAL);

		}

		Urho3D::UIElement* ToolBarUI::CreateGroup(const Urho3D::String& name, Urho3D::LayoutMode layoutmode)
		{
			UIElement* group = GetChild(name);
			if (group)
				return group;

			group = new UIElement(context_);
			group->SetName(name);
			group->SetDefaultStyle(GetDefaultStyle());
			group->SetLayoutMode(layoutmode);
			group->SetAlignment(Urho3D::HA_LEFT, Urho3D::VA_CENTER);
			AddChild(group);
			return group;
		}

		Urho3D::CheckBox* ToolBarUI::CreateToolBarToggle(const Urho3D::String& groupname, const Urho3D::String& title)
		{
			UIElement* group = GetChild(groupname);
			if (group)
			{
				Urho3D::CheckBox* toggle = new Urho3D::CheckBox(context_);
				toggle->SetName(title);
				toggle->SetDefaultStyle(GetDefaultStyle());
				toggle->SetStyle(baseStyle_);
				toggle->SetOpacity(0.7f);

				CreateToolBarIcon(toggle);
				CreateToolTip(toggle, title, Urho3D::IntVector2(toggle->GetWidth() + 10, toggle->GetHeight() - 10));

				group->AddChild(toggle);
				FinalizeGroupHorizontal(group, baseStyle_);

				return toggle;
			}
			return NULL;
		}

		Urho3D::CheckBox* ToolBarUI::CreateToolBarToggle(const Urho3D::String& title)
		{
			Urho3D::CheckBox* toggle = new Urho3D::CheckBox(context_);
			toggle->SetName(title);
			toggle->SetDefaultStyle(GetDefaultStyle());
			toggle->SetStyle(baseStyle_);
			toggle->SetOpacity(0.7f);

			CreateToolBarIcon(toggle);
			CreateToolTip(toggle, title, Urho3D::IntVector2(toggle->GetWidth() + 10, toggle->GetHeight() - 10));
			AddChild(toggle);

			return toggle;
		}

		Urho3D::UIElement* ToolBarUI::CreateToolBarIcon(Urho3D::UIElement* element)
		{
			Urho3D::BorderImage* icon = new Urho3D::BorderImage(context_);
			icon->SetName("Icon");
			icon->SetDefaultStyle(iconStyle_);
			icon->SetStyle(element->GetName());
			icon->SetFixedSize(GetHeight() - 11, GetHeight() - 11);
			icon->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
			element->AddChild(icon);
			return icon;
		}

		Urho3D::UIElement* ToolBarUI::CreateToolTip(Urho3D::UIElement* parent, const Urho3D::String& title, const Urho3D::IntVector2& offset)
		{
			Urho3D::ToolTip* toolTip = parent->CreateChild<Urho3D::ToolTip>("ToolTip");
			toolTip->SetPosition(offset);

			Urho3D::BorderImage* textHolder = toolTip->CreateChild<BorderImage>("BorderImage");
			textHolder->SetStyle("ToolTipBorderImage");

			Urho3D::Text* toolTipText = textHolder->CreateChild<Urho3D::Text>("Text");
			toolTipText->SetStyle("ToolTipText");
			toolTipText->SetText(title);

			return toolTip;
		}

		void ToolBarUI::FinalizeGroupHorizontal(Urho3D::UIElement* group, const Urho3D::String& baseStyle)
		{
			int width = 0;
			for (unsigned int i = 0; i < group->GetNumChildren(); ++i)
			{
				UIElement* child = group->GetChild(i);
				width += child->GetMinWidth();
				if (i == 0 && i < group->GetNumChildren() - 1)
					child->SetStyle(baseStyle + "GroupLeft");
				else if (i < group->GetNumChildren() - 1)
					child->SetStyle(baseStyle + "GroupMiddle");
				else
					child->SetStyle(baseStyle + "GroupRight");
				child->SetFixedSize(GetHeight() - 6, GetHeight() - 6);
			}
			/// \todo SetMaxSize(group->GetSize()) does not work !? 
			//group->SetMaxSize(group->GetSize());
			group->SetFixedWidth(width);
		}

		Urho3D::UIElement* ToolBarUI::CreateToolBarSpacer(int width)
		{
			UIElement* spacer = new UIElement(context_);
			spacer->SetFixedWidth(width);
			AddChild(spacer);
			return spacer;
		}
	}
}