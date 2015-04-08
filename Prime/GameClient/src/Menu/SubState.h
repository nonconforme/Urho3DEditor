#ifndef _MENU_SUBSTATE_H
#define _MENU_SUBSTATE_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/UIElement.h>

namespace Prime
{
	class StateObject;

	namespace Menu
	{
		/// %StateObject registers event handlers by default
		class SubState : public Urho3D::UIElement
		{
			OBJECT(SubState);

		public:
			SubState(Urho3D::Context* context);

		protected:

			virtual void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			virtual void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			virtual void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			virtual void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		protected:

			void SetState(Prime::StateObject* state);
			void SetSubState(SubState* state);

		};
	}
}
#endif