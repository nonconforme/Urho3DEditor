#include "Menu.h"
#include "StateManager.h"
#include "StateEvents.h"
//#include "Game/Game.h"
//#include "Editor/Editor.h"
#include "SubStates/MainMenu.h"
#include "SubStates/PlayGame.h"
#include "SubStates/Settings.h"
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

namespace Prime
{
	namespace Menu
	{

		MenuState::MenuState(Urho3D::Context* context) :
			StateObject(context),
			_activeState(0)
		{

		}
		MenuState::~MenuState()
		{

		}

		void MenuState::CreateScene()
		{
			_scene = new Urho3D::Scene(context_);
			_scene->SetName("MainMenu");

			_cameraNode = new Urho3D::Node(context_);
			Urho3D::Camera* camera = _cameraNode->CreateComponent<Urho3D::Camera>();
			camera->SetFarClip(300.0f);
			_cameraNode->SetPosition(Urho3D::Vector3(0.0f, 0.0f, 0.0f));

			Urho3D::SharedPtr<Urho3D::Viewport> viewport(new Urho3D::Viewport(context_, _scene, _cameraNode->GetComponent<Urho3D::Camera>()));
			GetSubsystem<Urho3D::Renderer>()->SetViewport(0, viewport);
			GetSubsystem<Urho3D::Audio>()->SetListener(_cameraNode->CreateComponent<Urho3D::SoundListener>());
		}

		void MenuState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			CreateScene();

			SubscribeToEvent(E_SWITCHSUBSTATE, HANDLER(MenuState, HandleSwitchSubState));

			PostLoadingComplete();
		}

		void MenuState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			Urho3D::VariantMap map;
			map[SwitchSubState::P_NEWSUBSTATE] = new Menu::SubStates::MainMenuSubState(context_);
			SendEvent(E_SWITCHSUBSTATE, map);
		}

		void MenuState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (_activeState)
			{
				_activeState->SendEvent(E_STATEEND);
			}
		}

		void MenuState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (_activeState)
			{
				_activeState->SendEvent(E_STATEPOSTEND);
				_activeState.Reset();
			}
		}

		void MenuState::HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void MenuState::HandleSwitchSubState(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace SwitchSubState;

			SubState* subState = static_cast<SubState*>(eventData[P_NEWSUBSTATE].GetPtr());

			if (_activeState)
			{
				_activeState->SendEvent(E_STATEEND);
				_activeState->SendEvent(E_STATEPOSTEND);
				_activeState.Reset();
			}
			if (subState)
			{
				_activeState = subState;

				SubscribeToEvent((Urho3D::Object*)subState, E_STATESTART, HANDLER(MenuState, HandleSubStateStart));
				SubscribeToEvent((Urho3D::Object*)subState, E_STATEEND, HANDLER(MenuState, HandleSubStateEnd));

				subState->SendEvent(E_STATEPRESTART);
				subState->SendEvent(E_STATESTART);
			}
		}

		void MenuState::HandleSubStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void MenuState::HandleSubStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}
	}
}