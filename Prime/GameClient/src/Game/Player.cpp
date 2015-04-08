#include "Player.h"
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
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
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

		Player::Player(Urho3D::Context* context) :
			LogicComponent(context),
			_cameraDistance(5.f),
			_cameraRotation(0, 0),
			_controls()
		{
		}

		Player::~Player()
		{

		}

		void Player::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<Player>("Game");
		}

		/// Handle enabled/disabled state change. Changes update event subscription.
		void Player::OnSetEnabled()
		{

		}

		/// Called when the component is added to a scene node. Other components may not yet exist.
		void Player::Start()
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			//Urho3D::StaticModel* model = GetNode()->CreateComponent<Urho3D::StaticModel>();

			//model->SetModel(cache->GetResource<Urho3D::Model>("Models/Sphere.mdl"));
			//model->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Water.xml"));
			//model->SetCastShadows(true);
			//// Create the physics components
			//Urho3D::RigidBody* body = GetNode()->CreateComponent<Urho3D::RigidBody>();
			//body->SetMass(1.0f);
			//body->SetFriction(1.0f);
			//// In addition to friction, use motion damping so that the ball can not accelerate limitlessly
			//body->SetLinearDamping(0.5f);
			//body->SetAngularDamping(0.5f);
			//Urho3D::CollisionShape* shape = GetNode()->CreateComponent<Urho3D::CollisionShape>();
			//shape->SetSphere(1.0f);

			//Urho3D::Light* playerLight = GetNode()->CreateComponent<Urho3D::Light>();
			//playerLight->SetRange(3.0f);
			//playerLight->SetColor(Urho3D::Color(0.5f + (Urho3D::Rand() & 1) * 0.5f, 0.5f + (Urho3D::Rand() & 1) * 0.5f, 0.5f + (Urho3D::Rand() & 1) * 0.5f));

			SubscribeToEvent(E_SETCAMERA, HANDLER(Player, HandleSetCamera));
			SubscribeToEvent(Urho3D::E_MOUSEWHEEL, HANDLER(Player, HandleMouseWheel));
			SubscribeToEvent(Urho3D::E_KEYDOWN, HANDLER(Player, HandleKeyDown));
			SubscribeToEvent(Urho3D::E_KEYUP, HANDLER(Player, HandleKeyUp));
		}

		/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
		void Player::DelayedStart()
		{
			
		}

		/// Called when the component is detached from a scene node, usually on destruction.
		void Player::Stop()
		{

		}

		/// Called on scene update, variable timestep.
		void Player::Update(float timeStep)
		{
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();
			Urho3D::Input* input = GetSubsystem<Urho3D::Input>();

			const float MOUSE_SENSITIVITY = 0.8f;

			if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT))
			{
				//ui->GetCursor()->SetVisible(false);

				Urho3D::IntVector2 mouseMove = input->GetMouseMove();
				_cameraRotation.x_ += MOUSE_SENSITIVITY * mouseMove.x_;
				_cameraRotation.y_ += MOUSE_SENSITIVITY * mouseMove.y_;
				_cameraRotation.y_ = Urho3D::Clamp((float)_cameraRotation.y_, 1.0f, 90.0f);
			}
			else
			{
				//ui->GetCursor()->SetVisible(true);
			}

			if (_cameraNode.NotNull())
			{
				_cameraNode->SetRotation(Urho3D::Quaternion(_cameraRotation.y_, _cameraRotation.x_, 0.0f));
				_cameraNode->SetPosition(GetNode()->GetPosition() + _cameraNode->GetRotation() * Urho3D::Vector3::BACK * _cameraDistance);
			}
		}

		/// Called on scene post-update, variable timestep.
		void Player::PostUpdate(float timeStep)
		{

		}

		/// Called on physics update, fixed timestep.
		void Player::FixedUpdate(float timeStep)
		{
			const float MOVE_TORQUE = 4.5f;

			Urho3D::RigidBody* body = GetNode()->GetComponent<Urho3D::RigidBody>();

			if (!body)
				return;

			Urho3D::Node* node = _cameraNode ? _cameraNode : GetNode();

			Urho3D::Quaternion rot = Urho3D::Quaternion();
			rot.FromEulerAngles(0.f, node->GetRotation().YawAngle(), 0.f);

			if (_controls.forward)
				body->ApplyTorque(rot * Urho3D::Vector3::RIGHT * MOVE_TORQUE);
			if (_controls.backwards)
				body->ApplyTorque(rot * Urho3D::Vector3::LEFT * MOVE_TORQUE);
			if (_controls.left)
				body->ApplyTorque(rot * Urho3D::Vector3::FORWARD * MOVE_TORQUE);
			if (_controls.right)
				body->ApplyTorque(rot * Urho3D::Vector3::BACK * MOVE_TORQUE);

			if (_controls.power1)
			{
				_controls.power1 = false;

				body->ApplyForce(rot * Urho3D::Vector3::UP * MOVE_TORQUE * 100.f);
			}
		}

		/// Called on physics post-update, fixed timestep.
		void Player::FixedPostUpdate(float timeStep)
		{

		}

		void Player::HandleSetCamera(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace SetCamera;

			_cameraNode = static_cast<Urho3D::Node*>(eventData[P_CAMERA_NODE].GetPtr());
		}

		void Player::HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseWheel;

			int wheel = eventData[P_WHEEL].GetInt();			//int
			eventData[P_BUTTONS];		//int
			eventData[P_QUALIFIERS];	//int

			const float WHEEL_SENSITIVITY = 0.8f;

			_cameraDistance += wheel * WHEEL_SENSITIVITY;
			_cameraDistance = Urho3D::Clamp(_cameraDistance, 1.f, 20.f);
		}

		void Player::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::KeyDown;

			int key = eventData[P_KEY].GetInt();			//int
			eventData[P_BUTTONS];		//int
			eventData[P_QUALIFIERS];	//int
			eventData[P_REPEAT];		//bool
			eventData[P_RAW];			//uint

			if (key == Urho3D::KEY_W)
				_controls.forward = true;
			if (key == Urho3D::KEY_S)
				_controls.backwards = true;
			if (key == Urho3D::KEY_A)
				_controls.left = true;
			if (key == Urho3D::KEY_D)
				_controls.right = true;
			if (key == Urho3D::KEY_SPACE)
				_controls.power1 = true;
		}

		void Player::HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::KeyUp;

			int key = eventData[P_KEY].GetInt();			//int
			eventData[P_BUTTONS];		//int
			eventData[P_QUALIFIERS];	//int
			eventData[P_RAW];			//uint

			if (key == Urho3D::KEY_W)
				_controls.forward = false;
			if (key == Urho3D::KEY_S)
				_controls.backwards = false;
			if (key == Urho3D::KEY_A)
				_controls.left = false;
			if (key == Urho3D::KEY_D)
				_controls.right = false;
			if (key == Urho3D::KEY_SPACE)
				_controls.power1 = false;
		}
	}
}