#include "StateManager.h"
#include "StateObject.h"
#include "StateEvents.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>

namespace Prime
{

	StateManager::StateManager(Urho3D::Context* context) :
		Urho3D::Object(context),
		_internalState(NO_TRANSITION)
	{
		SubscribeToEvent(E_SWITCHSTATE, HANDLER(StateManager, HandleSwitchState));
		SubscribeToEvent(E_STATELOADINGCOMPLETE, HANDLER(StateManager, HandleStateLoadingComplete));
	}

	StateManager::StateManager(Urho3D::Context* context, Urho3D::Object* loadingState)
		: Object(context)
		, _loadingState(loadingState)
		, _internalState(NO_TRANSITION)
	{

	}

	StateManager::~StateManager()
	{
	}

	Urho3D::Object* StateManager::GetState()
	{
		return _state;
	}

	//void StateManager::SetLoadingState(Object* loadingState)
	//{
	//	if (_internalState != NO_TRANSITION) {
	//        throw "Tried to set loading state while already setting a state";
	//    }
	//	_loadingState = loadingState;
	//}

	Urho3D::Object* StateManager::GetLoadingState()
	{
		return _loadingState;
	}

	void StateManager::HandleSwitchState(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace SwitchState;

		StateObject* state = static_cast<StateObject*>(eventData[P_NEWSTATE].GetPtr());

		if (_internalState != NO_TRANSITION) {
			throw "Tried to set state while already setting a state";
		}
		_internalState = LOADINGSCREEN;


		/*if (_loadingState) {
		_loadingState->SendEvent(E_STATEPRESTART);
		}*/
		if (_state) {
			_state->SendEvent(E_STATEEND);
		}
		/*if (_loadingState) {
		_loadingState->SendEvent(E_STATESTART);
		}*/
		if (_state) {
			_state->SendEvent(E_STATEPOSTEND);
		}

		Urho3D::SharedPtr<Object> oldState = _state;
		_state = state;

		if (state) {
			state->SendEvent(E_STATEPRESTART);
		}

		Urho3D::VariantMap map = context_->GetEventDataMap();
		map[StateLoadingStart::P_NEWSTATE] = state;
		SendEvent(E_STATELOADINGSTART, map);
	}

	void StateManager::HandleStateLoadingComplete(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace StateLoadingComplete;

		if (_internalState != LOADINGSCREEN) {
			throw "Tried to post done loading to switch to final state while not loading";
		}
		_internalState = NO_TRANSITION;

		//if (_loadingState) {
		//_loadingState->SendEvent(E_STATEEND);
		//}
		if (_state) {
			_state->SendEvent(E_STATESTART);
		}
		//if (_loadingState) {
		//_loadingState->SendEvent(E_STATEPOSTEND);
		//}

		Urho3D::VariantMap& map = context_->GetEventDataMap();
		eventData[StateLoadingEnd::P_NEWSTATE] = _state;
		SendEvent(E_STATELOADINGEND, map);
	}
}