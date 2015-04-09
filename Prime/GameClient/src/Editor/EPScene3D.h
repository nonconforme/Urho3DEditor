#ifndef _EDITOR_PLUGINS_EPSCENE3D_H
#define _EDITOR_PLUGINS_EPSCENE3D_H

#pragma once

#include "EditorPlugin.h"
#include "../UI/UIGlobals.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Scene/Node.h>

namespace Urho3D
{
	class Window;
	class View3D;
	class Camera;
	class Node;
	class Scene;
	class CustomGeometry;
	class Texture2D;
	class Viewport;
	class DebugRenderer;
	class UI;
	class Input;
	class SoundListener;
	class Renderer;
	class ResourceCache;
	class Text;
	class Font;
	class LineEdit;
	class CheckBox;
	class XMLFile;
	class Menu;
	class FileSelector;
	class FileSystem;
	class File;
	class Button;
}
	
namespace Prime
{
	namespace Editor
	{
		enum MouseOrbitMode
		{
			ORBIT_RELATIVE = 0,
			ORBIT_WRAP
		};

		enum EditMode
		{
			EDIT_MOVE = 0,
			EDIT_ROTATE,
			EDIT_SCALE,
			EDIT_SELECT,
			EDIT_SPAWN
		};

		enum AxisMode
		{
			AXIS_WORLD = 0,
			AXIS_LOCAL
		};

		enum SnapScaleMode
		{
			SNAP_SCALE_FULL = 0,
			SNAP_SCALE_HALF,
			SNAP_SCALE_QUARTER
		};
		
		class EditorState;
		class EditorData;
		class EditorView;
		class EditorSelection;
		class EPScene3D;
		class GizmoScene3D;

		class EPScene3DView : public Urho3D::BorderImage
		{
			OBJECT(EPScene3DView);
			friend class EPScene3D;
		public:
			/// Construct.
			EPScene3DView(Urho3D::Context* context);
			/// Destruct.
			virtual ~EPScene3DView();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			virtual void Update(float timeStep) override;
			/// React to resize.
			virtual void OnResize() override;
			/// React to mouse hover.
			virtual void OnHover(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int buttons, int qualifiers, Urho3D::Cursor* cursor) override;
			/// React to mouse click begin.
			virtual void OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor) override;
			/// React to mouse click end.
			virtual void OnClickEnd(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor, Urho3D::UIElement* beginElement) override;

			/// Define the scene and camera to use in rendering. When ownScene is true the View3D will take ownership of them with shared pointers.
			void SetView(Urho3D::Scene* scene, bool ownScene = true);
			/// Set render texture pixel format. Default is RGB.
			void SetFormat(unsigned format);
			/// Set render target auto update mode. Default is true.
			void SetAutoUpdate(bool enable);
			/// Queue manual update on the render texture.
			void QueueUpdate();

			/// Return render texture pixel format.
			unsigned GetFormat() const { return rttFormat_; }
			/// Return whether render target updates automatically.
			bool GetAutoUpdate() const { return autoUpdate_; }
			/// Return scene.
			Urho3D::Scene* GetScene() const;
			/// Return camera scene node.
			Urho3D::Node* GetCameraNode() const;
			/// Return render texture.
			Urho3D::Texture2D* GetRenderTexture() const;
			/// Return depth stencil texture.
			Urho3D::Texture2D* GetDepthTexture() const;
			/// Return viewport.
			Urho3D::Viewport* GetViewport() const;

			Urho3D::Camera* GetCamera() const { return camera_; }
			float	GetYaw() const { return cameraYaw_; }
			float	GetPitch() const { return cameraPitch_; }

			void ResetCamera();
			void ReacquireCameraYawPitch();
			void CreateViewportContextUI(Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_);
		protected:
			/// Reset scene.
			void ResetScene();

			void ToggleOrthographic();

			void SetOrthographic(bool orthographic);

			void HandleResize();

			void HandleSettingsLineEditTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleOrthographicToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToggleViewportSettingsWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ResetCamera(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void CloseViewportSettingsWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void UpdateSettingsUI(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void OpenViewportSettingsWindow();

			/// Renderable texture.
			Urho3D::SharedPtr<Urho3D::Texture2D> renderTexture_;
			/// Depth stencil texture.
			Urho3D::SharedPtr<Urho3D::Texture2D> depthTexture_;
			/// Viewport.
			Urho3D::SharedPtr<Urho3D::Viewport> viewport_;
			/// Scene.
			Urho3D::SharedPtr<Urho3D::Scene> scene_;
			/// Camera scene node.
			Urho3D::SharedPtr<Urho3D::Node>		cameraNode_;
			/// Camera
			Urho3D::SharedPtr<Urho3D::Camera>	camera_;
			/// SoundListener
			Urho3D::SharedPtr<Urho3D::SoundListener> soundListener_;
			///
			float cameraYaw_;
			///
			float cameraPitch_;
			/// Own scene.
			bool ownScene_;
			/// Render texture format.
			unsigned rttFormat_;
			/// Render texture auto update mode.
			bool autoUpdate_;
			/// ui stuff
			Urho3D::SharedPtr<Urho3D::UIElement>statusBar;
			Urho3D::SharedPtr<Urho3D::Text> cameraPosText;

			Urho3D::SharedPtr<Urho3D::Window> settingsWindow;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraPosX;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraPosY;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraPosZ;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraRotX;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraRotY;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraRotZ;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraZoom;
			Urho3D::SharedPtr<Urho3D::LineEdit> cameraOrthoSize;
			Urho3D::SharedPtr<Urho3D::CheckBox> cameraOrthographic;
		};

		class EPScene3D : public EditorPlugin
		{
			OBJECT(EPScene3D);
			friend class GizmoScene3D;
		public:
			/// Construct.
			EPScene3D(Urho3D::Context* context);
			/// Destruct.
			virtual ~EPScene3D();


			virtual bool	HasMainScreen() override;
			virtual Urho3D::String	GetName() const override;
			virtual void	Edit(Object *object) override;
			virtual bool	Handles(Object *object) const override;
			/// calls Start, because EPScene3D is a main Editor plugin
			///	GetMainScreen will be called in AddEditorPlugin() once, so use it as Start().
			virtual Urho3D::UIElement*	GetMainScreen() override;
			virtual void		SetVisible(bool visible) override;
			virtual void		Update(float timeStep) override;

			bool IsSceneModified() { return sceneModified; }

			// debug handling
			void ToggleRenderingDebug()	{ renderingDebug = !renderingDebug; }
			void TogglePhysicsDebug(){ physicsDebug = !physicsDebug; }
			void ToggleOctreeDebug(){ octreeDebug = !octreeDebug; }
			// camera handling
			void ResetCamera();
			void ReacquireCameraYawPitch();
			void UpdateViewParameters();
			// grid
			void HideGrid();
			void ShowGrid();
			// scene update handling
			void StartSceneUpdate();
			void StopSceneUpdate();
		protected:
			void Start();
			void CreateMiniToolBarUI();
			void CreateToolBarUI();

			void CreateStatsBar();
			void SetupStatsBarText(Urho3D::Text* text, Urho3D::Font* font, int x, int y, Urho3D::HorizontalAlignment hAlign, Urho3D::VerticalAlignment vAlign);
			void UpdateStats(float timeStep);
			void SetFillMode(Urho3D::FillMode fM_);

			Urho3D::Vector3 SelectedNodesCenterPoint();
			void	DrawNodeDebug(Urho3D::Node* node, Urho3D::DebugRenderer* debug, bool drawNode = true);
			void	MakeBackup(const Urho3D::String& fileName);
			void	RemoveBackup(bool success, const Urho3D::String& fileName);

			/// edit nodes
			bool MoveNodes(Urho3D::Vector3 adjust);
			bool RotateNodes(Urho3D::Vector3 adjust);
			bool ScaleNodes(Urho3D::Vector3 adjust);

			/// Picking
			void ViewRaycast(bool mouseClick);
			void SelectComponent(Urho3D::Component* component, bool multiselect);
			void SelectNode(Urho3D::Node* node, bool multiselect);

			/// mouse handling
			void SetMouseMode(bool enable);
			void SetMouseLock();
			void ReleaseMouseLock();

			/// Engine Events Handling
			void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ViewMouseClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ViewMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ViewMouseClickEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleBeginViewUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleEndViewUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleBeginViewRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleEndViewRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Resize the view
			void HandleResizeView(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle Menu Bar Events
			void HandleMenuBarAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// messageBox
			void HandleMessageAcknowledgement(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			// Menu Bar actions
			/// create new scene, because we use only one scene reset it ...
			bool ResetScene();
			void HandleOpenSceneFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleSaveSceneFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleLoadNodeFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleSaveNodeFile(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			bool LoadScene(const Urho3D::String& fileName);
			bool SaveScene(const Urho3D::String& fileName);
			Urho3D::Node* LoadNode(const Urho3D::String& fileName, Urho3D::Node* parent = NULL);
			bool SaveNode(const Urho3D::String& fileName);
			Urho3D::Node* InstantiateNodeFromFile(Urho3D::File* file, const Urho3D::Vector3& position, const Urho3D::Quaternion& rotation, float scaleMod = 1.0f, Urho3D::Node* parent = NULL, Urho3D::CreateMode mode = Urho3D::REPLICATED);

			Urho3D::Node* CreateNode(Urho3D::CreateMode mode);
			void CreateComponent(const Urho3D::String& componentType);
			void CreateBuiltinObject(const Urho3D::String& name);
			bool CheckForExistingGlobalComponent(Urho3D::Node* node, const Urho3D::String& typeName);

			// Mini Tool Bar actions
			void MiniToolBarCreateLocalNode(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void MiniToolBarCreateReplNode(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void MiniToolBarCreateComponent(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			// Mini Tool Bar actions
			void UpdateToolBar();
			void ToolBarEditModeMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarEditModeRotate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarEditModeScale(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarEditModeSelect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarAxisModeWorld(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarAxisModeLocal(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarMoveSnap(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarRotateSnap(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarScaleSnap(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarSnapScaleModeHalf(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarSnapScaleModeQuarter(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarPickModeGeometries(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarPickModeLights(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarPickModeZones(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarPickModeRigidBodies(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarPickModeUIElements(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarFillModePoint(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarFillModeWireFrame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarFillModeSolid(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			Urho3D::SharedPtr<Urho3D::UIElement>		window_;
			Urho3D::SharedPtr<EPScene3DView>	activeView;
			Urho3D::SharedPtr<Urho3D::Node>				cameraNode_;
			Urho3D::SharedPtr<Urho3D::Camera>			camera_;

			/// cache editor subsystems
			EditorData*			editorData_;
			EditorView*			editorView_;
			EditorSelection*	editorSelection_;
			EditorState*				editor_;
			/// cache subsystems
			Urho3D::UI*				ui_;
			Urho3D::Input*			input_;
			Urho3D::Renderer*		renderer;
			Urho3D::ResourceCache*	cache_;
			Urho3D::FileSystem*		fileSystem_;

			///  properties \todo make an input edit state system or ...
			/// mouse handling
			bool	toggledMouseLock_;
			int		mouseOrbitMode;
			/// scene update handling
			bool	runUpdate = false;
			bool    revertOnPause = true;
			Urho3D::SharedPtr<Urho3D::XMLFile> revertData;
			///camera handling
			float	cameraBaseSpeed = 10.0f;
			float	cameraBaseRotationSpeed = 0.2f;
			float	cameraShiftSpeedMultiplier = 5.0f;
			bool	mouseWheelCameraPosition = false;
			bool	orbiting = false;
			bool	limitRotation = false;
			float	viewNearClip = 0.1f;
			float	viewFarClip = 1000.0f;
			float	viewFov = 45.0f;
			/// create node
			float	newNodeDistance = 20.0f;
			/// edit input states
			float	moveStep = 0.5f;
			float	rotateStep = 5.0f;
			float	scaleStep = 0.1f;
			float	snapScale = 1.0f;
			bool	moveSnap = false;
			bool	rotateSnap = false;
			bool	scaleSnap = false;
			/// debug handling
			bool	renderingDebug = false;
			bool	physicsDebug = false;
			bool	octreeDebug = false;
			/// mouse pick handling
			int		pickMode = Prime::UI::PICK_GEOMETRIES;
			/// modes
			EditMode editMode = EDIT_MOVE;
			AxisMode axisMode = AXIS_WORLD;
			Urho3D::FillMode fillMode = Urho3D::FILL_SOLID;
			SnapScaleMode snapScaleMode = SNAP_SCALE_FULL;
			/// scene handling
			bool sceneModified;
			Urho3D::String instantiateFileName;
			Urho3D::CreateMode instantiateMode;
			/// ui stuff
			Urho3D::SharedPtr<Urho3D::Text> editorModeText;
			Urho3D::SharedPtr<Urho3D::Text> renderStatsText;
			Urho3D::SharedPtr<Urho3D::Menu>	sceneMenu_;
			Urho3D::SharedPtr<Urho3D::Menu>	createMenu_;
			/// cached mini tool bar buttons, to set visibility
			Urho3D::Vector<Urho3D::Button*> miniToolBarButtons_;
			/// cached tool bar toggles, to set visibility
			Urho3D::Vector<Urho3D::UIElement*> toolBarToggles;
			bool toolBarDirty = true;

			/// gizmo
			Urho3D::SharedPtr<GizmoScene3D> gizmo_;

			//////////////////////////////////////////////////////////////////////////
			/// Grid handling \todo put it into a component or object ...

			void CreateGrid();
			void UpdateGrid(bool updateGridGeometry = true);

			Urho3D::SharedPtr<Urho3D::Node>				gridNode_;
			Urho3D::SharedPtr<Urho3D::CustomGeometry>	grid_;
			bool	showGrid_;
			bool	grid2DMode_;
			Urho3D::Color gridColor;
			Urho3D::Color gridSubdivisionColor;
			Urho3D::Color gridXColor;
			Urho3D::Color gridYColor;
			Urho3D::Color gridZColor;
		};
	}
}
#endif