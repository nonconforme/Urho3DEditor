#ifndef _GAME_SPAWNPOINT_H
#define _GAME_SPAWNPOINT_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D
{

}
namespace Prime
{
	namespace GamePlay
	{
		class SpawnPoint : public Urho3D::LogicComponent
		{
			OBJECT(SpawnPoint);
		public:
			SpawnPoint(Urho3D::Context* context, Urho3D::Node* cameraNode = 0);
			~SpawnPoint();

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

		private:
		};
	}
}
#endif