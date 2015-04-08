#ifndef _EDITOR_PLUGINS_PLUGINSCENE3DEDITOR_H
#define _EDITOR_PLUGINS_PLUGINSCENE3DEDITOR_H

#pragma once

#include "EditorPlugin.h"

#include "../UI/Macros.h"

#include <Urho3D/Urho3D.h>

namespace Urho3D
{
	class Camera;
	class Node;
}

namespace Prime
{
	namespace Editor
	{
		class PluginScene3DEditor : public EditorPlugin
		{
			OBJECT(PluginScene3DEditor);
		public:
			/// Construct.
			PluginScene3DEditor(Urho3D::Context* context);
			/// Destruct.
			virtual ~PluginScene3DEditor();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			/// called when the plugin is activated. 
			virtual void Enter();
			/// called when the plugin is closed or switched to other main plugin.
			virtual void Leave();
			/// return false if the input was not consumed otherwise true.
			virtual bool OnKeyInput(int key, bool down);
			/// return false if the input was not consumed otherwise true.
			virtual bool OnMouseButtonInput(int button, bool down);
			/// return false if this plugin is a subeditor and does not manipulate the main frame window
			virtual bool HasMainFrame();

			virtual void Update(float delta);
		protected:
			void HandleInputAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			bool initialized_;

			Urho3D::SharedPtr<Urho3D::Camera> camera_;

			Urho3D::SharedPtr<Urho3D::Node> cameraNode_;

			/// Camera yaw angle.
			float yaw_;
			/// Camera pitch angle.
			float pitch_;
			/// Input Vector
			Urho3D::Vector3 moveVector;

		};
	}
}
#endif