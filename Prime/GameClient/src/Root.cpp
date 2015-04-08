#include "Root.h"
#include "StateManager.h"
#include "UI/ResourcePicker.h"
#include "Loading/Loading.h"
#include "Menu/Menu.h"
#include "Game/Player.h"
#include "Game/DeathZone.h"
#include "Editor/EditorState.h"
#include "InputActionSystem.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>

namespace Prime
{

	Root Root::g_instance;
	bool  Root::g_initialized;

	Root::Root()
	{
		g_initialized = true;
	}

	Root::~Root()
	{
		g_initialized = false;
	}

	Root* Root::instance()
	{
		// if Root does not exist, create it
		return (g_initialized) ? &g_instance : new Root();
	}

	void Root::Run()
	{
		// Init
		_context = new Urho3D::Context();
		_engine = new Urho3D::Engine(_context);

		Urho3D::VariantMap engineParameters;
		engineParameters["WindowTitle"] = "B1UE";
		engineParameters["LogName"] = "B1UE.log";
		engineParameters["FullScreen"] = false;
		engineParameters["Headless"] = false;
		engineParameters["WindowResizable"] = true;
		
		//engineParameters["ForceGL2"] = true;
		//engineParameters["MultiSample"] = 16;
		//engineParameters["TextureAnisotropy"] = 16;
		//engineParameters["WindowResizable"] = true;
		engineParameters["RenderPath"] = "CoreData/RenderPaths/Deferred.xml";
		engineParameters["ResourcePaths"] = "Data;CoreData;IDEData";
		engineParameters["ResourcePrefixPath"] = "";
		//_engine->SetPauseMinimized(false);

		if (!_engine->Initialize(engineParameters))
		{
			return;
		}

		GamePlay::Player::RegisterObject(_context);
		GamePlay::DeathZone::RegisterObject(_context);
		Editor::EditorState::RegisterEditor(_context);

		_context->RegisterSubsystem(new UI::ResourcePickerManager(_context));
		_context->GetSubsystem<UI::ResourcePickerManager>()->Init();
		_context->RegisterSubsystem(new InputActionSystem(_context));

		// Setup StateManager
		_context->RegisterSubsystem(new StateManager(_context));

		Urho3D::WeakPtr<Menu::MenuState> menuState(new Menu::MenuState(_context));
		menuState->SetState();

		// Main thread
		m_bStop = false;
		while (!m_bStop && !_engine->IsExiting())
		{
			_engine->RunFrame();
		}
	}

	void Root::Stop()
	{
		m_bStop = true;
	}
}