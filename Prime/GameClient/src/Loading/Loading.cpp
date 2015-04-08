#include "Loading.h"
#include "StateEvents.h"
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

	LoadingState::LoadingState(Urho3D::Context* context) :
		StateObject(context)
	{

	}
	LoadingState::~LoadingState()
	{
		if (_uiContainer)
			_uiContainer->Remove();
	}

	void LoadingState::CreateScene()
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

	void LoadingState::CreateUI()
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

		int uiHeight = ui->GetRoot()->GetHeight();
		int uiWidth = ui->GetRoot()->GetWidth();

		// Setup login dialog container
		_uiContainer = root->CreateChild<Urho3D::UIElement>();
		_uiContainer->SetFixedSize(gfx->GetWidth(), gfx->GetHeight());
		_uiContainer->SetPosition(0, 0);
		_uiContainer->SetLayoutMode(Urho3D::LM_FREE);

		_loadingMessage = _uiContainer->CreateChild<Urho3D::Text>();
		_loadingMessage->SetFont(font, 14);
		_loadingMessage->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
		_loadingMessage->SetPosition(_uiContainer->GetWidth() / 4, _uiContainer->GetHeight() / 4);
		_loadingMessage->SetText("Loading...");

	}

	void LoadingState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{

	}

	void LoadingState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		//CreateScene();
		//CreateUI();
	}

	void LoadingState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{

	}

	void LoadingState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{

	}

	void LoadingState::HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace StateLoadingUpdate;

		Urho3D::String message = eventData[P_MESSAGE].GetString();

		//_loadingMessage->SetText("Loading... " + message);
	}
}