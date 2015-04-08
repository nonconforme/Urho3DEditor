#include "EPScene3D.h"
#include "EditorData.h"

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
		const int pickModeDrawableFlags[] = {
			Urho3D::DRAWABLE_GEOMETRY,
			Urho3D::DRAWABLE_LIGHT,
			Urho3D::DRAWABLE_ZONE
		};

		const Urho3D::String editModeText[] = {
			"Move",
			"Rotate",
			"Scale",
			"Select",
			"Spawn"
		};

		const Urho3D::String axisModeText[] = {
			"World",
			"Local"
		};

		const Urho3D::String pickModeText[] = {
			"Geometries",
			"Lights",
			"Zones",
			"Rigidbodies",
			"UI-elements"
		};

		const 	Urho3D::String fillModeText[] = {
			"Solid",
			"Wire",
			"Point"
		};

		EPScene3D::EPScene3D(Urho3D::Context* context) : EditorPlugin(context),
			showGrid_(true),
			grid2DMode_(false),
			sceneModified(false)
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

			gridColor = Urho3D::Color(0.1f, 0.1f, 0.1f);
			gridSubdivisionColor = Urho3D::Color(0.05f, 0.05f, 0.05f);
			gridXColor = Urho3D::Color(0.5f, 0.1f, 0.1f);
			gridYColor = Urho3D::Color(0.1f, 0.5f, 0.1f);
			gridZColor = Urho3D::Color(0.1f, 0.1f, 0.5f);

			toggledMouseLock_ = false;
			mouseOrbitMode = ORBIT_RELATIVE;
			instantiateMode = Urho3D::REPLICATED;
		}

		EPScene3D::~EPScene3D()
		{
		}

		bool EPScene3D::HasMainScreen()
		{
			return true;
		}

		Urho3D::String EPScene3D::GetName() const
		{
			return Urho3D::String("Edit");
		}

		void EPScene3D::Edit(Object *object)
		{
			if (Urho3D::Component::GetTypeStatic() == object->GetBaseType())
			{
			}

			if (Urho3D::Node::GetTypeStatic() == object->GetBaseType())
			{
			}
		}

		bool EPScene3D::Handles(Object *object) const
		{
			if (Urho3D::Component::GetTypeStatic() == object->GetBaseType())
				return true;
			if (Urho3D::Node::GetTypeStatic() == object->GetBaseType())
				return true;
			// 		if (Scene::GetTypeStatic() == object->GetType())
			// 			return true;
			return false;
		}

		Urho3D::UIElement* EPScene3D::GetMainScreen()
		{
			if (!window_)
				Start();

			return window_;
		}

		void EPScene3D::SetVisible(bool visible)
		{
			if (visible_ == visible)
				return;

			visible_ = visible;

			if (window_)
			{
				window_->SetVisible(visible);
				sceneMenu_->SetVisible(visible);
				createMenu_->SetVisible(visible);

				for (unsigned i = 0; i < miniToolBarButtons_.Size(); i++)
					miniToolBarButtons_[i]->SetVisible(visible);
			
				for (unsigned i = 0; i < toolBarToggles.Size(); i++)		
					toolBarToggles[i]->SetVisible(visible);
					
				if (visible)
				{
					SubscribeToEvent(Urho3D::E_POSTRENDERUPDATE, HANDLER(EPScene3D, HandlePostRenderUpdate));
					SubscribeToEvent(Urho3D::E_UIMOUSECLICK, HANDLER(EPScene3D, ViewMouseClick));
					SubscribeToEvent(Urho3D::E_MOUSEMOVE, HANDLER(EPScene3D, ViewMouseMove));
					SubscribeToEvent(Urho3D::E_UIMOUSECLICKEND, HANDLER(EPScene3D, ViewMouseClickEnd));
					SubscribeToEvent(Urho3D::E_BEGINVIEWUPDATE, HANDLER(EPScene3D, HandleBeginViewUpdate));
					SubscribeToEvent(Urho3D::E_ENDVIEWUPDATE, HANDLER(EPScene3D, HandleEndViewUpdate));
					SubscribeToEvent(Urho3D::E_BEGINVIEWRENDER, HANDLER(EPScene3D, HandleBeginViewRender));
					SubscribeToEvent(Urho3D::E_ENDVIEWRENDER, HANDLER(EPScene3D, HandleEndViewRender));
					gizmo_->ShowGizmo();
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
					gizmo_->HideGizmo();
					activeView->SetAutoUpdate(false);
				}
			}
		}

		void EPScene3D::Update(float timeStep)
		{
			UpdateStats(timeStep);

			if (runUpdate)
				editorData_->GetEditorScene()->Update(timeStep);

			if (toolBarDirty && editorView_->IsToolBarVisible())
				UpdateToolBar();

			gizmo_->UpdateGizmo();

			if (ui_->HasModalElement() || ui_->GetFocusElement() != NULL)
			{
				ReleaseMouseLock();
				return;
			}

			// Move camera
			if (!input_->GetKeyDown(Urho3D::KEY_LCTRL))
			{
				float speedMultiplier = 1.0;
				if (input_->GetKeyDown(Urho3D::KEY_LSHIFT))
					speedMultiplier = cameraShiftSpeedMultiplier;

				if (input_->GetKeyDown('W') || input_->GetKeyDown(Urho3D::KEY_UP))
				{
					cameraNode_->Translate(Urho3D::Vector3(0.0f, 0.0f, cameraBaseSpeed) * timeStep * speedMultiplier);
				}
				if (input_->GetKeyDown('S') || input_->GetKeyDown(Urho3D::KEY_DOWN))
				{
					cameraNode_->Translate(Urho3D::Vector3(0.0f, 0.0f, -cameraBaseSpeed) * timeStep * speedMultiplier);
				}
				if (input_->GetKeyDown('A') || input_->GetKeyDown(Urho3D::KEY_LEFT))
				{
					cameraNode_->Translate(Urho3D::Vector3(-cameraBaseSpeed, 0.0f, 0.0f) * timeStep * speedMultiplier);
				}
				if (input_->GetKeyDown('D') || input_->GetKeyDown(Urho3D::KEY_RIGHT))
				{
					cameraNode_->Translate(Urho3D::Vector3(cameraBaseSpeed, 0.0f, 0.0f) * timeStep * speedMultiplier);
				}
				if (input_->GetKeyDown('E') || input_->GetKeyDown(Urho3D::KEY_PAGEUP))
				{
					cameraNode_->Translate(Urho3D::Vector3(0.0f, cameraBaseSpeed, 0.0f) * timeStep * speedMultiplier, Urho3D::TS_WORLD);
				}
				if (input_->GetKeyDown('Q') || input_->GetKeyDown(Urho3D::KEY_PAGEDOWN))
				{
					cameraNode_->Translate(Urho3D::Vector3(0.0f, -cameraBaseSpeed, 0.0f) * timeStep * speedMultiplier, Urho3D::TS_WORLD);
				}
				if (input_->GetMouseMoveWheel() != 0 && ui_->GetElementAt(ui_->GetCursor()->GetPosition()) == NULL)
				{
					if (mouseWheelCameraPosition)
					{
						cameraNode_->Translate(Urho3D::Vector3(0.0f, 0.0f, -cameraBaseSpeed) * -(float)input_->GetMouseMoveWheel() * 20.0f * timeStep *
							speedMultiplier);
					}
					else
					{
						float zoom = camera_->GetZoom() + -(float)input_->GetMouseMoveWheel() *.1f * speedMultiplier;
						camera_->SetZoom(Urho3D::Clamp(zoom, .1f, 30.0f));
					}
				}
			}

			// Rotate/orbit/pan camera
			if (input_->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT) || input_->GetMouseButtonDown(Urho3D::MOUSEB_MIDDLE))
			{
				SetMouseLock();

				Urho3D::IntVector2 mouseMove = input_->GetMouseMove();
				if (mouseMove.x_ != 0 || mouseMove.y_ != 0)
				{
					if (input_->GetKeyDown(Urho3D::KEY_LSHIFT) && input_->GetMouseButtonDown(Urho3D::MOUSEB_MIDDLE))
					{
						cameraNode_->Translate(Urho3D::Vector3(-(float)mouseMove.x_, (float)mouseMove.y_, 0.0f) * timeStep * cameraBaseSpeed * 0.5f);
					}
					else
					{
						activeView->cameraYaw_ += mouseMove.x_ * cameraBaseRotationSpeed;
						activeView->cameraPitch_ += mouseMove.y_ * cameraBaseRotationSpeed;

						if (limitRotation)
							activeView->cameraPitch_ = Urho3D::Clamp(activeView->cameraPitch_, -90.0, 90.0);

						Urho3D::Quaternion q = Urho3D::Quaternion(activeView->cameraPitch_, activeView->cameraYaw_, 0);
						cameraNode_->SetRotation(q);

						if (input_->GetMouseButtonDown(Urho3D::MOUSEB_MIDDLE) && (editorSelection_->GetNumSelectedNodes() > 0 || editorSelection_->GetNumSelectedComponents() > 0))
						{
							Urho3D::Vector3 centerPoint = SelectedNodesCenterPoint();
							Urho3D::Vector3 d = cameraNode_->GetWorldPosition() - centerPoint;
							cameraNode_->SetWorldPosition(centerPoint - q * Urho3D::Vector3(0.0, 0.0, d.Length()));
							orbiting = true;
						}
					}
				}
			}
			else
				ReleaseMouseLock();

			if (orbiting && !input_->GetMouseButtonDown(Urho3D::MOUSEB_MIDDLE))
				orbiting = false;

			// Move/rotate/scale object
			if (!editorSelection_->GetEditNodes().Empty() && editMode != EDIT_SELECT && input_->GetKeyDown(Urho3D::KEY_LCTRL))
			{
				Urho3D::Vector3 adjust(0, 0, 0);
				if (input_->GetKeyDown(Urho3D::KEY_UP))
					adjust.z_ = 1;
				if (input_->GetKeyDown(Urho3D::KEY_DOWN))
					adjust.z_ = -1;
				if (input_->GetKeyDown(Urho3D::KEY_LEFT))
					adjust.x_ = -1;
				if (input_->GetKeyDown(Urho3D::KEY_RIGHT))
					adjust.x_ = 1;
				if (input_->GetKeyDown(Urho3D::KEY_PAGEUP))
					adjust.y_ = 1;
				if (input_->GetKeyDown(Urho3D::KEY_PAGEDOWN))
					adjust.y_ = -1;
				if (editMode == EDIT_SCALE)
				{
					if (input_->GetKeyDown(Urho3D::KEY_KP_PLUS))
						adjust = Urho3D::Vector3(1, 1, 1);
					if (input_->GetKeyDown(Urho3D::KEY_KP_MINUS))
						adjust = Urho3D::Vector3(-1, -1, -1);
				}

				if (adjust == Urho3D::Vector3(0, 0, 0))
					return;

				bool moved = false;
				adjust *= timeStep * 10;

				switch (editMode)
				{
				case EDIT_MOVE:
					if (!moveSnap)
						moved = MoveNodes(adjust * moveStep);
					break;

				case EDIT_ROTATE:
					if (!rotateSnap)
						moved = RotateNodes(adjust * rotateStep);
					break;

				case EDIT_SCALE:
					if (!scaleSnap)
						moved = ScaleNodes(adjust * scaleStep);
					break;
				}

				// 			if (moved)
				// 				UpdateNodeAttributes();
			}
		}

		void EPScene3D::ResetCamera()
		{
			//for (uint i = 0; i < viewports.length; ++i)
			activeView->ResetCamera();
		}

		void EPScene3D::ReacquireCameraYawPitch()
		{
			//for (uint i = 0; i < viewports.length; ++i)
			activeView->ReacquireCameraYawPitch();
		}

		void EPScene3D::UpdateViewParameters()
		{
			//for (uint i = 0; i < viewports.length; ++i)
			{
				activeView->camera_->SetNearClip(viewNearClip);
				activeView->camera_->SetFarClip(viewFarClip);
				activeView->camera_->SetFov(viewFov);
			}
		}

		void EPScene3D::CreateStatsBar()
		{
			Urho3D::Font* font = cache_->GetResource<Urho3D::Font>("Fonts/Anonymous Pro.ttf");

			editorModeText = new Urho3D::Text(context_);
			activeView->AddChild(editorModeText);
			renderStatsText = new Urho3D::Text(context_);
			activeView->AddChild(renderStatsText);

			if (window_->GetWidth() >= 1200)
			{
				SetupStatsBarText(editorModeText, font, 35, 64, Urho3D::HA_LEFT, Urho3D::VA_TOP);
				SetupStatsBarText(renderStatsText, font, -4, 64, Urho3D::HA_RIGHT, Urho3D::VA_TOP);
			}
			else
			{
				SetupStatsBarText(editorModeText, font, 1, 1, Urho3D::HA_LEFT, Urho3D::VA_TOP);
				SetupStatsBarText(renderStatsText, font, 1, 15, Urho3D::HA_LEFT, Urho3D::VA_TOP);
			}
		}

		void EPScene3D::SetupStatsBarText(Urho3D::Text* text, Urho3D::Font* font, int x, int y, Urho3D::HorizontalAlignment hAlign, Urho3D::VerticalAlignment vAlign)
		{
			text->SetPosition(Urho3D::IntVector2(x, y));
			text->SetHorizontalAlignment(hAlign);
			text->SetVerticalAlignment(vAlign);
			text->SetFont(font, 11);
			text->SetColor(Urho3D::Color(1, 1, 0));
			text->SetTextEffect(Urho3D::TE_SHADOW);
			text->SetPriority(-100);
		}

		void EPScene3D::UpdateStats(float timeStep)
		{
			editorModeText->SetText(Urho3D::String(
				"Mode: " + editModeText[editMode] +
				"  Axis: " + axisModeText[axisMode] +
				"  Pick: " + pickModeText[pickMode] +
				"  Fill: " + fillModeText[fillMode] +
				"  Updates: " + (runUpdate ? "Running" : "Paused")));

			renderStatsText->SetText(Urho3D::String(
				"Tris: " + Urho3D::String(renderer->GetNumPrimitives()) +
				"  Batches: " + Urho3D::String(renderer->GetNumBatches()) +
				"  Lights: " + Urho3D::String(renderer->GetNumLights(true)) +
				"  Shadowmaps: " + Urho3D::String(renderer->GetNumShadowMaps(true)) +
				"  Occluders: " + Urho3D::String(renderer->GetNumOccluders(true))));

			editorModeText->SetSize(editorModeText->GetMinSize());
			renderStatsText->SetSize(renderStatsText->GetMinSize());
		}

		void EPScene3D::SetFillMode(Urho3D::FillMode fM_)
		{
			fillMode = fM_;
	// 		for (uint i = 0; i < viewports.length; ++i)
	// 			viewports[i].camera.fillMode = fillMode_;
			camera_->SetFillMode(fM_);
		}

		void EPScene3D::Start()
		{
			EditorData* editorData_ = GetSubsystem<EditorData>();
			window_ = new Urho3D::UIElement(context_);
			window_->SetDefaultStyle(editorData_->GetDefaultStyle());

			activeView = window_->CreateChild<EPScene3DView>("Scene3DView");
			activeView->SetDefaultStyle(editorData_->GetDefaultStyle());
			activeView->SetView(editorData_->GetEditorScene());
			activeView->CreateViewportContextUI(editorData_->GetDefaultStyle(), editorData_->GetIconStyle());
			cameraNode_ = activeView->GetCameraNode();
			camera_ = activeView->GetCamera();
			activeView->SetAutoUpdate(true);

			CreateGrid();
			ShowGrid();
			CreateStatsBar();

			SubscribeToEvent(window_, Urho3D::E_RESIZED, HANDLER(EPScene3D, HandleResizeView));

			//////////////////////////////////////////////////////////////////////////
			/// Menu Bar entries

			sceneMenu_ = editorView_->GetGetMenuBar()->CreateMenu("Scene");
			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "New scene", UI::A_NEWSCENE_VAR, 'N', Urho3D::QUAL_SHIFT | Urho3D::QUAL_CTRL);
			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "Open scene...", UI::A_OPENSCENE_VAR, 'O', Urho3D::QUAL_CTRL);
			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "Save scene", UI::A_SAVESCENE_VAR, 'S', Urho3D::QUAL_CTRL);
			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "Save scene as...", UI::A_SAVESCENEAS_VAR, 'S', Urho3D::QUAL_SHIFT | Urho3D::QUAL_CTRL);

			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "Load node as replicated", UI::A_LOADNODEASREP_VAR);
			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "Load node as local", UI::A_LOADNODEASLOCAL_VAR);
			editorView_->GetGetMenuBar()->CreateMenuItem("Scene", "Save node as", UI::A_SAVENODEAS_VAR);

			createMenu_ = editorView_->GetGetMenuBar()->CreateMenu("Create");

			editorView_->GetGetMenuBar()->CreateMenuItem("Create", "Replicated node", UI::A_CREATEREPNODE_VAR, 0, 0, true, "Create Replicated node");
			editorView_->GetGetMenuBar()->CreateMenuItem("Create", "Local node", UI::A_CREATELOCALNODE_VAR, 0, 0, true, "Create Local node");

			Urho3D::Menu* childMenu = editorView_->GetGetMenuBar()->CreateMenuItem("Create", "Component", Urho3D::StringHash::ZERO, UI::SHOW_POPUP_INDICATOR);
			Urho3D::Window* childPopup = editorView_->GetGetMenuBar()->CreatePopupMenu(childMenu);

			const Urho3D::HashMap<Urho3D::String, Urho3D::Vector<Urho3D::StringHash> >& objectCategories = context_->GetObjectCategories();
			Urho3D::HashMap<Urho3D::String, Urho3D::Vector<Urho3D::StringHash> >::ConstIterator it;

			/// Mini Tool Bar entries 
			/// \todo create scroll bar for the mini tool bar or something ... because there are to many components that can be added this way 
	// 		MiniToolBarUI* minitool = editorView_->GetMiniToolBar();
	// 		Button* b = (Button*)minitool->CreateSmallToolBarButton("Node", "Replicated Node");
	// 		miniToolBarButtons_.Push(b);
	// 		SubscribeToEvent(b, E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateReplNode));
	// 		b = (Button*)minitool->CreateSmallToolBarButton("Node", "Local Node");
	// 		miniToolBarButtons_.Push(b);
	// 		SubscribeToEvent(b, E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateLocalNode));
		

			for (it = objectCategories.Begin(); it != objectCategories.End(); it++)
			{
				// Skip the UI category for the component menus
				if (it->first_ == "UI")
					continue;

				Urho3D::Menu* menu = editorView_->GetGetMenuBar()->CreatePopupMenuItem(childPopup, it->first_, Urho3D::StringHash::ZERO, UI::SHOW_POPUP_INDICATOR);
				Urho3D::Window* popup = editorView_->GetGetMenuBar()->CreatePopupMenu(menu);

				/// GetObjectsByCategory
				Urho3D::Vector<Urho3D::String> components;
				const Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Urho3D::ObjectFactory> >& factories = context_->GetObjectFactories();
				const Urho3D::Vector<Urho3D::StringHash>& factoryHashes = it->second_;
				components.Reserve(factoryHashes.Size());
				for (unsigned j = 0; j < factoryHashes.Size(); ++j)
				{
					Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Urho3D::ObjectFactory> >::ConstIterator k = factories.Find(factoryHashes[j]);
					if (k != factories.End())
						components.Push(k->second_->GetTypeName());
				}
				//minitool->CreateSmallToolBarSpacer(3);
				/// \todo CreateIconizedMenuItem
				for (unsigned j = 0; j < components.Size(); ++j)
				{
					editorView_->GetGetMenuBar()->CreatePopupMenuItem(popup, components[j], UI::A_CREATECOMPONENT_VAR);
					/// Mini Tool Bar entries
	// 				b = (Button*)minitool->CreateSmallToolBarButton(components[j]);
	// 				miniToolBarButtons_.Push(b);
				}
				
			}

			childMenu = editorView_->GetGetMenuBar()->CreateMenuItem("Create", "Builtin object", Urho3D::StringHash::ZERO, UI::SHOW_POPUP_INDICATOR);
			childPopup = editorView_->GetGetMenuBar()->CreatePopupMenu(childMenu);
			Urho3D::String objects[] = { "Box", "Cone", "Cylinder", "Plane", "Pyramid", "Sphere", "TeaPot", "Torus" };
			for (int i = 0; i < 8; i++)
			{
				editorView_->GetGetMenuBar()->CreatePopupMenuItem(childPopup, objects[i], UI::A_CREATEBUILTINOBJ_VAR);
			}

			SubscribeToEvent(editorView_->GetGetMenuBar(), UI::E_MENUBAR_ACTION, HANDLER(EPScene3D, HandleMenuBarAction));

		
			/// Mini Tool Bar entries
			CreateMiniToolBarUI();

			/// Tool Bar entries
			CreateToolBarUI();

			/// create gizmo

			gizmo_ = new GizmoScene3D(context_, this);

			gizmo_->CreateGizmo();
			gizmo_->ShowGizmo();
		}

		void EPScene3D::CreateMiniToolBarUI()
		{

			UI::MiniToolBarUI* minitool = editorView_->GetMiniToolBar();

			Urho3D::Button* b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Node", "Replicated Node");
			miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateReplNode));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Node", "Local Node");
			miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateLocalNode));

			minitool->CreateSmallToolBarSpacer(3);
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Light");
			miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Camera");
			miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Zone");
			miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("StaticModel");
			miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("AnimatedModel"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("BillboardSet"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("ParticleEmitter"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Skybox"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Terrain"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Text3D"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));

			minitool->CreateSmallToolBarSpacer(3);
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("SoundListener"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("SoundSource3D"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("SoundSource"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));

			minitool->CreateSmallToolBarSpacer(3);
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("RigidBody"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("CollisionShape"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Constraint"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));

			minitool->CreateSmallToolBarSpacer(3);
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("AnimationController"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("ScriptInstance"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));

			minitool->CreateSmallToolBarSpacer(3);
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("Navigable"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("NavigationMesh"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("OffMeshConnection"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
			minitool->CreateSmallToolBarSpacer(3);
			b = (Urho3D::Button*)minitool->CreateSmallToolBarButton("NetworkPriority"); miniToolBarButtons_.Push(b);
			SubscribeToEvent(b, Urho3D::E_RELEASED, HANDLER(EPScene3D, MiniToolBarCreateComponent));
		}

		void EPScene3D::CreateToolBarUI()
		{
			UI::ToolBarUI* minitool = editorView_->GetToolBar();

			Urho3D::UIElement* e = minitool->CreateGroup("EditModeGroup", Urho3D::LM_HORIZONTAL);
			toolBarToggles.Push(e);
			Urho3D::CheckBox* checkbox = minitool->CreateToolBarToggle("EditModeGroup", "EditMove");
			if (checkbox->IsChecked() != (editMode == EDIT_MOVE))
				checkbox->SetChecked(editMode == EDIT_MOVE);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarEditModeMove));
			checkbox = minitool->CreateToolBarToggle("EditModeGroup", "EditRotate");
			if (checkbox->IsChecked() != (editMode == EDIT_ROTATE))
				checkbox->SetChecked(editMode == EDIT_ROTATE);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarEditModeRotate));
			checkbox = minitool->CreateToolBarToggle("EditModeGroup", "EditScale");
			if (checkbox->IsChecked() != (editMode == EDIT_SCALE))
				checkbox->SetChecked(editMode == EDIT_SELECT);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarEditModeScale));
			checkbox = minitool->CreateToolBarToggle("EditModeGroup", "EditSelect");
			if (checkbox->IsChecked() != (editMode == EDIT_SELECT))
				checkbox->SetChecked(editMode == EDIT_SELECT);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarEditModeSelect));

			e = minitool->CreateGroup("AxisModeGroup", Urho3D::LM_HORIZONTAL);
			toolBarToggles.Push(e);
			checkbox = minitool->CreateToolBarToggle("AxisModeGroup", "AxisWorld");
			if (checkbox->IsChecked() != (axisMode == AXIS_WORLD))
				checkbox->SetChecked(axisMode == AXIS_WORLD);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarAxisModeWorld));
			checkbox = minitool->CreateToolBarToggle("AxisModeGroup", "AxisLocal");
			if (checkbox->IsChecked() != (axisMode == AXIS_LOCAL))
				checkbox->SetChecked((axisMode == AXIS_LOCAL));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarAxisModeLocal));

			minitool->CreateToolBarSpacer(4);
			checkbox = minitool->CreateToolBarToggle("MoveSnap");
			if (checkbox->IsChecked() != moveSnap)
				checkbox->SetChecked(moveSnap);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarMoveSnap));
			toolBarToggles.Push(checkbox);
			checkbox = minitool->CreateToolBarToggle("RotateSnap");
			if (checkbox->IsChecked() != rotateSnap)
				checkbox->SetChecked(rotateSnap);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarRotateSnap));
			toolBarToggles.Push(checkbox);
			checkbox = minitool->CreateToolBarToggle("ScaleSnap");
			if (checkbox->IsChecked() != scaleSnap)
				checkbox->SetChecked(scaleSnap);
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarScaleSnap));
			toolBarToggles.Push(checkbox);

			e = minitool->CreateGroup("SnapScaleModeGroup", Urho3D::LM_HORIZONTAL);
			toolBarToggles.Push(e);
			checkbox = minitool->CreateToolBarToggle("SnapScaleModeGroup", "SnapScaleHalf");
			if (checkbox->IsChecked() != (snapScaleMode == SNAP_SCALE_HALF))
				checkbox->SetChecked((snapScaleMode == SNAP_SCALE_HALF));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarSnapScaleModeHalf));
			checkbox = minitool->CreateToolBarToggle("SnapScaleModeGroup", "SnapScaleQuarter");
			if (checkbox->IsChecked() != (snapScaleMode == SNAP_SCALE_QUARTER))
				checkbox->SetChecked((snapScaleMode == SNAP_SCALE_QUARTER));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarSnapScaleModeQuarter));

			minitool->CreateToolBarSpacer(4);
			e = minitool->CreateGroup("PickModeGroup", Urho3D::LM_HORIZONTAL);
			toolBarToggles.Push(e);
			checkbox = minitool->CreateToolBarToggle("PickModeGroup", "PickGeometries");
			if (checkbox->IsChecked() != (pickMode == UI::PICK_GEOMETRIES))
				checkbox->SetChecked((pickMode == UI::PICK_GEOMETRIES));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarPickModeGeometries));
			checkbox = minitool->CreateToolBarToggle("PickModeGroup", "PickLights");
			if (checkbox->IsChecked() != (pickMode == UI::PICK_LIGHTS))
				checkbox->SetChecked((pickMode == UI::PICK_LIGHTS));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarPickModeLights));
			checkbox = minitool->CreateToolBarToggle("PickModeGroup", "PickZones");
			if (checkbox->IsChecked() != (pickMode == UI::PICK_ZONES))
				checkbox->SetChecked((pickMode == UI::PICK_ZONES));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarPickModeZones));
			checkbox = minitool->CreateToolBarToggle("PickModeGroup", "PickRigidBodies");
			if (checkbox->IsChecked() != (pickMode == UI::PICK_RIGIDBODIES))
				checkbox->SetChecked((pickMode == UI::PICK_RIGIDBODIES));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarPickModeRigidBodies));
			checkbox = minitool->CreateToolBarToggle("PickModeGroup", "PickUIElements");
			if (checkbox->IsChecked() != (pickMode == UI::PICK_UI_ELEMENTS))
				checkbox->SetChecked((pickMode == UI::PICK_UI_ELEMENTS));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarPickModeUIElements));

			minitool->CreateToolBarSpacer(4);
			e = minitool->CreateGroup("FillModeGroup", Urho3D::LM_HORIZONTAL);
			toolBarToggles.Push(e);
			checkbox = minitool->CreateToolBarToggle("FillModeGroup", "FillPoint");
			if (checkbox->IsChecked() != (fillMode == Urho3D::FILL_POINT))
				checkbox->SetChecked((fillMode == Urho3D::FILL_POINT));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarFillModePoint));
			checkbox = minitool->CreateToolBarToggle("FillModeGroup", "FillWireFrame");
			if (checkbox->IsChecked() != (fillMode == Urho3D::FILL_WIREFRAME))
				checkbox->SetChecked((fillMode == Urho3D::FILL_WIREFRAME));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarFillModeWireFrame));
			checkbox = minitool->CreateToolBarToggle("FillModeGroup", "FillSolid");
			if (checkbox->IsChecked() != (fillMode == Urho3D::FILL_SOLID))
				checkbox->SetChecked((fillMode == Urho3D::FILL_SOLID));
			SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(EPScene3D, ToolBarFillModeSolid));


		}

		Urho3D::Vector3 EPScene3D::SelectedNodesCenterPoint()
		{
			Urho3D::Vector3 centerPoint;
			unsigned int count = editorSelection_->GetNumSelectedNodes();
			for (unsigned int i = 0; i < editorSelection_->GetNumSelectedNodes(); ++i)
				centerPoint += editorSelection_->GetSelectedNodes()[i]->GetWorldPosition();

			for (unsigned int i = 0; i < editorSelection_->GetNumSelectedComponents(); ++i)
			{
				Urho3D::Drawable* drawable = dynamic_cast<Urho3D::Drawable*>(editorSelection_->GetSelectedComponents()[i]);
				count++;
				if (drawable != NULL)
					centerPoint += drawable->GetNode()->LocalToWorld(drawable->GetBoundingBox().Center());
				else
					centerPoint += editorSelection_->GetSelectedComponents()[i]->GetNode()->GetWorldPosition();
			}

			if (count > 0)
				return centerPoint / float(count);
			else
				return centerPoint;
		}

		void EPScene3D::DrawNodeDebug(Urho3D::Node* node, Urho3D::DebugRenderer* debug, bool drawNode /*= true*/)
		{
			if (drawNode)
				debug->AddNode(node, 1.0f, false);

			// Exception for the scene to avoid bringing the editor to its knees: drawing either the whole hierarchy or the subsystem-
			// components can have a large performance hit. Also do not draw terrain child nodes due to their large amount
			// (TerrainPatch component itself draws nothing as debug geometry)
			if (node != editorData_->GetEditorScene() && node->GetComponent<Urho3D::Terrain>() == NULL)
			{
				for (unsigned int j = 0; j < node->GetNumComponents(); ++j)
					node->GetComponents()[j]->DrawDebugGeometry(debug, false);

				// To avoid cluttering the view, do not draw the node axes for child nodes
				for (unsigned int k = 0; k < node->GetNumChildren(); ++k)
					DrawNodeDebug(node->GetChildren()[k], debug, false);
			}
		}

		bool EPScene3D::MoveNodes(Urho3D::Vector3 adjust)
		{
			bool moved = false;

			if (adjust.Length() > Urho3D::M_EPSILON)
			{
				for (unsigned int i = 0; i < editorSelection_->GetNumEditNodes(); ++i)
				{
					if (moveSnap)
					{
						float moveStepScaled = moveStep * snapScale;
						adjust.x_ = floor(adjust.x_ / moveStepScaled + 0.5f) * moveStepScaled;
						adjust.y_ = floor(adjust.y_ / moveStepScaled + 0.5f) * moveStepScaled;
						adjust.z_ = floor(adjust.z_ / moveStepScaled + 0.5f) * moveStepScaled;
					}

					Urho3D::Node* node = editorSelection_->GetEditNodes()[i];
					Urho3D::Vector3 nodeAdjust = adjust;
					if (axisMode == AXIS_LOCAL && editorSelection_->GetNumEditNodes() == 1)
						nodeAdjust = node->GetWorldRotation() * nodeAdjust;

					Urho3D::Vector3 worldPos = node->GetWorldPosition();
					Urho3D::Vector3 oldPos = node->GetPosition();

					worldPos += nodeAdjust;

					if (node->GetParent() == NULL)
						node->SetPosition(worldPos);
					else
						node->SetPosition(node->GetParent()->WorldToLocal(worldPos));

					if (node->GetPosition() != oldPos)
						moved = true;
				}
			}

			return moved;
		}

		bool EPScene3D::RotateNodes(Urho3D::Vector3 adjust)
		{
			bool moved = false;

			if (rotateSnap)
			{
				float rotateStepScaled = rotateStep * snapScale;
				adjust.x_ = floor(adjust.x_ / rotateStepScaled + 0.5f) * rotateStepScaled;
				adjust.y_ = floor(adjust.y_ / rotateStepScaled + 0.5f) * rotateStepScaled;
				adjust.z_ = floor(adjust.z_ / rotateStepScaled + 0.5f) * rotateStepScaled;
			}

			if (adjust.Length() > Urho3D::M_EPSILON)
			{
				moved = true;

				for (unsigned int i = 0; i < editorSelection_->GetNumEditNodes(); ++i)
				{
					Urho3D::Node* node = editorSelection_->GetEditNodes()[i];
					Urho3D::Quaternion rotQuat(adjust.x_, adjust.y_, adjust.z_);
					if (axisMode == AXIS_LOCAL && editorSelection_->GetNumEditNodes() == 1)
						node->SetRotation(node->GetRotation() * rotQuat);
					else
					{
						Urho3D::Vector3 offset = node->GetWorldPosition();/// \todo -gizmoAxisX.axisRay.origin;

						if (node->GetParent() != NULL && node->GetParent()->GetWorldRotation() != Urho3D::Quaternion(1, 0, 0, 0))
							rotQuat = node->GetParent()->GetWorldRotation().Inverse() * rotQuat * node->GetParent()->GetWorldRotation();

						node->SetRotation(rotQuat * node->GetRotation());
						Urho3D::Vector3 newPosition = rotQuat * offset; /// \todo gizmoAxisX.axisRay.origin +

						if (node->GetParent() != NULL)
							newPosition = node->GetParent()->WorldToLocal(newPosition);

						node->SetPosition(newPosition);
					}
				}
			}

			return moved;
		}

		bool EPScene3D::ScaleNodes(Urho3D::Vector3 adjust)
		{
			bool moved = false;

			if (adjust.Length() > Urho3D::M_EPSILON)
			{
				for (unsigned int i = 0; i < editorSelection_->GetNumEditNodes(); ++i)
				{
					Urho3D::Node* node = editorSelection_->GetEditNodes()[i];

					Urho3D::Vector3 scale = node->GetScale();
					Urho3D::Vector3 oldScale = scale;

					if (!scaleSnap)
						scale += adjust;
					else
					{
						float scaleStepScaled = scaleStep * snapScale;
						if (adjust.x_ != 0)
						{
							scale.x_ += adjust.x_ * scaleStepScaled;
							scale.x_ = floor(scale.x_ / scaleStepScaled + 0.5f) * scaleStepScaled;
						}
						if (adjust.y_ != 0)
						{
							scale.y_ += adjust.y_ * scaleStepScaled;
							scale.y_ = floor(scale.y_ / scaleStepScaled + 0.5f) * scaleStepScaled;
						}
						if (adjust.z_ != 0)
						{
							scale.z_ += adjust.z_ * scaleStepScaled;
							scale.z_ = floor(scale.z_ / scaleStepScaled + 0.5f) * scaleStepScaled;
						}
					}

					if (scale != oldScale)
						moved = true;

					node->SetScale(scale);
				}
			}

			return moved;
		}

		void EPScene3D::ViewRaycast(bool mouseClick)
		{
			// Ignore if UI has modal element
			if (ui_->HasModalElement())
				return;

			// Ignore if mouse is grabbed by other operation
			if (input_->IsMouseGrabbed())
				return;
			Urho3D::Scene* editorScene = editorData_->GetEditorScene();

			Urho3D::Input* input = GetSubsystem<Urho3D::Input>();

			Urho3D::IntVector2 pos = ui_->GetCursorPosition();
			Urho3D::UIElement* elementAtPos = ui_->GetElementAt(pos, pickMode != UI::PICK_UI_ELEMENTS);
			if (editMode == EDIT_SPAWN)
			{
	// 			if (mouseClick && input_->GetMouseButtonPress(MOUSEB_LEFT) && elementAtPos == NULL)
	// 				SpawnObject();
				return;
			}

			// Do not raycast / change selection if hovering over the gizmo
			if (gizmo_->IsGizmoSelected())
				return;

			Urho3D::DebugRenderer* debug = editorScene->GetComponent<Urho3D::DebugRenderer>();

			if (pickMode == UI::PICK_UI_ELEMENTS)
			{
	// 			bool leftClick = mouseClick && input->GetMouseButtonPress(MOUSEB_LEFT);
	// 			bool multiselect = input->GetQualifierDown(QUAL_CTRL);
	// 
	// 			// Only interested in user-created UI elements
	// 			if (elementAtPos != NULL && elementAtPos != editorUIElement && elementAtPos.GetElementEventSender() == editorUIElement)
	// 			{
	// 				ui.DebugDraw(elementAtPos);
	// 
	// 				if (leftClick)
	// 					SelectUIElement(elementAtPos, multiselect);
	// 			}
	// 			// If clicked on emptiness in non-multiselect mode, clear the selection
	// 			else if (leftClick && !multiselect && ui.GetElementAt(pos) is null)
	// 				hierarchyList.ClearSelection();

				return;
			}

			// Do not raycast / change selection if hovering over a UI element when not in PICK_UI_ELEMENTS Mode
			if (elementAtPos != activeView)
				return;
			const Urho3D::IntVector2& screenpos = activeView->GetScreenPosition();
			float	posx = float(pos.x_ - screenpos.x_) / float(activeView->GetWidth());
			float	posy = float(pos.y_ - screenpos.y_) / float(activeView->GetHeight());
			Urho3D::Ray cameraRay = camera_->GetScreenRay(posx, posy);

			Urho3D::Component* selectedComponent = NULL;
			if (pickMode < UI::PICK_RIGIDBODIES)
			{
				if (editorScene->GetComponent<Urho3D::Octree>() == NULL)
					return;

				Urho3D::PODVector<Urho3D::RayQueryResult> result_;
				Urho3D::RayOctreeQuery query_(result_, cameraRay, Urho3D::RAY_TRIANGLE, camera_->GetFarClip(), pickModeDrawableFlags[pickMode], 0x7fffffff);
				editorScene->GetComponent<Urho3D::Octree>()->RaycastSingle(query_);

				if (result_.Size() != 0 && result_[0].drawable_ != NULL)
				{
					Urho3D::Drawable* drawable = result_[0].drawable_;
					// If selecting a terrain patch, select the parent terrain instead
					if (drawable->GetTypeName() != "TerrainPatch")
					{
						selectedComponent = drawable;
						if (debug != NULL)
						{
							debug->AddNode(drawable->GetNode(), 1.0, false);
							drawable->DrawDebugGeometry(debug, false);
						}
					}
					else if (drawable->GetNode()->GetParent() != NULL)
						selectedComponent = drawable->GetNode()->GetParent()->GetComponent<Urho3D::Terrain>();
				}
			}
			else
			{
				if (editorScene->GetComponent<Urho3D::PhysicsWorld>() == NULL)
					return;

				// If we are not running the actual physics update, refresh collisions before raycasting
				if (!runUpdate)
					editorScene->GetComponent<Urho3D::PhysicsWorld>()->UpdateCollisions();

				Urho3D::PhysicsRaycastResult result;
				editorScene->GetComponent<Urho3D::PhysicsWorld>()->RaycastSingle(result, cameraRay, camera_->GetFarClip());

				if (result.body_ != NULL)
				{
					Urho3D::RigidBody* body = result.body_;
					if (debug != NULL)
					{
						debug->AddNode(body->GetNode(), 1.0, false);
						body->DrawDebugGeometry(debug, false);
					}
					selectedComponent = body;
				}
			}

			if (mouseClick && input->GetMouseButtonPress(Urho3D::MOUSEB_LEFT))
			{
				bool multiselect = input->GetQualifierDown(Urho3D::QUAL_CTRL);
				if (selectedComponent != NULL)
				{
					if (input->GetQualifierDown(Urho3D::QUAL_SHIFT))
					{
						// If we are selecting components, but have nodes in existing selection, do not multiselect to prevent confusion
						if (!editorSelection_->GetSelectedNodes().Empty())
							multiselect = false;
						SelectComponent(selectedComponent, multiselect);
					}
					else
					{
						// If we are selecting nodes, but have components in existing selection, do not multiselect to prevent confusion
						if (!editorSelection_->GetSelectedComponents().Empty())
							multiselect = false;
						SelectNode(selectedComponent->GetNode(), multiselect);
					}
				}
				else
				{
					// If clicked on emptiness in non-multiselect mode, clear the selection
					if (!multiselect)
						SelectComponent(NULL, false);
				}
			}

		}

		void EPScene3D::SelectComponent(Urho3D::Component* component, bool multiselect)
		{
			if (component == NULL && !multiselect)
			{
				editor_->GetHierarchyWindow()->GetHierarchyList()->ClearSelection();
				return;
			}

			Urho3D::Node* node = component->GetNode();
			if (node == NULL && !multiselect)
			{
				editor_->GetHierarchyWindow()->GetHierarchyList()->ClearSelection();
				return;
			}
			Urho3D::ListView* hierarchyList = editor_->GetHierarchyWindow()->GetHierarchyList();

			unsigned int nodeIndex = editor_->GetHierarchyWindow()->GetListIndex(node);
			unsigned int componentIndex = editor_->GetHierarchyWindow()->GetComponentListIndex(component);
			unsigned int numItems = hierarchyList->GetNumItems();

			if (nodeIndex < numItems && componentIndex < numItems)
			{
				// Expand the node chain now
				if (!multiselect || !hierarchyList->IsSelected(componentIndex))
				{
					// Go in the parent chain up to make sure the chain is expanded
					Urho3D::Node* current = node;
					do
					{
						hierarchyList->Expand(editor_->GetHierarchyWindow()->GetListIndex(current), true);
						current = current->GetParent();
					} while (current != NULL);
				}

				// This causes an event to be sent, in response we set the node/component selections, and refresh editors
				if (!multiselect)
					hierarchyList->SetSelection( componentIndex);
				else
					hierarchyList->ToggleSelection(componentIndex);
			}
			else if (!multiselect)
				hierarchyList->ClearSelection();
		}

		void EPScene3D::SelectNode(Urho3D::Node* node, bool multiselect)
		{
			if (node == NULL && !multiselect)
			{
				editor_->GetHierarchyWindow()->GetHierarchyList()->ClearSelection();
				return;
			}
			Urho3D::ListView* hierarchyList = editor_->GetHierarchyWindow()->GetHierarchyList();
			unsigned int index = editor_->GetHierarchyWindow()->GetListIndex(node);
			unsigned int numItems = hierarchyList->GetNumItems();

			if (index < numItems)
			{
				// Expand the node chain now
				if (!multiselect || !hierarchyList->IsSelected(index))
				{
					// Go in the parent chain up to make sure the chain is expanded
					Urho3D::Node* current = node;
					do
					{
						hierarchyList->Expand(editor_->GetHierarchyWindow()->GetListIndex(current), true);
						current = current->GetParent();
					} while (current != NULL);
				}

				// This causes an event to be sent, in response we set the node/component selections, and refresh editors
				if (!multiselect)
					hierarchyList->SetSelection( index);
				else
					hierarchyList->ToggleSelection(index);
			}
			else if (!multiselect)
				hierarchyList->ClearSelection();
		}

		void EPScene3D::SetMouseMode(bool enable)
		{
			if (enable)
			{
				if (mouseOrbitMode == ORBIT_RELATIVE)
				{
					input_->SetMouseMode(Urho3D::MM_RELATIVE);
					ui_->GetCursor()->SetVisible(false);
				}
				else if (mouseOrbitMode == ORBIT_WRAP)
					input_->SetMouseMode(Urho3D::MM_WRAP);
			}
			else
			{
				input_->SetMouseMode(Urho3D::MM_ABSOLUTE);
				ui_->GetCursor()->SetVisible(true);
			}
		}

		void EPScene3D::SetMouseLock()
		{
			toggledMouseLock_ = true;
			SetMouseMode(true);
		}

		void EPScene3D::ReleaseMouseLock()
		{
			if (toggledMouseLock_)
			{
				toggledMouseLock_ = false;
				SetMouseMode(false);
			}
		}

		void EPScene3D::HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::PostRenderUpdate;

			Urho3D::Scene* scene = editorData_->GetEditorScene();

			Urho3D::DebugRenderer* debug = scene->GetComponent<Urho3D::DebugRenderer>();
			if (debug == NULL)
				return;

			// Visualize the currently selected nodes
			for (unsigned int i = 0; i < editorSelection_->GetNumSelectedNodes(); ++i)
				DrawNodeDebug(editorSelection_->GetSelectedNodes()[i], debug);

			// Visualize the currently selected components
			for (unsigned int i = 0; i < editorSelection_->GetNumSelectedComponents(); ++i)
				editorSelection_->GetSelectedComponents()[i]->DrawDebugGeometry(debug, false);

			// Visualize the currently selected UI-elements
			for (unsigned int i = 0; i < editorSelection_->GetNumSelectedUIElements(); ++i)
				ui_->DebugDraw(editorSelection_->GetSelectedUIElements()[i]);

			if (renderingDebug)
				renderer->DrawDebugGeometry(false);

			Urho3D::PhysicsWorld* physics = scene->GetComponent<Urho3D::PhysicsWorld>();
			if (physicsDebug && physics != NULL)
				physics->DrawDebugGeometry(true);

			Urho3D::Octree* octree = scene->GetComponent<Urho3D::Octree>();
			if (octreeDebug && octree != NULL)
				octree->DrawDebugGeometry(true);

			ViewRaycast(false);
		}

		void EPScene3D::ViewMouseClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::UIMouseClick;

			ViewRaycast(true);
		}

		void EPScene3D::ViewMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseMove;
		}

		void EPScene3D::ViewMouseClickEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::UIMouseClickEnd;
		}

		void EPScene3D::HandleBeginViewUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::BeginViewUpdate;
		}

		void EPScene3D::HandleEndViewUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::EndViewUpdate;
		}

		void EPScene3D::HandleBeginViewRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::BeginViewRender;
		}

		void EPScene3D::HandleEndViewRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::EndViewUpdate;
		}

		void EPScene3D::HandleResizeView(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (activeView)
				activeView->SetSize(window_->GetSize());
		}

		void EPScene3D::HandleMenuBarAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace UI::MenuBarAction;

			Urho3D::StringHash action = eventData[P_ACTION].GetStringHash();
			if (action == UI::A_NEWSCENE_VAR)
			{
				ResetScene();
			}
			else if (action == UI::A_OPENSCENE_VAR)
			{
				editor_->CreateFileSelector("Open scene", "Open", "Cancel", editorData_->uiScenePath, editorData_->uiSceneFilters, editorData_->uiSceneFilter);
				SubscribeToEvent(editor_->GetUIFileSelector(), Urho3D::E_FILESELECTED, HANDLER(EPScene3D, HandleOpenSceneFile));
			}
			else if (action == UI::A_SAVESCENE_VAR || action == UI::A_SAVESCENEAS_VAR)
			{
				editor_->CreateFileSelector("Save scene as", "Save", "Cancel", editorData_->uiScenePath, editorData_->uiSceneFilters, editorData_->uiSceneFilter);
				editor_->GetUIFileSelector()->SetFileName(GetFileNameAndExtension(editorData_->GetEditorScene()->GetFileName()));
				SubscribeToEvent(editor_->GetUIFileSelector(), Urho3D::E_FILESELECTED, HANDLER(EPScene3D, HandleSaveSceneFile));
			}
			else if (action == UI::A_LOADNODEASREP_VAR)
			{
				instantiateMode = Urho3D::REPLICATED;
				editor_->CreateFileSelector("Load node", "Load", "Cancel", editorData_->uiNodePath, editorData_->uiSceneFilters, editorData_->uiNodeFilter);
				SubscribeToEvent(editor_->GetUIFileSelector(), Urho3D::E_FILESELECTED, HANDLER(EPScene3D, HandleLoadNodeFile));
			}
			else if (action == UI::A_LOADNODEASLOCAL_VAR)
			{
				instantiateMode = Urho3D::LOCAL;
				editor_->CreateFileSelector("Load node", "Load", "Cancel", editorData_->uiNodePath, editorData_->uiSceneFilters, editorData_->uiNodeFilter);
				SubscribeToEvent(editor_->GetUIFileSelector(), Urho3D::E_FILESELECTED, HANDLER(EPScene3D, HandleLoadNodeFile));
			}
			else if (action == UI::A_SAVENODEAS_VAR)
			{
				if (editorSelection_->GetEditNode() != NULL && editorSelection_->GetEditNode() != editorData_->GetEditorScene())
				{
					editor_->CreateFileSelector("Save node", "Save", "Cancel", editorData_->uiNodePath, editorData_->uiSceneFilters, editorData_->uiNodeFilter);
					editor_->GetUIFileSelector()->SetFileName(GetFileNameAndExtension(instantiateFileName));
					SubscribeToEvent(editor_->GetUIFileSelector(), Urho3D::E_FILESELECTED, HANDLER(EPScene3D, HandleSaveNodeFile));
				}
			}
			else if (action == UI::A_CREATEREPNODE_VAR)
			{
				CreateNode(Urho3D::REPLICATED);
			}
			else if (action == UI::A_CREATELOCALNODE_VAR)
			{
				CreateNode(Urho3D::LOCAL);
			}
			else if (action == UI::A_CREATECOMPONENT_VAR)
			{
				Urho3D::String uiname = eventData[P_UINAME].GetString();
				CreateComponent(uiname);
			}
			else if (action == UI::A_CREATEBUILTINOBJ_VAR)
			{
				Urho3D::String uiname = eventData[P_UINAME].GetString();
				CreateBuiltinObject(uiname);
			}
		}

		void EPScene3D::HandleMessageAcknowledgement(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MessageACK;

			if (eventData[P_OK].GetBool())
			{
				sceneModified = false;
				ResetScene();
			}
		}

		bool EPScene3D::ResetScene()
		{
			ui_->GetCursor()->SetShape(Urho3D::CS_BUSY);

			if (sceneModified)
			{
				Urho3D::SharedPtr<Urho3D::MessageBox> messageBox(new Urho3D::MessageBox(context_, "Scene has been modified.\nContinue to reset?", "Warning"));
				messageBox->AddRef();
				if (messageBox->GetWindow() != NULL)
				{
					Urho3D::Button* cancelButton = (Urho3D::Button*)messageBox->GetWindow()->GetChild("CancelButton", true);
					cancelButton->SetVisible(true);
					cancelButton->SetFocus(true);
					SubscribeToEvent(messageBox, Urho3D::E_MESSAGEACK, HANDLER(EPScene3D, HandleMessageAcknowledgement));

					return false;
				}
			}

			// Clear stored script attributes
			//scriptAttributes.Clear();

			EditorState* editor = editorData_->GetEditor();
			editor->GetHierarchyWindow()->SetSuppressSceneChanges(true);

			// Create a scene with default values, these will be overridden when loading scenes
			editorData_->GetEditorScene()->Clear();
			editorData_->GetEditorScene()->CreateComponent<Urho3D::Octree>();
			editorData_->GetEditorScene()->CreateComponent<Urho3D::DebugRenderer>();

			// Release resources that became unused after the scene clear
			//cache.ReleaseAllResources(false);

			sceneModified = false;
			revertData = NULL;
			StopSceneUpdate();

			//		UpdateWindowTitle();
			//		DisableInspectorLock();
			editor->GetHierarchyWindow()->UpdateHierarchyItem(editorData_->GetEditorScene(), true);
			//		ClearEditActions();

			editor->GetHierarchyWindow()->SetSuppressSceneChanges(false);

			ResetCamera();
			//	CreateGizmo();
			CreateGrid();
			//	SetActiveViewport(viewports[0]);

			return true;
		}

		void EPScene3D::HandleOpenSceneFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			editor_->CloseFileSelector(editorData_->uiSceneFilter, editorData_->uiScenePath);
			LoadScene(UI::UIUtils::ExtractFileName(eventData));
		}

		void EPScene3D::HandleSaveSceneFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			editor_->CloseFileSelector(editorData_->uiSceneFilter, editorData_->uiScenePath);
			SaveScene(UI::UIUtils::ExtractFileName(eventData, true));
		}

		void EPScene3D::HandleLoadNodeFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			editor_->CloseFileSelector(editorData_->uiSceneFilter, editorData_->uiScenePath);
			LoadNode(UI::UIUtils::ExtractFileName(eventData));
		}

		void EPScene3D::HandleSaveNodeFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			editor_->CloseFileSelector(editorData_->uiSceneFilter, editorData_->uiScenePath);
			SaveNode(UI::UIUtils::ExtractFileName(eventData, true));
		}

		bool EPScene3D::LoadScene(const Urho3D::String& fileName)
		{
			if (fileName.Empty())
				return false;

			ui_->GetCursor()->SetShape(Urho3D::CS_BUSY);

			// Always load the scene from the filesystem, not from resource paths
			if (!fileSystem_->FileExists(fileName))
			{
				LOGERRORF("No such scene %s", fileName.CString());

				Urho3D::MessageBox(context_, "No such scene.\n" + fileName);
				return false;
			}

			Urho3D::File file(context_);
			if (!file.Open(fileName, Urho3D::FILE_READ))
			{
				LOGERRORF("Could not open file %s", fileName.CString());

				Urho3D::MessageBox(context_, "Could not open file.\n" + fileName);
				return false;
			}

			// Reset stored script attributes.
			// 	scriptAttributes.Clear();
			//
			// 	// Add the scene's resource path in case it's necessary
			// 	String newScenePath = GetPath(fileName);
			// 	if (!rememberResourcePath || !sceneResourcePath.StartsWith(newScenePath, false))
			// 		SetResourcePath(newScenePath);

			editor_->GetHierarchyWindow()->SetSuppressSceneChanges(true);
			sceneModified = false;
			revertData = NULL;
			StopSceneUpdate();

			Urho3D::String extension = GetExtension(fileName);
			bool loaded;
			if (extension != ".xml")
				loaded = editorData_->GetEditorScene()->Load(file);
			else
				loaded = editorData_->GetEditorScene()->LoadXML(file);

			// Release resources which are not used by the new scene
			/// \todo this creates an bug in the attribute inspector because the loaded xml files are released
			cache_->ReleaseAllResources(false);

			// Always pause the scene, and do updates manually
			editorData_->GetEditorScene()->SetUpdateEnabled(false);

			// 	UpdateWindowTitle();
			// 	DisableInspectorLock();
			editor_->GetHierarchyWindow()->UpdateHierarchyItem(editorData_->GetEditorScene(), true);
			// 	ClearEditActions();
			//

			editor_->GetHierarchyWindow()->SetSuppressSceneChanges(false);
			/// \todo
			editorSelection_->ClearSelection();
			editor_->GetAttributeWindow()->GetEditNodes() = editorSelection_->GetEditNodes();
			editor_->GetAttributeWindow()->GetEditComponents() = editorSelection_->GetEditComponents();
			editor_->GetAttributeWindow()->GetEditUIElements() = editorSelection_->GetEditUIElements();
			editor_->GetAttributeWindow()->Update();
			//
			// 	// global variable to mostly bypass adding mru upon importing tempscene
			// 	if (!skipMruScene)
			// 		UpdateSceneMru(fileName);
			//
			// 	skipMruScene = false;
			//
			ResetCamera();
			// 	CreateGizmo();
			CreateGrid();
			// 	SetActiveViewport(viewports[0]);
			//
			// 	// Store all ScriptInstance and LuaScriptInstance attributes
			// 	UpdateScriptInstances();

			return loaded;
		}

		bool EPScene3D::SaveScene(const Urho3D::String& fileName)
		{
			if (fileName.Empty())
				return false;

			ui_->GetCursor()->SetShape(Urho3D::CS_BUSY);

			// Unpause when saving so that the scene will work properly when loaded outside the editor
			editorData_->GetEditorScene()->SetUpdateEnabled(true);

			MakeBackup(fileName);
			Urho3D::File file(context_, fileName, Urho3D::FILE_WRITE);
			Urho3D::String extension = GetExtension(fileName);
			bool success = (extension != ".xml" ? editorData_->GetEditorScene()->Save(file) : editorData_->GetEditorScene()->SaveXML(file));
			RemoveBackup(success, fileName);

			editorData_->GetEditorScene()->SetUpdateEnabled(false);

			if (success)
			{
				//	UpdateSceneMru(fileName);
				sceneModified = false;
				//	UpdateWindowTitle();
			}
			else
				Urho3D::MessageBox(context_, "Could not save scene successfully!\nSee Urho3D.log for more detail.");

			return success;
		}

		Urho3D::Node* EPScene3D::LoadNode(const Urho3D::String& fileName, Urho3D::Node* parent /*= NULL*/)
		{
			if (fileName.Empty())
				return NULL;

			if (!fileSystem_->FileExists(fileName))
			{
				Urho3D::MessageBox(context_, "No such node file.\n" + fileName);
				return NULL;
			}

			Urho3D::File file(context_);
			if (!file.Open(fileName, Urho3D::FILE_READ))
			{
				Urho3D::MessageBox(context_, "Could not open file.\n" + fileName);
				return NULL;
			}

			ui_->GetCursor()->SetShape(Urho3D::CS_BUSY);

			// Before instantiating, add object's resource path if necessary
			//SetResourcePath(GetPath(fileName), true, true);

			Urho3D::Ray cameraRay = camera_->GetScreenRay(0.5, 0.5); // Get ray at view center
			Urho3D::Vector3 position, normal;
			//	GetSpawnPosition(cameraRay, newNodeDistance, position, normal, 0, true);

			Urho3D::Node* newNode = InstantiateNodeFromFile(&file, position, Urho3D::Quaternion(), 1, parent, instantiateMode);
			if (newNode != NULL)
			{
				//FocusNode(newNode);
				instantiateFileName = fileName;
			}
			return newNode;
		}

		bool EPScene3D::SaveNode(const Urho3D::String& fileName)
		{
			if (fileName.Empty())
				return false;

			ui_->GetCursor()->SetShape(Urho3D::CS_BUSY);

			MakeBackup(fileName);

			Urho3D::File file(context_);
			if (!file.Open(fileName, Urho3D::FILE_WRITE))
			{
				Urho3D::MessageBox(context_, "Could not open file.\n" + fileName);
				return NULL;
			}

			Urho3D::String extension = GetExtension(fileName);
			bool success = (extension != ".xml" ? editorSelection_->GetEditNode()->Save(file) : editorSelection_->GetEditNode()->SaveXML(file));
			RemoveBackup(success, fileName);

			if (success)
				instantiateFileName = fileName;
			else
				Urho3D::MessageBox(context_, "Could not save node successfully!\nSee Urho3D.log for more detail.");

			return success;
		}

		Urho3D::Node* EPScene3D::InstantiateNodeFromFile(Urho3D::File* file, const Urho3D::Vector3& position, const Urho3D::Quaternion& rotation, float scaleMod /*= 1.0f*/, Urho3D::Node* parent /*= NULL*/, Urho3D::CreateMode mode /*= REPLICATED*/)
		{
			if (file == NULL)
				return NULL;

			Urho3D::Node* newNode = NULL;
			unsigned int numSceneComponent = editorData_->GetEditorScene()->GetNumComponents();

			editor_->GetHierarchyWindow()->SetSuppressSceneChanges(true);

			Urho3D::String extension = GetExtension(file->GetName());
			if (extension != ".xml")
				newNode = editorData_->GetEditorScene()->Instantiate(*file, position, rotation, mode);
			else
				newNode = editorData_->GetEditorScene()->InstantiateXML(*file, position, rotation, mode);

			editor_->GetHierarchyWindow()->SetSuppressSceneChanges(false);

			if (parent != NULL)
				newNode->SetParent(parent);

			if (newNode != NULL)
			{
				newNode->SetScale(newNode->GetScale() * scaleMod);
				// 			if (alignToAABBBottom)
				// 			{
				// 				Drawable@ drawable = GetFirstDrawable(newNode);
				// 				if (drawable !is null)
				// 				{
				// 					BoundingBox aabb = drawable.worldBoundingBox;
				// 					Vector3 aabbBottomCenter(aabb.center.x, aabb.min.y, aabb.center.z);
				// 					Vector3 offset = aabbBottomCenter - newNode.worldPosition;
				// 					newNode.worldPosition = newNode.worldPosition - offset;
				// 				}
				// 			}

				// Create an undo action for the load
				// 			CreateNodeAction action;
				// 			action.Define(newNode);
				// 			SaveEditAction(action);
				// 			SetSceneModified();
				sceneModified = true;

				if (numSceneComponent != editorData_->GetEditorScene()->GetNumComponents())
					editor_->GetHierarchyWindow()->UpdateHierarchyItem(editorData_->GetEditorScene());
				else
					editor_->GetHierarchyWindow()->UpdateHierarchyItem(newNode);
			}

			return newNode;
		}

		Urho3D::Node* EPScene3D::CreateNode(Urho3D::CreateMode mode)
		{
			Urho3D::Node* newNode = NULL;
			if (editorSelection_->GetEditNode() != NULL)
				newNode = editorSelection_->GetEditNode()->CreateChild("", mode);
			else
				newNode = editorData_->GetEditorScene()->CreateChild("", mode);
			// Set the new node a certain distance from the camera
			//	newNode.position = GetNewNodePosition();

			// Create an undo action for the create
			// 		CreateNodeAction action;
			// 		action.Define(newNode);
			// 		SaveEditAction(action);
			// 		SetSceneModified();
			sceneModified = true;

			//		FocusNode(newNode);

			return newNode;
		}

		void EPScene3D::CreateComponent(const Urho3D::String& componentType)
		{
			// If this is the root node, do not allow to create duplicate scene-global components
			if (editorSelection_->GetEditNode() == editorData_->GetEditorScene() && CheckForExistingGlobalComponent(editorSelection_->GetEditNode(), componentType))
				return;

			// Group for storing undo actions
			//EditActionGroup group;

			// For now, make a local node's all components local
			/// \todo Allow to specify the createmode
			for (unsigned int i = 0; i < editorSelection_->GetNumEditNodes(); ++i)
			{
				Urho3D::Component* newComponent = editorSelection_->GetEditNodes()[i]->CreateComponent(componentType, editorSelection_->GetEditNodes()[i]->GetID() < Urho3D::FIRST_LOCAL_ID ? Urho3D::REPLICATED : Urho3D::LOCAL);
				if (newComponent != NULL)
				{
					// Some components such as CollisionShape do not create their internal object before the first call to ApplyAttributes()
					// to prevent unnecessary initialization with default values. Call now
					newComponent->ApplyAttributes();

					// 				CreateComponentAction action;
					// 				action.Define(newComponent);
					// 				group.actions.Push(action);
				}
			}

			// 		SaveEditActionGroup(group);
			// 		SetSceneModified();
			sceneModified = true;

			// Although the edit nodes selection are not changed, call to ensure attribute inspector notices new components of the edit nodes
			//	HandleHierarchyListSelectionChange();

			editor_->GetAttributeWindow()->Update();
		}

		void EPScene3D::CreateBuiltinObject(const Urho3D::String& name)
		{
			Urho3D::Node* newNode = editorData_->GetEditorScene()->CreateChild(name, Urho3D::REPLICATED);
			// Set the new node a certain distance from the camera
			//	newNode.position = GetNewNodePosition();

			Urho3D::StaticModel* object = newNode->CreateComponent<Urho3D::StaticModel>();

			object->SetModel(cache_->GetResource<Urho3D::Model>("Models/" + name + ".mdl"));

			// Create an undo action for the create
			// 		CreateNodeAction action;
			// 		action.Define(newNode);
			// 		SaveEditAction(action);
			// 		SetSceneModified();

			sceneModified = true;

			//		FocusNode(newNode);
		}

		bool EPScene3D::CheckForExistingGlobalComponent(Urho3D::Node* node, const Urho3D::String& typeName)
		{
			if (typeName != "Octree" && typeName != "PhysicsWorld" && typeName != "DebugRenderer")
				return false;
			else
				return node->HasComponent(typeName);
		}

		void EPScene3D::MiniToolBarCreateLocalNode(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			CreateNode(Urho3D::LOCAL);
		}

		void EPScene3D::MiniToolBarCreateReplNode(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			CreateNode(Urho3D::REPLICATED);
		}

		void EPScene3D::MiniToolBarCreateComponent(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			Urho3D::Button* b = dynamic_cast<Urho3D::Button*>(GetEventSender());
			if (b)
				CreateComponent(b->GetName());
		}

		void EPScene3D::UpdateToolBar()
		{
			/// \todo 

			UI::ToolBarUI* toolBar = editorView_->GetToolBar();

			Urho3D::CheckBox* checkbox = (Urho3D::CheckBox*)toolBar->GetChild("EditMove", true);
			if (checkbox->IsChecked() != (editMode == EDIT_MOVE))
				checkbox->SetChecked(editMode == EDIT_MOVE);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("EditRotate", true);
			if (checkbox->IsChecked() != (editMode == EDIT_ROTATE))
				checkbox->SetChecked(editMode == EDIT_ROTATE);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("EditScale", true);
			if (checkbox->IsChecked() != (editMode == EDIT_SCALE))
				checkbox->SetChecked(editMode == EDIT_SCALE);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("EditSelect", true);
			if (checkbox->IsChecked() != (editMode == EDIT_SELECT))
				checkbox->SetChecked(editMode == EDIT_SELECT);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("AxisWorld", true);
			if (checkbox->IsChecked() != (axisMode == AXIS_WORLD))
				checkbox->SetChecked(axisMode == AXIS_WORLD);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("AxisLocal", true);
			if (checkbox->IsChecked() != (axisMode == AXIS_LOCAL))
				checkbox->SetChecked(axisMode == AXIS_LOCAL);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("MoveSnap", true);
			if (checkbox->IsChecked() != moveSnap)
				checkbox->SetChecked(moveSnap);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("RotateSnap", true);
			if (checkbox->IsChecked() != rotateSnap)
				checkbox->SetChecked(rotateSnap);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("ScaleSnap", true);
			if (checkbox->IsChecked() != scaleSnap)
				checkbox->SetChecked(scaleSnap);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("SnapScaleHalf", true);
			if (checkbox->IsChecked() != (snapScaleMode == SNAP_SCALE_HALF))
				checkbox->SetChecked(snapScaleMode == SNAP_SCALE_HALF);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("SnapScaleQuarter", true);
			if (checkbox->IsChecked() != (snapScaleMode == SNAP_SCALE_QUARTER))
				checkbox->SetChecked(snapScaleMode == SNAP_SCALE_QUARTER);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("PickGeometries", true);
			if (checkbox->IsChecked() != (pickMode == UI::PICK_GEOMETRIES))
				checkbox->SetChecked(pickMode == UI::PICK_GEOMETRIES);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("PickLights", true);
			if (checkbox->IsChecked() != (pickMode == UI::PICK_LIGHTS))
				checkbox->SetChecked(pickMode == UI::PICK_LIGHTS);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("PickZones", true);
			if (checkbox->IsChecked() != (pickMode == UI::PICK_ZONES))
				checkbox->SetChecked(pickMode == UI::PICK_ZONES);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("PickRigidBodies", true);
			if (checkbox->IsChecked() != (pickMode == UI::PICK_RIGIDBODIES))
				checkbox->SetChecked(pickMode == UI::PICK_RIGIDBODIES);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("PickUIElements", true);
			if (checkbox->IsChecked() != (pickMode == UI::PICK_UI_ELEMENTS))
				checkbox->SetChecked(pickMode == UI::PICK_UI_ELEMENTS);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("FillPoint", true);
			if (checkbox->IsChecked() != (fillMode == Urho3D::FILL_POINT))
				checkbox->SetChecked(fillMode == Urho3D::FILL_POINT);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("FillWireFrame", true);
			if (checkbox->IsChecked() != (fillMode == Urho3D::FILL_WIREFRAME))
				checkbox->SetChecked(fillMode == Urho3D::FILL_WIREFRAME);

			checkbox = (Urho3D::CheckBox*)toolBar->GetChild("FillSolid", true);
			if (checkbox->IsChecked() != (fillMode == Urho3D::FILL_SOLID))
				checkbox->SetChecked(fillMode == Urho3D::FILL_SOLID);

			toolBarDirty = false;
		}

		void EPScene3D::ToolBarEditModeMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				editMode = EDIT_MOVE;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarEditModeRotate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				editMode = EDIT_ROTATE;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarEditModeScale(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				editMode = EDIT_SCALE;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarEditModeSelect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				editMode = EDIT_SELECT;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarAxisModeWorld(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				axisMode = AXIS_WORLD;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarAxisModeLocal(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				axisMode = AXIS_LOCAL;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarMoveSnap(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			moveSnap = edit->IsChecked();
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarRotateSnap(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			rotateSnap = edit->IsChecked();
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarScaleSnap(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			scaleSnap = edit->IsChecked();
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarSnapScaleModeHalf(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
			{
				snapScaleMode = SNAP_SCALE_HALF;
				snapScale = 0.5;
			}
			else if (snapScaleMode == SNAP_SCALE_HALF)
			{
				snapScaleMode = SNAP_SCALE_FULL;
				snapScale = 1.0;
			}
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarSnapScaleModeQuarter(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
			{
				snapScaleMode = SNAP_SCALE_QUARTER;
				snapScale = 0.25;
			}
			else if (snapScaleMode == SNAP_SCALE_QUARTER)
			{
				snapScaleMode = SNAP_SCALE_FULL;
				snapScale = 1.0;
			}
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarPickModeGeometries(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				pickMode = UI::PICK_GEOMETRIES;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarPickModeLights(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				pickMode = UI::PICK_LIGHTS;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarPickModeZones(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				pickMode = UI::PICK_ZONES;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarPickModeRigidBodies(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				pickMode = UI::PICK_RIGIDBODIES;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarPickModeUIElements(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
				pickMode = UI::PICK_UI_ELEMENTS;
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarFillModePoint(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
			{
				fillMode = Urho3D::FILL_POINT;
				SetFillMode(fillMode);
			}
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarFillModeWireFrame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
			{
				fillMode = Urho3D::FILL_WIREFRAME;
				SetFillMode(fillMode);
			}
			toolBarDirty = true;
		}

		void EPScene3D::ToolBarFillModeSolid(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* edit = (Urho3D::CheckBox*)eventData[P_ELEMENT].GetPtr();
			if (edit && edit->IsChecked())
			{
				fillMode = Urho3D::FILL_SOLID;
				SetFillMode(fillMode);
			}
			toolBarDirty = true;
		}

		void EPScene3D::MakeBackup(const Urho3D::String& fileName)
		{
			fileSystem_->Rename(fileName, fileName + ".old");
		}

		void EPScene3D::RemoveBackup(bool success, const Urho3D::String& fileName)
		{
			if (success)
				fileSystem_->Delete(fileName + ".old");
		}

		void EPScene3D::HideGrid()
		{
			if (grid_ != NULL)
				grid_->SetEnabled(false);
		}

		void EPScene3D::ShowGrid()
		{
			if (grid_ != NULL)
			{
				grid_->SetEnabled(true);

				EditorData* editorData_ = GetSubsystem<EditorData>();
				if (editorData_->GetEditorScene()->GetComponent<Urho3D::Octree>() != NULL)
					editorData_->GetEditorScene()->GetComponent<Urho3D::Octree>()->AddManualDrawable(grid_);
			}
		}

		void EPScene3D::StartSceneUpdate()
		{
			runUpdate = true;
			// Run audio playback only when scene is updating, so that audio components' time-dependent attributes stay constant when
			// paused (similar to physics)
			//audio.Play();
			toolBarDirty = true;

			// Save scene data for reverting if enabled
			if (revertOnPause)
			{
				revertData = new Urho3D::XMLFile(context_);
				Urho3D::XMLElement root = revertData->CreateRoot("scene");
				editorData_->GetEditorScene()->SaveXML(root);
			}
			else
				revertData = NULL;
		}

		void EPScene3D::StopSceneUpdate()
		{
			runUpdate = false;
			//audio.Stop();
			toolBarDirty = true;

			// If scene should revert on update stop, load saved data now
			if (revertOnPause && revertData.NotNull())
			{
				editor_->GetHierarchyWindow()->SetSuppressSceneChanges(true);
		
				editorData_->GetEditorScene()->Clear();
				editorData_->GetEditorScene()->LoadXML(revertData->GetRoot());
				CreateGrid();
				editor_->GetHierarchyWindow()->UpdateHierarchyItem(editorData_->GetEditorScene(), true);
				//ClearEditActions();
				editor_->GetHierarchyWindow()->SetSuppressSceneChanges(false);
			}

			revertData = NULL;
		}

		void EPScene3D::CreateGrid()
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			if (!gridNode_)
			{
				gridNode_ = new Urho3D::Node(context_);
				grid_ = gridNode_->CreateComponent<Urho3D::CustomGeometry>();
				grid_->SetNumGeometries(1);
				grid_->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/VColUnlit.xml"));
				grid_->SetViewMask(0x80000000); // Editor raycasts use viewmask 0x7fffffff
				grid_->SetOccludee(false);
			}
			UpdateGrid();
		}

		void EPScene3D::UpdateGrid(bool updateGridGeometry /*= true*/)
		{
			showGrid_ ? ShowGrid() : HideGrid();
			gridNode_->SetScale(Urho3D::Vector3(8.0f, 8.0f, 8.0f));

			if (!updateGridGeometry)
				return;

			unsigned int size = (unsigned int)(floor(8.0f / 2.0f) * 2.0f);
			float halfSizeScaled = size / 2.0f;
			float scale = 1.0f;
			unsigned int subdivisionSize = (unsigned int)(pow(2.0f, 3.0f));

			if (subdivisionSize > 0)
			{
				size *= subdivisionSize;
				scale /= subdivisionSize;
			}

			unsigned int halfSize = size / 2;

			grid_->BeginGeometry(0, Urho3D::LINE_LIST);
			float lineOffset = -halfSizeScaled;
			for (unsigned int i = 0; i <= size; ++i)
			{
				bool lineCenter = i == halfSize;
				bool lineSubdiv = !Urho3D::Equals(float(i% subdivisionSize), 0.0f);

				if (!grid2DMode_)
				{
					grid_->DefineVertex(Urho3D::Vector3(lineOffset, 0.0, halfSizeScaled));
					grid_->DefineColor(lineCenter ? gridZColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
					grid_->DefineVertex(Urho3D::Vector3(lineOffset, 0.0, -halfSizeScaled));
					grid_->DefineColor(lineCenter ? gridZColor : (lineSubdiv ? gridSubdivisionColor : gridColor));

					grid_->DefineVertex(Urho3D::Vector3(-halfSizeScaled, 0.0, lineOffset));
					grid_->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
					grid_->DefineVertex(Urho3D::Vector3(halfSizeScaled, 0.0, lineOffset));
					grid_->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
				}
				else
				{
					grid_->DefineVertex(Urho3D::Vector3(lineOffset, halfSizeScaled, 0.0));
					grid_->DefineColor(lineCenter ? gridYColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
					grid_->DefineVertex(Urho3D::Vector3(lineOffset, -halfSizeScaled, 0.0));
					grid_->DefineColor(lineCenter ? gridYColor : (lineSubdiv ? gridSubdivisionColor : gridColor));

					grid_->DefineVertex(Urho3D::Vector3(-halfSizeScaled, lineOffset, 0.0));
					grid_->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
					grid_->DefineVertex(Urho3D::Vector3(halfSizeScaled, lineOffset, 0.0));
					grid_->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
				}

				lineOffset += scale;
			}
			grid_->Commit();
		}

		EPScene3DView::EPScene3DView(Urho3D::Context* context) : BorderImage(context),
			ownScene_(true),
			rttFormat_(Urho3D::Graphics::GetRGBFormat()),
			autoUpdate_(true),
			cameraYaw_(0.0f),
			cameraPitch_(0.0f)
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
			camera_->SetViewMask(0xffffffff); // It's easier to only have 1 gizmo active this viewport is shared with the gizmo
		}

		EPScene3DView::~EPScene3DView()
		{
			ResetScene();
		}

		void EPScene3DView::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<EPScene3DView>();
		}

		void EPScene3DView::OnResize()
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

		void EPScene3DView::OnHover(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int buttons, int qualifiers, Urho3D::Cursor* cursor)
		{
			UIElement::OnHover(position, screenPosition, buttons, qualifiers, cursor);
		}

		void EPScene3DView::OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor)
		{
			UIElement::OnClickBegin(position, screenPosition, button, buttons, qualifiers, cursor);
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();
			ui->SetFocusElement(NULL);
		}

		void EPScene3DView::OnClickEnd(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor, Urho3D::UIElement* beginElement)
		{
			UIElement::OnClickEnd(position, screenPosition, button, buttons, qualifiers, cursor, beginElement);
		}

		void EPScene3DView::SetView(Urho3D::Scene* scene, bool ownScene /*= true*/)
		{
			ResetScene();
			ResetCamera();

			scene_ = scene;
			ownScene_ = ownScene;

			viewport_->SetScene(scene_);
			viewport_->SetCamera(camera_);
			QueueUpdate();
		}

		void EPScene3DView::SetFormat(unsigned format)
		{
			if (format != rttFormat_)
			{
				rttFormat_ = format;
				OnResize();
			}
		}

		void EPScene3DView::SetAutoUpdate(bool enable)
		{
			if (enable != autoUpdate_)
			{
				autoUpdate_ = enable;
				Urho3D::RenderSurface* surface = renderTexture_->GetRenderSurface();
				if (surface)
					surface->SetUpdateMode(autoUpdate_ ? Urho3D::SURFACE_UPDATEALWAYS : Urho3D::SURFACE_MANUALUPDATE);
			}
		}

		void EPScene3DView::QueueUpdate()
		{
			if (!autoUpdate_)
			{
				Urho3D::RenderSurface* surface = renderTexture_->GetRenderSurface();
				if (surface)
					surface->QueueUpdate();
			}
		}

		Urho3D::Scene* EPScene3DView::GetScene() const
		{
			return scene_;
		}

		Urho3D::Node* EPScene3DView::GetCameraNode() const
		{
			return cameraNode_;
		}

		void EPScene3DView::ResetCamera()
		{
			cameraNode_->SetPosition(Urho3D::Vector3(0.0f, 5.0f, -10.0f));
			// Look at the origin so user can see the scene.
			cameraNode_->SetRotation(Urho3D::Quaternion(Urho3D::Vector3(0.0f, 0.0f, 1.0f), -cameraNode_->GetPosition()));
			ReacquireCameraYawPitch();
			//	UpdateSettingsUI();
		}

		void EPScene3DView::ResetCamera(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			ResetCamera();
		}

		void EPScene3DView::CloseViewportSettingsWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			settingsWindow->SetVisible(false);
		}

		void EPScene3DView::UpdateSettingsUI(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			cameraPosX->SetText(Urho3D::String(cameraNode_->GetPosition().x_));
			cameraPosY->SetText(Urho3D::String(cameraNode_->GetPosition().y_));
			cameraPosZ->SetText(Urho3D::String(cameraNode_->GetPosition().z_));
			cameraRotX->SetText(Urho3D::String(cameraNode_->GetRotation().PitchAngle()));
			cameraRotY->SetText(Urho3D::String(cameraNode_->GetRotation().YawAngle()));
			cameraRotZ->SetText(Urho3D::String(cameraNode_->GetRotation().RollAngle()));
			cameraZoom->SetText(Urho3D::String(camera_->GetZoom()));
			cameraOrthoSize->SetText(Urho3D::String(camera_->GetOrthoSize()));
			cameraOrthographic->SetChecked(camera_->IsOrthographic());
		}

		void EPScene3DView::OpenViewportSettingsWindow()
		{
			Urho3D::VariantMap emptyMap;
			UpdateSettingsUI(Urho3D::StringHash::ZERO, emptyMap);

			settingsWindow->SetVisible(true);
			settingsWindow->BringToFront();
		}

		void EPScene3DView::ReacquireCameraYawPitch()
		{
			cameraYaw_ = cameraNode_->GetRotation().YawAngle();
			cameraPitch_ = cameraNode_->GetRotation().PitchAngle();
		}

		void EPScene3DView::CreateViewportContextUI(Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_)
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

			settingsWindow = Urho3D::DynamicCast<Urho3D::Window>(ui->LoadLayout(cache->GetResource<Urho3D::XMLFile>("UI/EditorViewport.xml")));
			//settingsWindow->SetOpacity(editor_->GetuiMaxOpacity());
			settingsWindow->SetVisible(false);
			AddChild(settingsWindow);

			cameraPosX = (Urho3D::LineEdit*)settingsWindow->GetChild("PositionX", true);
			cameraPosY = (Urho3D::LineEdit*)settingsWindow->GetChild("PositionY", true);
			cameraPosZ = (Urho3D::LineEdit*)settingsWindow->GetChild("PositionZ", true);
			cameraRotX = (Urho3D::LineEdit*)settingsWindow->GetChild("RotationX", true);
			cameraRotY = (Urho3D::LineEdit*)settingsWindow->GetChild("RotationY", true);
			cameraRotZ = (Urho3D::LineEdit*)settingsWindow->GetChild("RotationZ", true);
			cameraOrthographic = (Urho3D::CheckBox*)settingsWindow->GetChild("Orthographic", true);
			cameraZoom = (Urho3D::LineEdit*)settingsWindow->GetChild("Zoom", true);
			cameraOrthoSize = (Urho3D::LineEdit*)settingsWindow->GetChild("OrthoSize", true);

			SubscribeToEvent(cameraPosX, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraPosY, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraPosZ, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraRotX, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraRotY, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraRotZ, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraZoom, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraOrthoSize, Urho3D::E_TEXTCHANGED, HANDLER(EPScene3DView, HandleSettingsLineEditTextChange));
			SubscribeToEvent(cameraOrthographic, Urho3D::E_TOGGLED, HANDLER(EPScene3DView, HandleOrthographicToggled));

			SubscribeToEvent(settingsButton, Urho3D::E_RELEASED, HANDLER(EPScene3DView, ToggleViewportSettingsWindow));
			SubscribeToEvent(settingsWindow->GetChild("ResetCamera", true), Urho3D::E_RELEASED, HANDLER(EPScene3DView, ResetCamera));
			SubscribeToEvent(settingsWindow->GetChild("CloseButton", true), Urho3D::E_RELEASED, HANDLER(EPScene3DView, CloseViewportSettingsWindow));
			SubscribeToEvent(settingsWindow->GetChild("Refresh", true), Urho3D::E_RELEASED, HANDLER(EPScene3DView, UpdateSettingsUI));
			HandleResize();
		}

		Urho3D::Texture2D* EPScene3DView::GetRenderTexture() const
		{
			return renderTexture_;
		}

		Urho3D::Texture2D* EPScene3DView::GetDepthTexture() const
		{
			return depthTexture_;
		}

		Urho3D::Viewport* EPScene3DView::GetViewport() const
		{
			return viewport_;
		}

		void EPScene3DView::ResetScene()
		{
			if (!scene_)
				return;

			if (!ownScene_)
			{
				Urho3D::RefCount* refCount = scene_->RefCountPtr();
				++refCount->refs_;
				scene_ = 0;
				--refCount->refs_;
			}
			else
				scene_ = 0;
		}

		void EPScene3DView::ToggleOrthographic()
		{
			SetOrthographic(!camera_->IsOrthographic());
		}

		void EPScene3DView::SetOrthographic(bool orthographic)
		{
			camera_->SetOrthographic(orthographic);
			Urho3D::VariantMap emptyMap;
			UpdateSettingsUI(Urho3D::StringHash::ZERO, emptyMap);
		}

		void EPScene3DView::HandleResize()
		{
			statusBar->SetLayoutBorder(Urho3D::IntRect(8, 4, 4, 8));
			Urho3D::IntVector2 pos = settingsWindow->GetPosition();
			pos.x_ = 5;
			settingsWindow->SetPosition(pos);

			statusBar->SetFixedSize(GetWidth(), 22);
		}

		void EPScene3DView::HandleSettingsLineEditTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::TextChanged;

			Urho3D::LineEdit* element = (Urho3D::LineEdit*)eventData[P_ELEMENT].GetPtr();
			if (!element || element->GetText().Empty())
				return;

			if (element == cameraRotX || element == cameraRotY || element == cameraRotZ)
			{
				Urho3D::Vector3 euler = cameraNode_->GetRotation().EulerAngles();
				if (element == cameraRotX)
					euler.x_ = ToFloat(element->GetText());
				else if (element == cameraRotY)
					euler.y_ = ToFloat(element->GetText());
				else if (element == cameraRotZ)
					euler.z_ = ToFloat(element->GetText());

				cameraNode_->SetRotation(Urho3D::Quaternion(euler.x_, euler.y_, euler.z_));
			}
			else if (element == cameraPosX || element == cameraPosY || element == cameraPosZ)
			{
				Urho3D::Vector3 pos = cameraNode_->GetPosition();
				if (element == cameraPosX)
					pos.x_ = ToFloat(element->GetText());
				else if (element == cameraPosY)
					pos.y_ = ToFloat(element->GetText());
				else if (element == cameraPosZ)
					pos.z_ = ToFloat(element->GetText());

				cameraNode_->SetPosition(pos);
			}
			else if (element == cameraZoom)
				camera_->SetZoom(ToFloat(element->GetText()));
			else if (element == cameraOrthoSize)
				camera_->SetOrthoSize(ToFloat(element->GetText()));
		}

		void EPScene3DView::HandleOrthographicToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			SetOrthographic(cameraOrthographic->IsChecked());
		}

		void EPScene3DView::ToggleViewportSettingsWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (settingsWindow->IsVisible())
			{
				Urho3D::VariantMap emptyMap;
				CloseViewportSettingsWindow(Urho3D::StringHash::ZERO, emptyMap);
			}
			else
				OpenViewportSettingsWindow();
		}

		void EPScene3DView::Update(float timeStep)
		{
			Urho3D::Vector3 cameraPos = cameraNode_->GetPosition();
			Urho3D::String xText(cameraPos.x_);
			Urho3D::String yText(cameraPos.y_);
			Urho3D::String zText(cameraPos.z_);
			xText.Resize(8);
			yText.Resize(8);
			zText.Resize(8);

			cameraPosText->SetText(Urho3D::String(
				"Pos: " + xText + " " + yText + " " + zText +
				" Zoom: " + Urho3D::String(camera_->GetZoom())));

			cameraPosText->SetSize(cameraPosText->GetMinSize());
		}
	}
}