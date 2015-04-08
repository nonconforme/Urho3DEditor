#include "EPGame.h"
#include "EditorData.h"
#include "../Game/Events.h"

#include "EditorSelection.h"
#include "../UI/UIUtils.h"
#include "../UI/MenuBarUI.h"
#include "EditorState.h"
#include "../UI/HierarchyWindow.h"
#include "GizmoScene3D.h"
#include "../UI/ToolBarUI.h"
#include "../UI/AttributeInspector.h"
#include "../UI/MiniToolBarUI.h"
#include "EditorView.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/View3D.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/RenderSurface.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Graphics/Drawable.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/UI/MessageBox.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/UI/ListView.h>


namespace Prime
{
	namespace Editor
	{
		EPGame::EPGame(Urho3D::Context* context) : EditorPlugin(context)
		{
			ui_ = GetSubsystem<Urho3D::UI>();
			input_ = GetSubsystem<Urho3D::Input>();
			cache_ = GetSubsystem<Urho3D::ResourceCache>();
			renderer = GetSubsystem<Urho3D::Renderer>();
			fileSystem_ = GetSubsystem<Urho3D::FileSystem>();

			editorView_ = GetSubsystem<EditorView>();
			editorData_ = GetSubsystem<EditorData>();
			editorSelection_ = GetSubsystem<EditorSelection>();
			if (editorData_)
				editor_ = editorData_->GetEditor();

			toggledMouseLock_ = false;
		}

		EPGame::~EPGame()
		{
		}

		bool EPGame::HasMainScreen()
		{
			return true;
		}

		Urho3D::String EPGame::GetName() const
		{
			return Urho3D::String("Play");
		}

		void EPGame::Edit(Object *object)
		{
			if (Urho3D::Component::GetTypeStatic() == object->GetBaseType())
			{
			}

			if (Urho3D::Node::GetTypeStatic() == object->GetBaseType())
			{
			}
		}

		bool EPGame::Handles(Object *object) const
		{
			if (Urho3D::Component::GetTypeStatic() == object->GetBaseType())
				return true;
			if (Urho3D::Node::GetTypeStatic() == object->GetBaseType())
				return true;
			// 		if (Scene::GetTypeStatic() == object->GetType())
			// 			return true;
			return false;
		}

		Urho3D::UIElement* EPGame::GetMainScreen()
		{
			if (!window_)
				Start();

			return window_;
		}

		void EPGame::SetVisible(bool visible)
		{
			if (visible_ == visible)
				return;

			visible_ = visible;

			if (window_)
			{
				window_->SetVisible(visible);
				_gameMenu->SetVisible(visible);

				for (unsigned i = 0; i < miniToolBarButtons_.Size(); i++)
					miniToolBarButtons_[i]->SetVisible(visible);

				for (unsigned i = 0; i < toolBarToggles.Size(); i++)
					toolBarToggles[i]->SetVisible(visible);

				if (visible)
				{
					activeView->SetView(editorData_->GetEditorScene());

					SubscribeToEvent(Urho3D::E_POSTRENDERUPDATE, HANDLER(EPGame, HandlePostRenderUpdate));
					SubscribeToEvent(Urho3D::E_UIMOUSECLICK, HANDLER(EPGame, ViewMouseClick));
					SubscribeToEvent(Urho3D::E_MOUSEMOVE, HANDLER(EPGame, ViewMouseMove));
					SubscribeToEvent(Urho3D::E_UIMOUSECLICKEND, HANDLER(EPGame, ViewMouseClickEnd));
					SubscribeToEvent(Urho3D::E_BEGINVIEWUPDATE, HANDLER(EPGame, HandleBeginViewUpdate));
					SubscribeToEvent(Urho3D::E_ENDVIEWUPDATE, HANDLER(EPGame, HandleEndViewUpdate));
					SubscribeToEvent(Urho3D::E_BEGINVIEWRENDER, HANDLER(EPGame, HandleBeginViewRender));
					SubscribeToEvent(Urho3D::E_ENDVIEWRENDER, HANDLER(EPGame, HandleEndViewRender));
					activeView->SetAutoUpdate(true);
				}
				else
				{
					UnsubscribeFromEvent(Urho3D::E_POSTRENDERUPDATE);
					UnsubscribeFromEvent(Urho3D::E_UIMOUSECLICK);
					UnsubscribeFromEvent(Urho3D::E_MOUSEMOVE);
					UnsubscribeFromEvent(Urho3D::E_UIMOUSECLICKEND);
					UnsubscribeFromEvent(Urho3D::E_BEGINVIEWUPDATE);
					UnsubscribeFromEvent(Urho3D::E_ENDVIEWUPDATE);
					UnsubscribeFromEvent(Urho3D::E_BEGINVIEWRENDER);
					UnsubscribeFromEvent(Urho3D::E_ENDVIEWRENDER);
					activeView->SetAutoUpdate(false);
				}
			}
		}

		void EPGame::Update(float timeStep)
		{
			if (runUpdate)
				activeView->Update(timeStep);

			if (toolBarDirty && editorView_->IsToolBarVisible())
				UpdateToolBar();

			if (ui_->HasModalElement() || ui_->GetFocusElement() != NULL)
			{
				ReleaseMouseLock();
				return;
			}
		}

		void EPGame::Start()
		{
			EditorData* editorData_ = GetSubsystem<EditorData>();
			window_ = new Urho3D::UIElement(context_);
			window_->SetDefaultStyle(editorData_->GetDefaultStyle());

			activeView = window_->CreateChild<EPGameView>("SceneGame");
			activeView->SetDefaultStyle(editorData_->GetDefaultStyle());
			activeView->SetView(editorData_->GetEditorScene());
			activeView->CreateViewportContextUI(editorData_->GetDefaultStyle(), editorData_->GetIconStyle());
			cameraNode_ = activeView->GetCameraNode();
			camera_ = activeView->GetCamera();
			activeView->SetAutoUpdate(true);

			SubscribeToEvent(window_, Urho3D::E_RESIZED, HANDLER(EPGame, HandleResizeView));

			//////////////////////////////////////////////////////////////////////////
			/// Menu Bar entries

			_gameMenu = editorView_->GetGetMenuBar()->CreateMenu("Game");
			//editorView_->GetGetMenuBar()->CreateMenuItem("Game", "New scene", UI::A_NEWSCENE_VAR, 'N', Urho3D::QUAL_SHIFT | Urho3D::QUAL_CTRL);

			_gameMenu->SetVisible(false);

			/// Mini Tool Bar entries
			CreateMiniToolBarUI();

			/// Tool Bar entries
			CreateToolBarUI();
		}


		void EPGame::CreateMiniToolBarUI()
		{
			UI::MiniToolBarUI* minitool = editorView_->GetMiniToolBar();

		}

		void EPGame::CreateToolBarUI()
		{
			UI::ToolBarUI* minitool = editorView_->GetToolBar();


			Urho3D::UIElement* e = minitool->CreateGroup("RunUpdateGroup", Urho3D::LM_HORIZONTAL);
			toolBarToggles.Push(e);
			Urho3D::CheckBox* checkbox = minitool->CreateToolBarToggle("RunUpdateGroup", "RunUpdatePlay");
			if (checkbox->IsChecked() != runUpdate)
				checkbox->SetChecked(runUpdate);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPGame, ToolBarRunUpdatePlay));
			checkbox = minitool->CreateToolBarToggle("RunUpdateGroup", "RunUpdatePause");
			if (checkbox->IsChecked() != (runUpdate == false))
				checkbox->SetChecked(runUpdate == false);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPGame, ToolBarRunUpdatePause));
			checkbox = minitool->CreateToolBarToggle("RunUpdateGroup", "RevertOnPause");
			checkbox->SetChecked(false);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPGame, ToolBarRevertOnPause));
			minitool->CreateToolBarSpacer(4);
		}

		void EPGame::SetMouseLock()
		{
			toggledMouseLock_ = true;
		}

		void EPGame::ReleaseMouseLock()
		{
			if (toggledMouseLock_)
			{
				toggledMouseLock_ = false;
			}
		}

		void EPGame::HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::PostRenderUpdate;

		}

		void EPGame::ViewMouseClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::UIMouseClick;

		}

		void EPGame::ViewMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseMove;
		}

		void EPGame::ViewMouseClickEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::UIMouseClickEnd;
		}

		void EPGame::HandleBeginViewUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::BeginViewUpdate;
		}

		void EPGame::HandleEndViewUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::EndViewUpdate;
		}

		void EPGame::HandleBeginViewRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::BeginViewRender;
		}

		void EPGame::HandleEndViewRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::EndViewUpdate;
		}

		void EPGame::HandleResizeView(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (activeView)
				activeView->SetSize(window_->GetSize());
		}

		void EPGame::UpdateToolBar()
		{
			/// \todo 

			UI::ToolBarUI* toolBar = editorView_->GetToolBar();

			Urho3D::CheckBox* checkbox = (Urho3D::CheckBox*)toolBar->GetChild("RunUpdatePlay", true);
			if (checkbox->IsChecked() != runUpdate)
				checkbox->SetChecked(runUpdate);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("RunUpdatePause", true);
			if (checkbox->IsChecked() != (runUpdate == false))
				checkbox->SetChecked(runUpdate == false);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("RevertOnPause", true);
			checkbox->SetChecked(false);

			toolBarDirty = false;
		}

		void EPGame::ToolBarRunUpdatePlay(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;

			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				StartSceneUpdate();
			toolBarDirty = true;
		}

		void EPGame::ToolBarRunUpdatePause(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				StopSceneUpdate();
			toolBarDirty = true;
		}

		void EPGame::ToolBarRevertOnPause(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			activeView->SetView(editorData_->GetEditorScene());
			toolBarDirty = true;
		}

		void EPGame::StartSceneUpdate()
		{
			runUpdate = true;
			// Run audio playback only when scene is updating, so that audio components' time-dependent attributes stay constant when
			// paused (similar to physics)
			//audio.Play();
			toolBarDirty = true;

			activeView->StartSceneUpdate();
		}

		void EPGame::StopSceneUpdate()
		{
			runUpdate = false;
			//audio.Stop();
			toolBarDirty = true;

			activeView->StopSceneUpdate();
		}

		EPGameView::EPGameView(Urho3D::Context* context) : BorderImage(context),
			rttFormat_(Urho3D::Graphics::GetRGBFormat()),
			autoUpdate_(true)
		{
			SetEnabled(true);
			bringToFront_ = true;

			renderTexture_ = new Urho3D::Texture2D(context_);
			depthTexture_ = new Urho3D::Texture2D(context_);
			viewport_ = new Urho3D::Viewport(context_);

			cameraNode_ = new Urho3D::Node(context_);
			camera_ = cameraNode_->CreateComponent<Urho3D::Camera>();
			//		camera.fillMode = fillMode;
			soundListener_ = cameraNode_->CreateComponent<Urho3D::SoundListener>();
			camera_->SetViewMask(0x7FFFFFFF); // It's easier to only have 1 gizmo active this viewport is shared with the gizmo
		}

		EPGameView::~EPGameView()
		{
			ResetScene();
		}

		void EPGameView::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<EPGameView>();
		}

		void EPGameView::OnResize()
		{
			int width = GetWidth();
			int height = GetHeight();

			if (width > 0 && height > 0)
			{
				renderTexture_->SetSize(width, height, rttFormat_, Urho3D::TEXTURE_RENDERTARGET);
				depthTexture_->SetSize(width, height, Urho3D::Graphics::GetDepthStencilFormat(), Urho3D::TEXTURE_DEPTHSTENCIL);
				Urho3D::RenderSurface* surface = renderTexture_->GetRenderSurface();
				surface->SetViewport(0, viewport_);
				surface->SetUpdateMode(autoUpdate_ ? Urho3D::SURFACE_UPDATEALWAYS : Urho3D::SURFACE_MANUALUPDATE);
				surface->SetLinkedDepthStencil(depthTexture_->GetRenderSurface());

				SetTexture(renderTexture_);
				SetImageRect(Urho3D::IntRect(0, 0, width, height));

				if (!autoUpdate_)
					surface->QueueUpdate();
			}
			HandleResize();
		}

		void EPGameView::OnHover(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int buttons, int qualifiers, Urho3D::Cursor* cursor)
		{
			UIElement::OnHover(position, screenPosition, buttons, qualifiers, cursor);
		}

		void EPGameView::OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor)
		{
			UIElement::OnClickBegin(position, screenPosition, button, buttons, qualifiers, cursor);
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();
			ui->SetFocusElement(NULL);
		}

		void EPGameView::OnClickEnd(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor, Urho3D::UIElement* beginElement)
		{
			UIElement::OnClickEnd(position, screenPosition, button, buttons, qualifiers, cursor, beginElement);
		}

		void EPGameView::StartSceneUpdate()
		{
			if (scene_)
				scene_->SetUpdateEnabled(true);
		}

		void EPGameView::StopSceneUpdate()
		{
			if (scene_)
				scene_->SetUpdateEnabled(false);
		}

		void EPGameView::SetView(Urho3D::Scene* scene)
		{
			bool sceneUpdateEnabled = scene_ ? scene_->IsUpdateEnabled() : false;

			ResetScene();
			ResetCamera();

			Urho3D::SharedPtr<Urho3D::XMLFile> sceneData(new Urho3D::XMLFile(context_));
			Urho3D::XMLElement root = sceneData->CreateRoot("scene");

			scene->SaveXML(root);
			scene_->LoadXML(root);
			scene_->SetUpdateEnabled(sceneUpdateEnabled);

			viewport_->SetScene(scene_);
			viewport_->SetCamera(camera_);

			Urho3D::VariantMap map = context_->GetEventDataMap();
			map[GamePlay::SetCamera::P_CAMERA_NODE] = cameraNode_;
			SendEvent(GamePlay::E_SETCAMERA, map);

			QueueUpdate();
		}

		void EPGameView::SetFormat(unsigned format)
		{
			if (format != rttFormat_)
			{
				rttFormat_ = format;
				OnResize();
			}
		}

		void EPGameView::SetAutoUpdate(bool enable)
		{
			if (enable != autoUpdate_)
			{
				autoUpdate_ = enable;
				Urho3D::RenderSurface* surface = renderTexture_->GetRenderSurface();
				if (surface)
					surface->SetUpdateMode(autoUpdate_ ? Urho3D::SURFACE_UPDATEALWAYS : Urho3D::SURFACE_MANUALUPDATE);
			}
		}

		void EPGameView::QueueUpdate()
		{
			if (!autoUpdate_)
			{
				Urho3D::RenderSurface* surface = renderTexture_->GetRenderSurface();
				if (surface)
					surface->QueueUpdate();
			}
		}

		Urho3D::Scene* EPGameView::GetScene() const
		{
			return scene_;
		}

		Urho3D::Node* EPGameView::GetCameraNode() const
		{
			return cameraNode_;
		}

		void EPGameView::ResetCamera()
		{
			//cameraNode_->SetPosition(Urho3D::Vector3(0.0f, 5.0f, -10.0f));
			// Look at the origin so user can see the scene.
			//cameraNode_->SetRotation(Urho3D::Quaternion(Urho3D::Vector3(0.0f, 0.0f, 1.0f), -cameraNode_->GetPosition()));

			//	UpdateSettingsUI();
		}

		void EPGameView::CreateViewportContextUI(Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_)
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			Urho3D::Font* font = cache->GetResource<Urho3D::Font>("Fonts/Anonymous Pro.ttf");
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

			statusBar = CreateChild<BorderImage>("ToolBar");
			AddChild(statusBar);

			statusBar->SetStyle("EditorToolBar");

			statusBar->SetLayout(Urho3D::LM_HORIZONTAL);
			statusBar->SetAlignment(Urho3D::HA_LEFT, Urho3D::VA_BOTTOM);
			statusBar->SetLayoutSpacing(4);
			//statusBar->SetOpacity(editor_->GetuiMaxOpacity());

			Urho3D::Button* settingsButton = UI::UIUtils::CreateSmallToolBarButton(context_, uiStyle, iconStyle_, "Settings");
			statusBar->AddChild(settingsButton);

			cameraPosText = new  Urho3D::Text(context_);
			statusBar->AddChild(cameraPosText);

			cameraPosText->SetFont(font, 11);
			cameraPosText->SetColor(Urho3D::Color(1.0f, 1.0f, 0.0f));
			cameraPosText->SetTextEffect(Urho3D::TE_SHADOW);
			cameraPosText->SetPriority(-100);

			HandleResize();
		}

		Urho3D::Texture2D* EPGameView::GetRenderTexture() const
		{
			return renderTexture_;
		}

		Urho3D::Texture2D* EPGameView::GetDepthTexture() const
		{
			return depthTexture_;
		}

		Urho3D::Viewport* EPGameView::GetViewport() const
		{
			return viewport_;
		}

		void EPGameView::ResetScene()
		{
			scene_.Reset();
			scene_ = new Urho3D::Scene(context_);
		}

		void EPGameView::HandleResize()
		{
			statusBar->SetLayoutBorder(Urho3D::IntRect(8, 4, 4, 8));

			statusBar->SetFixedSize(GetWidth(), 22);
		}

		//void EPGameView::Update(float timeStep)
		//{
		//	//scene_->Update(timeStep);

		//	Urho3D::Vector3 cameraPos = cameraNode_->GetPosition();
		//	Urho3D::String xText(cameraPos.x_);
		//	Urho3D::String yText(cameraPos.y_);
		//	Urho3D::String zText(cameraPos.z_);
		//	xText.Resize(8);
		//	yText.Resize(8);
		//	zText.Resize(8);

		//	cameraPosText->SetText(Urho3D::String(
		//		"Pos: " + xText + " " + yText + " " + zText +
		//		" Zoom: " + Urho3D::String(camera_->GetZoom())));

		//	cameraPosText->SetSize(cameraPosText->GetMinSize());
		//}
	}
}