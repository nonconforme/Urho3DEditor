#include "PlayGame.h"
#include "StateManager.h"
#include "StateEvents.h"
#include "../Menu.h"
//#include "../../Game/Game.h"
//#include "../../Editor/Editor.h"
#include "MainMenu.h"
#include "Root.h"

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
		namespace SubStates
		{


			PlayGameSubState::PlayGameSubState(Urho3D::Context* context) :
				SubState(context)
			{

			}

			PlayGameSubState::~PlayGameSubState()
			{

			}

			void PlayGameSubState::HandleBackButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				Urho3D::VariantMap map;
				map[SwitchSubState::P_NEWSUBSTATE] = new MainMenuSubState(context_);
				SendEvent(E_SWITCHSUBSTATE, map);
			}

			void PlayGameSubState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
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
				// TODO: Refactor into better UI

				SetFixedSize(gfx->GetWidth(), gfx->GetHeight());
				SetPosition(0, 0);
				SetLayoutMode(Urho3D::LM_FREE);

				// Setup button container
				_buttonContainer = CreateChild<Urho3D::UIElement>();
				_buttonContainer->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				_buttonContainer->SetPosition(-(GetWidth() / 3.f), (GetHeight() / 3.f));
				_buttonContainer->SetHeight(24);
				_buttonContainer->SetLayoutMode(Urho3D::LM_VERTICAL);
				_buttonContainer->SetLayoutSpacing(20);
				_buttonContainer->SetVisible(false);

				// Create Back Button
				Urho3D::Button* _button_Exit = _buttonContainer->CreateChild<Urho3D::Button>();
				_button_Exit->SetColor(Urho3D::Color(0.f, 0.f, 0.f, 0.f));
				_button_Exit->SetFixedSize(64, 24);

				Urho3D::Text* button_Exit_Text = _button_Exit->CreateChild<Urho3D::Text>();
				button_Exit_Text->SetFont(font, 14);
				button_Exit_Text->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				button_Exit_Text->SetText("Back");

				SubscribeToEvent(_button_Exit, Urho3D::E_RELEASED, HANDLER(PlayGameSubState, HandleBackButton));
			}

			void PlayGameSubState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				Urho3D::UI* ui = context_->GetSubsystem<Urho3D::UI>();
				Urho3D::UIElement* root = ui->GetRoot();
				root->AddChild(this);

				_buttonContainer->SetVisible(true);
			}

			void PlayGameSubState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				_buttonContainer->SetVisible(false);
			}

			void PlayGameSubState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				Remove();
			}
		}
	}
}