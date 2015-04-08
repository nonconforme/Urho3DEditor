#include "SubState.h"
#include "StateObject.h"
#include "StateEvents.h"

#include <Urho3D/Core/Context.h>

namespace Prime
{
	namespace Menu
	{

		SubState::SubState(Urho3D::Context* context) :
			Urho3D::UIElement(context)
		{
			SubscribeToEvent(this, E_STATEPRESTART, HANDLER(SubState, HandleStatePreStart));
			SubscribeToEvent(this, E_STATESTART, HANDLER(SubState, HandleStateStart));
			SubscribeToEvent(this, E_STATEEND, HANDLER(SubState, HandleStateEnd));
			SubscribeToEvent(this, E_STATEPOSTEND, HANDLER(SubState, HandleStatePostEnd));
		}

		void SubState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void SubState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void SubState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void SubState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
		}

		void SubState::SetState(Prime::StateObject* state)
		{
			Urho3D::VariantMap eventData = context_->GetEventDataMap();
			eventData[SwitchState::P_NEWSTATE] = state;
			SendEvent(E_SWITCHSTATE, eventData);
		}

		void SubState::SetSubState(SubState* state)
		{
			Urho3D::VariantMap eventData = context_->GetEventDataMap();
			eventData[SwitchSubState::P_NEWSUBSTATE] = state;
			SendEvent(E_SWITCHSUBSTATE, eventData);
		}
	}
}