#include "EditorView.h"
#include "EditorData.h"

#include "../UI/TabWindow.h"
#include "../UI/HierarchyWindow.h"
#include "../UI/AttributeInspector.h"
#include "../UI/MenuBarUI.h"
#include "../UI/ToolBarUI.h"
#include "../UI/MiniToolBarUI.h"
#include "../UI/UIUtils.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/View3D.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Math/BoundingBox.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Core/CoreEvents.h>


namespace Prime
{
	namespace Editor
	{
		EditorView::EditorView(Urho3D::Context* context, EditorData* data) : Object(context)
		{
			if (!data)
				return;

			cache_ = GetSubsystem<Urho3D::ResourceCache>();
			graphics_ = GetSubsystem<Urho3D::Graphics>();
			ui_ = GetSubsystem<Urho3D::UI>();

			Urho3D::XMLFile* styleFile = NULL;
			Urho3D::XMLFile* iconstyle = NULL;
			if (data)
			{
				styleFile = data->GetDefaultStyle();
				iconstyle = data->GetIconStyle();
			}
			else
			{
				styleFile = cache_->GetResource<Urho3D::XMLFile>("UI/IDEStyle.xml");
				iconstyle = cache_->GetResource<Urho3D::XMLFile>("UI/IDEIcons.xml");
			}

			uiRoot_ = data->GetEdiorRootUI();

			menubar_ = UI::MenuBarUI::Create(uiRoot_, "EditorMenuBar");

			toolbar_ = UI::ToolBarUI::Create(uiRoot_, "EditorToolBar", iconstyle);

			int minitoolheight = ui_->GetRoot()->GetHeight() - toolbar_->GetMinHeight() - menubar_->GetMinHeight();

			Urho3D::UIElement* mainFrameUI_ = uiRoot_->CreateChild<Urho3D::UIElement>("MainFrame");
			mainFrameUI_->SetDefaultStyle(styleFile);
			mainFrameUI_->SetFixedSize(ui_->GetRoot()->GetWidth(), minitoolheight);

			miniToolbar_ = UI::MiniToolBarUI::Create(mainFrameUI_, "EditorMiniToolBar", iconstyle, 28, minitoolheight, styleFile);


			leftFrame_ = mainFrameUI_->CreateChild<UI::TabWindow>("LeftFrame");

			leftFrame_->SetStyle("TabWindow");
			leftFrame_->SetWidth(200);
			leftFrame_->SetFixedHeight(minitoolheight);
			leftFrame_->SetResizable(true);
			leftFrame_->SetResizeBorder(Urho3D::IntRect(6, 6, 6, 6));
			leftFrame_->SetLayoutBorder(Urho3D::IntRect(4, 4, 4, 4));
			leftFrame_->SetPosition(miniToolbar_->GetPosition() + Urho3D::IntVector2(miniToolbar_->GetWidth(), 0));

			middleFrame_ = mainFrameUI_->CreateChild<UI::TabWindow>("MiddleFrame");
			middleFrame_->SetPosition(leftFrame_->GetPosition() + Urho3D::IntVector2(leftFrame_->GetWidth(), 0));
			middleFrame_->SetStyle("TabWindow");
			middleFrame_->SetHeight(minitoolheight);
			middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - 400);
			middleFrame_->SetLayoutBorder(Urho3D::IntRect(4, 4, 4, 4));

			rightFrame_ = mainFrameUI_->CreateChild<UI::TabWindow>("LeftFrame");
			rightFrame_->SetPosition(middleFrame_->GetPosition() + Urho3D::IntVector2(middleFrame_->GetWidth(), 0));
			rightFrame_->SetStyle("TabWindow");
			rightFrame_->SetLayout(Urho3D::LM_VERTICAL);
			rightFrame_->SetWidth(200);
			rightFrame_->SetResizable(true);
			rightFrame_->SetFixedHeight(minitoolheight);
			rightFrame_->SetResizeBorder(Urho3D::IntRect(6, 6, 6, 6));
			rightFrame_->SetLayoutBorder(Urho3D::IntRect(4, 4, 4, 4));
			SubscribeToEvent(rightFrame_, Urho3D::E_RESIZED, HANDLER(EditorView, HandlePost));
			SubscribeToEvent(leftFrame_, Urho3D::E_RESIZED, HANDLER(EditorView, HandlePost));

			minLeftFrameWidth_ = 100;
			minMiddleFrameWidth_ = 200; /// \todo use minMiddleFrameWidth_
			minRightFrameWidth_ = 100;
		}

		EditorView::~EditorView()
		{
		}

		void EditorView::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<EditorView>();
		}

		void EditorView::HandlePost(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Resized;

			Urho3D::UIElement* element = (Urho3D::UIElement*)eventData[P_ELEMENT].GetPtr();
			int width = eventData[P_WIDTH].GetInt();
			int height = eventData[P_HEIGHT].GetInt();

			if (element == leftFrame_)
			{
				if (width < minLeftFrameWidth_)
				{
					leftFrame_->SetWidth(minLeftFrameWidth_);
				}
				middleFrame_->SetPosition(leftFrame_->GetPosition() + Urho3D::IntVector2(leftFrame_->GetWidth(), 0));
				if (rightFrame_->IsVisible())
				{
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
				}
				else
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - miniToolbar_->GetWidth());
			}
			else if (element == rightFrame_)
			{
				if (width < minRightFrameWidth_)
				{
					rightFrame_->SetWidth(minRightFrameWidth_);
				}
				if (leftFrame_->IsVisible())
				{
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
				}
				else
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());

				rightFrame_->SetPosition(middleFrame_->GetPosition() + Urho3D::IntVector2(middleFrame_->GetWidth(), 0));
			}
		}

		void EditorView::UpdateLayout()
		{
			int minitoolheight = ui_->GetRoot()->GetHeight() - menubar_->GetMinHeight();
			if (toolbar_->IsVisible())
				minitoolheight -= toolbar_->GetMinHeight();
			miniToolbar_->SetFixedHeight(minitoolheight);
			middleFrame_->SetFixedHeight(minitoolheight);
			if (leftFrame_->IsVisible())
			{
				if (miniToolbar_->IsVisible())
					leftFrame_->SetPosition(miniToolbar_->GetPosition() + Urho3D::IntVector2(miniToolbar_->GetWidth(), 0));
				else
					leftFrame_->SetPosition(Urho3D::IntVector2::ZERO);

				middleFrame_->SetPosition(leftFrame_->GetPosition() + Urho3D::IntVector2(leftFrame_->GetWidth(), 0));
				leftFrame_->SetFixedHeight(minitoolheight);
				if (rightFrame_->IsVisible())
				{
					rightFrame_->SetFixedHeight(minitoolheight);
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
					rightFrame_->SetPosition(middleFrame_->GetPosition() + Urho3D::IntVector2(middleFrame_->GetWidth(), 0));
				}
				else
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - miniToolbar_->GetWidth());
			}
			else
			{
				if (miniToolbar_->IsVisible())
					middleFrame_->SetPosition(miniToolbar_->GetPosition() + Urho3D::IntVector2(miniToolbar_->GetWidth(), 0));
				else
					middleFrame_->SetPosition(Urho3D::IntVector2::ZERO);

				if (rightFrame_->IsVisible())
				{
					rightFrame_->SetFixedHeight(minitoolheight);
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
					rightFrame_->SetPosition(middleFrame_->GetPosition() + Urho3D::IntVector2(middleFrame_->GetWidth(), 0));
				}
				else
					middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - miniToolbar_->GetWidth());
			}
		}

		void EditorView::SetToolBarVisible(bool enable)
		{
			toolbar_->SetVisible(enable);
			UpdateLayout();
		}

		bool EditorView::IsToolBarVisible() const
		{
			return toolbar_->IsVisible();
		}

		void EditorView::SetLeftFrameVisible(bool enable)
		{
			leftFrame_->SetVisible(enable);
			UpdateLayout();
		}

		bool EditorView::IsLeftFrameVisible() const
		{
			return leftFrame_->IsVisible();
		}

		void EditorView::SetRightFrameVisible(bool enable)
		{
			rightFrame_->SetVisible(enable);
			UpdateLayout();
		}

		bool EditorView::IsRightFrameVisible() const
		{
			return rightFrame_->IsVisible();
		}

		// 	void EditorView::SetMiniToolBarVisible(bool enable)
		// 	{
		// 		miniToolbar_->SetVisible(enable);
		// 		UpdateLayout();
		// 	}
		// 
		// 	bool EditorView::IsMiniToolBarVisible() const
		// 	{
		// 		return miniToolbar_->IsVisible();
		// 	}

		UI::TabWindow* EditorView::GetLeftFrame()
		{
			return leftFrame_;
		}

		UI::TabWindow* EditorView::GetRightFrame()
		{
			return rightFrame_;
		}

		UI::TabWindow* EditorView::GetMiddleFrame()
		{
			return middleFrame_;
		}

		UI::ToolBarUI* EditorView::GetToolBar()
		{
			return toolbar_;
		}

		UI::MiniToolBarUI* EditorView::GetMiniToolBar()
		{
			return miniToolbar_;
		}

		UI::MenuBarUI* EditorView::GetGetMenuBar()
		{
			return menubar_;
		}
	}
}