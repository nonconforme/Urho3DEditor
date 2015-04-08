#ifndef _GAME_PLAYER_H
#define _GAME_PLAYER_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D
{

}
namespace Prime
{
	namespace GamePlay
	{
		struct Controls
		{
			Controls()
			{
				forward = 0;
				backwards = 0;
				left = 0;
				right = 0;
				power1 = 0;
			}
			bool forward;
			bool backwards;
			bool left;
			bool right;
			bool power1;
		};

		class Player : public Urho3D::LogicComponent
		{
			OBJECT(Player);
		public:
			Player(Urho3D::Context* context);
			~Player();

			static void RegisterObject(Urho3D::Context* context);

			/// Handle enabled/disabled state change. Changes update event subscription.
			void OnSetEnabled() override;
			/// Called when the component is added to a scene node. Other components may not yet exist.
			void Start() override;
			/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
			void DelayedStart() override;
			/// Called when the component is detached from a scene node, usually on destruction.
			void Stop() override;
			/// Called on scene update, variable timestep.
			void Update(float timeStep) override;
			/// Called on scene post-update, variable timestep.
			void PostUpdate(float timeStep) override;
			/// Called on physics update, fixed timestep.
			void FixedUpdate(float timeStep) override;
			/// Called on physics post-update, fixed timestep.
			void FixedPostUpdate(float timeStep) override;

		protected: // Internal functions

		protected: // Event handlers

			void HandleSetCamera(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		private:
			float _cameraDistance;
			Urho3D::WeakPtr<Urho3D::Node> _cameraNode;
			Urho3D::IntVector2 _cameraRotation;

			Controls _controls;
		};
	}
}
#endif