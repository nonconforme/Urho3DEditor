#ifndef _MENU_MENU_H
#define _MENU_MENU_H

#include "StateObject.h"
#include "SubState.h"

namespace Urho3D
{
	class Node;
	class Scene;
	class UIElement;
}

namespace Prime
{
	namespace Menu
	{
		class MenuState : public Prime::StateObject
		{
			OBJECT(MenuState);
		public:
			MenuState(Urho3D::Context* context);
			~MenuState();

		protected: // Internal functions

			void CreateScene();

		protected: // Event handlers

			void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleSwitchSubState(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleSubStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleSubStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		private:
			Urho3D::SharedPtr<Urho3D::Scene> _scene;
			Urho3D::SharedPtr<Urho3D::Node> _cameraNode;

			Urho3D::WeakPtr<Urho3D::UIElement> _dialog;
			Urho3D::SharedPtr<SubState> _activeState;
		};
	}
}
#endif