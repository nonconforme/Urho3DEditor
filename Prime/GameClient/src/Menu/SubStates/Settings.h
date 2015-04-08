#ifndef _MENU_SUBSTATE_SETTINGSMENU_H
#define _MENU_SUBSTATE_SETTINGSMENU_H

#include "../SubState.h"

namespace Prime
{
	namespace Menu
	{
		namespace SubStates
		{
			class Settings : public SubState
			{
				OBJECT(Settings);
			public:
				Settings(Urho3D::Context* context);
				~Settings();

			protected: // Internal functions


			protected: // Event handlers

				void HandlePlayButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
				void HandleEditorButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
				void HandleSettingsButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
				void HandleExitButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

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