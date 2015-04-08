#ifndef _MENU_SUBSTATE_PLAYGAMEMENU_H
#define _MENU_SUBSTATE_PLAYGAMEMENU_H

#include "../SubState.h"

namespace Prime
{
	namespace Menu
	{
		namespace SubStates
		{


			class PlayGameSubState : public SubState
			{
				OBJECT(PlayGameSubState);
			public:
				PlayGameSubState(Urho3D::Context* context);
				~PlayGameSubState();

			protected: // Internal functions


			protected: // Event handlers

				void HandleBackButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

				void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
				void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
				void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
				void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			private:
				Urho3D::WeakPtr<Urho3D::UIElement> _buttonContainer;
			};
		}
	}
}
#endif