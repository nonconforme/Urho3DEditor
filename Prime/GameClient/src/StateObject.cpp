#include "StateObject.h"
#include "StateManager.h"
#include "StateEvents.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>

namespace Prime
{

	StateObject::StateObject(Urho3D::Context* context) : Object(context)
	{
		SubscribeToEvent(this, E_STATEPRESTART, HANDLER(StateObject, HandleStatePreStart));
		SubscribeToEvent(this, E_STATESTART, HANDLER(StateObject, HandleStateStart));
		SubscribeToEvent(this, E_STATEEND, HANDLER(StateObject, HandleStateEnd));
		SubscribeToEvent(this, E_STATEPOSTEND, HANDLER(StateObject, HandleStatePostEnd));
		SubscribeToEvent(this, E_STATELOADINGUPDATE, HANDLER(StateObject, HandleLoadingUpdate));
	}

	void StateObject::SetState()
	{
		SetState(this);
	}

	void StateObject::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
	}

	void StateObject::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
	}

	void StateObject::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{

	}

	void StateObject::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{

	}

	void StateObject::HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{

	}

	void StateObject::SetState(StateObject* state)
	{
		Urho3D::VariantMap eventData = context_->GetEventDataMap();
		eventData[SwitchState::P_NEWSTATE] = state;
		SendEvent(E_SWITCHSTATE, eventData);
	}

	void StateObject::PostLoadingUpdate(const Urho3D::String& msg)
	{
		Urho3D::VariantMap eventData = context_->GetEventDataMap();
		eventData[StateLoadingUpdate::P_MESSAGE] = msg;
		SendEvent(E_STATELOADINGUPDATE, eventData);
	}

	void StateObject::PostLoadingComplete()
	{
		SendEvent(E_STATELOADINGCOMPLETE);
	}
}