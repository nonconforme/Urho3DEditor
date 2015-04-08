#ifndef _GAME_GAME_H
#define _GAME_GAME_H

#include "StateObject.h"

namespace Urho3D
{
	class Camera;
	class CustomGeometry;
	class FileSelector;
	class Menu;
	class Node;
	class Scene;
	class Slider;
	class Text;
	class UIElement;
	class Window;
}

namespace Prime
{
	namespace GamePlay
	{
		class GameplayState : public StateObject
		{
			OBJECT(GameplayState);
		public:
			GameplayState(Urho3D::Context* context);
			~GameplayState();

			void Init(Urho3D::String& mapName);

		protected: // Internal functions

			void SubscribeToEvents();

			void CreateScene();
			void CreateUI();
			void CreateCamera();

			void SpawnPlayer(Urho3D::Vector3 position = Urho3D::Vector3::ZERO);
			
		protected: // Event handlers

			void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleKillPlayer(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		private:
			Urho3D::SharedPtr<Urho3D::Node> _template_PlayerNode;

			Urho3D::SharedPtr<Urho3D::Scene> _scene;
			Urho3D::SharedPtr<Urho3D::Node> _cameraNode;
			Urho3D::WeakPtr<Urho3D::Node> _playerNode;

			Urho3D::SharedPtr<Urho3D::UIElement> _uiContainer;
			Urho3D::SharedPtr<Urho3D::UIElement> _dialog;
		};
	}
}
#endif