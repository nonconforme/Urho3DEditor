#include "MainMenu.h"
#include "StateManager.h"
#include "StateEvents.h"
#include "../Menu.h"
#include "../../Game/Game.h"
#include "../../Editor/EditorState.h"
#include "PlayGame.h"
#include "Settings.h"
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


			MainMenuSubState::MainMenuSubState(Urho3D::Context* context) :
				SubState(context)
			{

			}
			MainMenuSubState::~MainMenuSubState()
			{

			}

			void MainMenuSubState::HandlePlayButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				SetState(new GamePlay::GameplayState(context_));
			}

			void MainMenuSubState::HandleEditorButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				SetState(new Editor::EditorState(context_));
			}

			void MainMenuSubState::HandleSettingsButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				SetSubState(new Settings(context_));
			}

			void MainMenuSubState::HandleExitButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				context_->GetSubsystem<Urho3D::Engine>()->Exit();
			}

			void MainMenuSubState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
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
				_buttonContainer->SetPosition(-(GetWidth() / 4), 0);
				_buttonContainer->SetHeight(24);
				_buttonContainer->SetLayoutMode(Urho3D::LM_VERTICAL);
				_buttonContainer->SetLayoutSpacing(20);
				_buttonContainer->SetVisible(false);

				// Create New Game Button
				Urho3D::Button* _button_Play = _buttonContainer->CreateChild<Urho3D::Button>();
				_button_Play->SetColor(Urho3D::Color(0.f, 0.f, 0.f, 0.f));
				_button_Play->SetFixedSize(64, 24);

				Urho3D::Text* _button_Play_Text = _button_Play->CreateChild<Urho3D::Text>();
				_button_Play_Text->SetFont(font, 14);
				_button_Play_Text->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				_button_Play_Text->SetText("Play");

				// Create Editor Button
				Urho3D::Button* _button_Editor = _buttonContainer->CreateChild<Urho3D::Button>();
				_button_Editor->SetColor(Urho3D::Color(0.f, 0.f, 0.f, 0.f));
				_button_Editor->SetFixedSize(64, 24);

				Urho3D::Text* button_Editor_Text = _button_Editor->CreateChild<Urho3D::Text>();
				button_Editor_Text->SetFont(font, 14);
				button_Editor_Text->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				button_Editor_Text->SetText("Editor");

				// Create Settings Button
				Urho3D::Button* _button_Settings = _buttonContainer->CreateChild<Urho3D::Button>();
				_button_Settings->SetColor(Urho3D::Color(0.f, 0.f, 0.f, 0.f));
				_button_Settings->SetFixedSize(64, 24);

				Urho3D::Text* button_Settings_Text = _button_Settings->CreateChild<Urho3D::Text>();
				button_Settings_Text->SetFont(font, 14);
				button_Settings_Text->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				button_Settings_Text->SetText("Settings");

				// Create Exit Button
				Urho3D::Button* _button_Exit = _buttonContainer->CreateChild<Urho3D::Button>();
				_button_Exit->SetColor(Urho3D::Color(0.f, 0.f, 0.f, 0.f));
				_button_Exit->SetFixedSize(64, 24);

				Urho3D::Text* button_Exit_Text = _button_Exit->CreateChild<Urho3D::Text>();
				button_Exit_Text->SetFont(font, 14);
				button_Exit_Text->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				button_Exit_Text->SetText("Exit");

				SubscribeToEvent(_button_Play, Urho3D::E_RELEASED, HANDLER(MainMenuSubState, HandlePlayButton));
				SubscribeToEvent(_button_Editor, Urho3D::E_RELEASED, HANDLER(MainMenuSubState, HandleEditorButton));
				SubscribeToEvent(_button_Settings, Urho3D::E_RELEASED, HANDLER(MainMenuSubState, HandleSettingsButton));
				SubscribeToEvent(_button_Exit, Urho3D::E_RELEASED, HANDLER(MainMenuSubState, HandleExitButton));
			}

			void MainMenuSubState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				Urho3D::UI* ui = context_->GetSubsystem<Urho3D::UI>();
				Urho3D::UIElement* root = ui->GetRoot();
				root->AddChild(this);

				_buttonContainer->SetVisible(true);
			}

			void MainMenuSubState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				_buttonContainer->SetVisible(false);
			}

			void MainMenuSubState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
			{
				Remove();
			}
		}
	}
}