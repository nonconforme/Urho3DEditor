#include "MiniToolBarUI.h"

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

namespace Prime
{
	namespace UI
	{
		MiniToolBarUI* MiniToolBarUI::Create(Urho3D::UIElement* parent, const Urho3D::String& idname, Urho3D::XMLFile* iconStyle, int width, int height, Urho3D::XMLFile* defaultstyle)
		{
			MiniToolBarUI* menubar = parent->CreateChild<MiniToolBarUI>(idname);
			if (defaultstyle)
				menubar->SetDefaultStyle(defaultstyle);
			menubar->SetStyleAuto();
			if (height > 0)
				menubar->SetFixedHeight(height);
			else
				menubar->SetFixedHeight(parent->GetRoot()->GetHeight() - parent->GetMinHeight());
			menubar->SetFixedWidth(width);
			menubar->iconStyle_ = iconStyle;

			return menubar;
		}

		void MiniToolBarUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<MiniToolBarUI>();
			COPY_BASE_ATTRIBUTES(BorderImage);
			UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);

		}

		MiniToolBarUI::~MiniToolBarUI()
		{

		}

		MiniToolBarUI::MiniToolBarUI(Urho3D::Context* context) : BorderImage(context)
		{
			bringToFront_ = true;
			clipChildren_ = true;
			SetEnabled(true);
			SetLayout(Urho3D::LM_VERTICAL, 4, Urho3D::IntRect(4, 4, 4, 4));
			SetAlignment(Urho3D::HA_LEFT, Urho3D::VA_TOP);

		}

		Urho3D::UIElement* MiniToolBarUI::CreateSmallToolBarButton(const Urho3D::String& title, const Urho3D::String& toolTipTitle /*= String::EMPTY*/)
		{
			Urho3D::Button* button = new Urho3D::Button(context_);
			button->SetName(title);
			button->SetDefaultStyle(GetDefaultStyle());
			button->SetStyle("ToolBarButton");
			button->SetFixedSize(20, 20);
			CreateSmallToolBarIcon(button);

			if (toolTipTitle.Empty())
				CreateToolTip(button, title, Urho3D::IntVector2(button->GetWidth() + 10, button->GetHeight() - 10));
			else
				CreateToolTip(button, toolTipTitle, Urho3D::IntVector2(button->GetWidth() + 10, button->GetHeight() - 10));
			AddChild(button);
			return button;
		}

		Urho3D::UIElement* MiniToolBarUI::CreateSmallToolBarSpacer(unsigned int width)
		{
			UIElement* spacer = CreateChild<UIElement>("Spacer");
			spacer->SetFixedHeight(width);

			return spacer;
		}

		void MiniToolBarUI::CreateSmallToolBarIcon(UIElement* element)
		{
			BorderImage* icon = new BorderImage(context_);
			icon->SetName("Icon");
			icon->SetDefaultStyle(iconStyle_);
			icon->SetStyle(element->GetName());
			icon->SetFixedSize(14, 14);
			element->AddChild(icon);
		}

		Urho3D::UIElement* MiniToolBarUI::CreateToolTip(UIElement* parent, const Urho3D::String& title, const Urho3D::IntVector2& offset)
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
	}
}