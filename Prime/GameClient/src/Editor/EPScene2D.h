#ifndef _EDITOR_PLUGINS_EPSCENE2D_H
#define _EDITOR_PLUGINS_EPSCENE2D_H

#pragma once

#include "EditorPlugin.h"

#include <Urho3D/Urho3D.h>

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
		class EPScene2D;

		class EPScene2DEditor : public Urho3D::Object
		{
			OBJECT(EPScene2DEditor);
			friend class EPScene2D;
		public:
			/// Construct.
			EPScene2DEditor(Urho3D::Context* context);
			/// Destruct.
			virtual ~EPScene2DEditor();

			void Edit(Object *object);
			void Show();
			void Hide();

		protected:
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


			Urho3D::SharedPtr<Urho3D::Window> window_;

		};

		class EPScene2D : public EditorPlugin
		{
			OBJECT(EPScene2D);
		public:
			/// Construct.
			EPScene2D(Urho3D::Context* context);
			/// Destruct.
			virtual ~EPScene2D();

			virtual bool HasMainScreen() override;

			virtual Urho3D::String GetName() const override;

			virtual void Edit(Urho3D::Object *object) override;

			virtual bool Handles(Urho3D::Object *object) const override;

			virtual Urho3D::UIElement* GetMainScreen() override;

			virtual void SetVisible(bool visible) override;

		protected:
			Urho3D::SharedPtr<Urho3D::Window> window_;

		};
	}
}
#endif