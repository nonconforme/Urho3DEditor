#include "Game.h"
#include "Events.h"
#include "StateEvents.h"
#include "Player.h"
#include "DeathZone.h"
#include "Root.h"

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

		GameplayState::GameplayState(Urho3D::Context* context) :
			StateObject(context)
		{
			CreateScene();
			CreateUI();
			CreateCamera();

			SpawnPlayer();

			SubscribeToEvents();
		}

		GameplayState::~GameplayState()
		{
			if (_uiContainer)
				_uiContainer->Remove();
		}

		void GameplayState::Init(Urho3D::String& mapName)
		{

		}

		void GameplayState::SubscribeToEvents()
		{
			SubscribeToEvent(E_KILLPLAYER, HANDLER(GameplayState, HandleKillPlayer));
		}

		void GameplayState::CreateScene()
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			_scene = new Urho3D::Scene(context_);
			//Urho3D::SharedPtr<Urho3D::File> sceneFile = GetSubsystem<Urho3D::ResourceCache>()->GetFile("GameData/MainMenu.xml");
			//_scene->LoadXML(*sceneFile);
			_scene->SetName("Settings");
			_scene->CreateComponent<Urho3D::Octree>();
			_scene->CreateComponent<Urho3D::PhysicsWorld>();

			Urho3D::Node* zoneNode = _scene->CreateChild("Zone", Urho3D::LOCAL);
			Urho3D::Zone* zone = zoneNode->CreateComponent<Urho3D::Zone>();
			zone->SetBoundingBox(Urho3D::BoundingBox(-1000.0f, 1000.0f));
			zone->SetAmbientColor(Urho3D::Color(0.1f, 0.1f, 0.1f));
			zone->SetFogStart(100.0f);
			zone->SetFogEnd(300.0f);

			// Create a directional light without shadows
			Urho3D::Node* lightNode = _scene->CreateChild("DirectionalLight", Urho3D::LOCAL);
			lightNode->SetDirection(Urho3D::Vector3(0.5f, -1.0f, 0.5f));
			Urho3D::Light* light = lightNode->CreateComponent<Urho3D::Light>();
			light->SetLightType(Urho3D::LIGHT_DIRECTIONAL);
			light->SetColor(Urho3D::Color(0.2f, 0.2f, 0.2f));
			light->SetSpecularIntensity(1.0f);

			// Create a "floor" consisting of several tiles. Make the tiles physical but leave small cracks between them
			for (int y = -3; y <= 3; ++y)
			{
				for (int x = -3; x <= 3; ++x)
				{
					Urho3D::Node* floorNode = _scene->CreateChild("FloorTile");
					floorNode->SetPosition(Urho3D::Vector3(x * 20.2f, y * x * -2.5f, y * 20.2f));
					floorNode->SetScale(Urho3D::Vector3(20.0f, 1.0f, 20.0f));
					Urho3D::StaticModel* floorObject = floorNode->CreateComponent<Urho3D::StaticModel>();
					floorObject->SetModel(cache->GetResource<Urho3D::Model>("Models/Box.mdl"));
					floorObject->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Stone.xml"));

					Urho3D::RigidBody* body = floorNode->CreateComponent<Urho3D::RigidBody>();

					if (Urho3D::Abs(x) > 0 && Urho3D::Abs(y) > 0)
					{
						DeathZone* deathZone = floorNode->CreateComponent<DeathZone>();
					}	

					body->SetFriction(1.0f);
					Urho3D::CollisionShape* shape = floorNode->CreateComponent<Urho3D::CollisionShape>();
					shape->SetBox(Urho3D::Vector3::ONE);
				}
			}

			//_template_PlayerNode = new Urho3D::Node(context_);
			//_template_PlayerNode->SetEnabled(false);
			//_scene->AddChild(_template_PlayerNode);
			//_template_PlayerNode->CreateComponent<PlayerComponent>();
			//_template_PlayerNode->CreateComponent<CameraComponent>();

			//Urho3D::StaticModel* model = _template_PlayerNode->CreateComponent<Urho3D::StaticModel>();

			//model->SetModel(cache->GetResource<Urho3D::Model>("Models/Sphere.mdl"));
			//model->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Water.xml"));
			//model->SetCastShadows(true);
			//// Create the physics components
			//Urho3D::RigidBody* body = _template_PlayerNode->CreateComponent<Urho3D::RigidBody>();
			//body->SetMass(1.0f);
			//body->SetFriction(1.0f);
			//// In addition to friction, use motion damping so that the ball can not accelerate limitlessly
			//body->SetLinearDamping(0.5f);
			//body->SetAngularDamping(0.5f);
			//Urho3D::CollisionShape* shape = _template_PlayerNode->CreateComponent<Urho3D::CollisionShape>();
			//shape->SetSphere(1.0f);

			//Urho3D::Light* playerLight = _template_PlayerNode->CreateComponent<Urho3D::Light>();
			//light->SetRange(3.0f);
			//light->SetColor(Urho3D::Color(0.5f + (Urho3D::Rand() & 1) * 0.5f, 0.5f + (Urho3D::Rand() & 1) * 0.5f, 0.5f + (Urho3D::Rand() & 1) * 0.5f));
		}

		void GameplayState::CreateUI()
		{
			Urho3D::UI* ui = context_->GetSubsystem<Urho3D::UI>();
			Urho3D::Input* input = context_->GetSubsystem<Urho3D::Input>();
			Urho3D::Graphics* gfx = context_->GetSubsystem<Urho3D::Graphics>();
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			Urho3D::XMLFile* uiStyle = cache->GetResource<Urho3D::XMLFile>("UI/DefaultStyle.xml");
			Urho3D::Font* font = cache->GetResource<Urho3D::Font>("Fonts/Anonymous Pro.ttf");

			Urho3D::UIElement* root = ui->GetRoot();
			root->SetDefaultStyle(uiStyle);

			// Set mouse cursor to be visible while in the main menu

			input->SetMouseVisible(true);

			// Setup UI elements
			// TODO: Re factor into better UI

			int uiHeight = ui->GetRoot()->GetHeight();
			int uiWidth = ui->GetRoot()->GetWidth();

			// Setup login dialog container
			_uiContainer = root->CreateChild<Urho3D::UIElement>();
			_uiContainer->SetFixedSize(gfx->GetWidth(), gfx->GetHeight());
			_uiContainer->SetPosition(0, 0);
			_uiContainer->SetLayoutMode(Urho3D::LM_FREE);
		}

		void GameplayState::SpawnPlayer(Urho3D::Vector3 position)
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			if (_playerNode.NotNull())
			{
				_playerNode->Remove();
			}

			//_playerNode = _template_PlayerNode->Clone();
			_playerNode = _scene->CreateChild("Player");
			_playerNode->CreateComponent<Player>();
			_playerNode->SetPosition(position);
			_scene->AddChild(_playerNode);
			
			Urho3D::VariantMap map = context_->GetEventDataMap();
			map[SetCamera::P_CAMERA_NODE] = _cameraNode;
			SendEvent(E_SETCAMERA, map);
		}

		void GameplayState::CreateCamera()
		{
			_cameraNode = _scene->CreateChild("MainCamera");
			Urho3D::Camera* camera = _cameraNode->CreateComponent<Urho3D::Camera>();
			camera->SetFarClip(300.0f);

			Urho3D::SharedPtr<Urho3D::Viewport> viewport(new Urho3D::Viewport(context_, _scene, _cameraNode->GetComponent<Urho3D::Camera>()));
			GetSubsystem<Urho3D::Renderer>()->SetViewport(0, viewport);
			GetSubsystem<Urho3D::Audio>()->SetListener(_cameraNode->CreateComponent<Urho3D::SoundListener>());
		}

		void GameplayState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void GameplayState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			//CreateScene();
			//CreateUI();
		}

		void GameplayState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void GameplayState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void GameplayState::HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace StateLoadingUpdate;

			Urho3D::String message = eventData[P_MESSAGE].GetString();


		}

		void GameplayState::HandleKillPlayer(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			SpawnPlayer(Urho3D::Vector3(0.f, 5.f, 0.f));
		}
	}
}