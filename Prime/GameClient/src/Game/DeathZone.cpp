#include "DeathZone.h"
#include "Events.h"
#include "Common.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/StaticModel.h>

namespace Prime
{
	namespace GamePlay
	{

		DeathZone::DeathZone(Urho3D::Context* context, Urho3D::Node* cameraNode) :
			LogicComponent(context)
		{

		}

		DeathZone::~DeathZone()
		{

		}

		void DeathZone::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<DeathZone>("Game");
		}

		/// Handle enabled/disabled state change. Changes update event subscription.
		void DeathZone::OnSetEnabled()
		{

		}

		/// Called when the component is added to a scene node. Other components may not yet exist.
		void DeathZone::Start()
		{
			SubscribeToEvent(GetNode(), Urho3D::E_NODECOLLISION, HANDLER(DeathZone, HandleNodeCollision));
		}

		/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
		void DeathZone::DelayedStart()
		{
		}

		/// Called when the component is detached from a scene node, usually on destruction.
		void DeathZone::Stop()
		{

		}

		/// Called on scene update, variable timestep.
		void DeathZone::Update(float timeStep)
		{

		}

		/// Called on scene post-update, variable timestep.
		void DeathZone::PostUpdate(float timeStep)
		{

		}

		/// Called on physics update, fixed timestep.
		void DeathZone::FixedUpdate(float timeStep)
		{

		}

		/// Called on physics post-update, fixed timestep.
		void DeathZone::FixedPostUpdate(float timeStep)
		{

		}

		void DeathZone::HandleNodeCollision(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::NodeCollision;

			Urho3D::RigidBody* body = static_cast<Urho3D::RigidBody*>(eventData[P_BODY].GetPtr());
			Urho3D::Node* otherNode = static_cast<Urho3D::Node*>(eventData[P_OTHERNODE].GetPtr());
			Urho3D::RigidBody* otherBody = static_cast<Urho3D::RigidBody*>(eventData[P_OTHERBODY].GetPtr());
			bool trigger = eventData[P_TRIGGER].GetBool();

			if (GetComponent<Urho3D::RigidBody>() == body)
				SendEvent(E_KILLPLAYER);
		}
	}
}