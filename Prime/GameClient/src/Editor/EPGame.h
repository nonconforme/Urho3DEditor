#ifndef _EDITOR_PLUGINS_EPGAME_H
#define _EDITOR_PLUGINS_EPGAME_H

#pragma once

#include "EditorPlugin.h"
#include "EPScene3D.h"
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
		class EditorState;
		class EditorData;
		class EditorView;
		class EditorSelection;
		class EPGame;
		class GizmoScene3D;

		class EPGameView : public Urho3D::BorderImage
		{
			OBJECT(EPGameView);
			friend class EPGame;
		public:
			/// Construct.
			EPGameView(Urho3D::Context* context);
			/// Destruct.
			virtual ~EPGameView();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			//virtual void Update(float timeStep) override;
			/// React to resize.
			virtual void OnResize() override;
			/// React to mouse hover.
			virtual void OnHover(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int buttons, int qualifiers, Urho3D::Cursor* cursor) override;
			/// React to mouse click begin.
			virtual void OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor) override;
			/// React to mouse click end.
			virtual void OnClickEnd(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor, Urho3D::UIElement* beginElement) override;

			void StartSceneUpdate();
			void StopSceneUpdate();

			/// Define the scene and camera to use in rendering. When ownScene is true the View3D will take ownership of them with shared pointers.
			void SetView(Urho3D::Scene* scene);
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

			void ResetCamera();

			void CreateViewportContextUI(Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_);
		protected:
			/// Reset scene.
			void ResetScene();

			void HandleResize();

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
			/// Render texture format.
			unsigned rttFormat_;
			/// Render texture auto update mode.
			bool autoUpdate_;
			/// ui stuff
			Urho3D::SharedPtr<Urho3D::UIElement>statusBar;
			Urho3D::SharedPtr<Urho3D::Text> cameraPosText;
		};

		class EPGame : public EditorPlugin
		{
			OBJECT(EPGame);
			friend class GizmoScene3D;
		public:
			/// Construct.
			EPGame(Urho3D::Context* context);
			/// Destruct.
			virtual ~EPGame();

			virtual bool	HasMainScreen() override;
			virtual Urho3D::String	GetName() const override;
			virtual void	Edit(Object *object) override;
			virtual bool	Handles(Object *object) const override;
			/// calls Start, because EPScene3D is a main Editor plugin
			///	GetMainScreen will be called in AddEditorPlugin() once, so use it as Start().
			virtual Urho3D::UIElement*	GetMainScreen() override;
			virtual void		SetVisible(bool visible) override;
			virtual void		Update(float timeStep) override;

			// scene update handling
			void StartSceneUpdate();
			void StopSceneUpdate();
		protected:
			void Start();
			void CreateMiniToolBarUI();
			void CreateToolBarUI();

			/// mouse handling
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

			// Mini Tool Bar actions
			void UpdateToolBar();
			void ToolBarRunUpdatePlay(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarRunUpdatePause(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ToolBarRevertOnPause(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			Urho3D::SharedPtr<Urho3D::UIElement>		window_;
			Urho3D::SharedPtr<EPGameView>	activeView;
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
			/// scene update handling
			bool	runUpdate = false;

			///camera handling
			float	viewNearClip = 0.1f;
			float	viewFarClip = 1000.0f;
			float	viewFov = 45.0f;

			/// scene handling
			/// ui stuff
			Urho3D::SharedPtr<Urho3D::Menu>	_gameMenu;
			/// cached mini tool bar buttons, to set visibility
			Urho3D::Vector<Urho3D::Button*> miniToolBarButtons_;
			/// cached tool bar toggles, to set visibility
			Urho3D::Vector<Urho3D::UIElement*> toolBarToggles;
			bool toolBarDirty = true;

			/// gizmo
			//Urho3D::SharedPtr<GizmoScene3D> gizmo_;
		};
	}
}
#endif