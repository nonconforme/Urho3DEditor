#ifndef _EDITOR_EDITORPLUGIN_H
#define _EDITOR_EDITORPLUGIN_H

#pragma once

#include "../UI/Macros.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>

namespace Urho3D
{
	class UIElement;
}

namespace Prime
{
	namespace Editor
	{
		class InGameEditor;
		class EditorView;

		enum EditorFramePosition
		{
			EFP_LEFT,
			EFP_CENTER,
			EFP_RIGHT
		};

		class EditorPlugin : public Urho3D::Object
		{
			OBJECT(EditorPlugin);
			BASEOBJECT(EditorPlugin);
		public:
			/// Construct.
			EditorPlugin(Urho3D::Context* context, EditorFramePosition framePosition = EFP_CENTER);
			/// Destruct.
			virtual ~EditorPlugin();

			/// return false if this plugin is a subeditor, does not manipulate the frame tabs.
			virtual bool HasMainScreen() { return false; }
			/// called from the main editor, when this editor plugin is a subeditor (overlayed editor).
			virtual bool ForwardInputEvent(){ return false; }
			/// called from the main editor, when this editor plugin is a subeditor (overlayed editor).
			virtual bool ForwardSceneInputEvent(){ return false; }
			/// returns the name of this plugin, default return the type name.
			virtual Urho3D::String GetName() const { return GetTypeName(); }
			/// called if the an object is selected to be edited. 
			virtual void Edit(Object *object) {}
			/// check if the object can be edited by this plugin
			virtual bool Handles(Object *object) const { return false; }
			/// add UI elements to the Editor View. \todo  ....
			// 		virtual void AddUIToView(EditorView* view) { }
			/// returns ui element content for this plugin, that will be displayed in the middle frame tabs.
			virtual Urho3D::UIElement* GetMainScreen() { return NULL; }
			/// make the editor plugin visible or not °°. 
			virtual void SetVisible(bool visible) { visible_ = visible; }
			/// is this plugin active/visible.
			bool IsVisible() { return visible_; }
			/// update is called only for main plugins. 
			virtual void Update(float timeStep) {  }
			EditorFramePosition GetFramePosition() { return _framePosition; }
			/// \todo handle changes that a pending (marked as *), external data, save/load editor state ??

		protected:
			bool visible_;

			EditorFramePosition _framePosition;
		};
	}
}

#endif