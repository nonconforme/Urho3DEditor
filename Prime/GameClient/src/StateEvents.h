#ifndef _STATE_EVENTS_H
#define _STATE_EVENTS_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>

namespace Prime
{
	/// %StatePreStart is sent to a state when it is about to load, it is to let
	/// it preload its assets and prepare its scene
	EVENT(E_STATEPRESTART, StatePreStart)
	{
	}

	/// %StateStart is sent when it is time to add its rendering data to the scene
	/// and to fully activate.  This should be fast
	EVENT(E_STATESTART, StateStart)
	{
	}

	/// %StateEnd is sent when it needs to stop its rendering, such as removing
	/// nodes and whatever else, this should be fast
	EVENT(E_STATEEND, StateEnd)
	{
	}

	/// %StatePostEnd is sent when the state is being removed, allowing it to
	/// clean up assets and whatever else it may need
	EVENT(E_STATEPOSTEND, StatePostEnd)
	{
	}

	/// %StateLoadingUpdate is received primarily just by the Loading State,
	/// it receives a message posted by the loading state between its PreStart
	/// and its Start to allow it to display the state of its loading.
	EVENT(E_STATELOADINGUPDATE, StateLoadingUpdate)
	{
		PARAM(P_MESSAGE, Message); // String
	}

	/// %StateLoadingStart is sent globally to announce a state change has
	/// started
	EVENT(E_STATELOADINGSTART, StateLoadingStart)
	{
		PARAM(P_NEWSTATE, NewState); // New StateObject
	}

	/// %StateLoadingEnd is sent globally to announce a state change has
	/// finished and is now fully loaded and operational
	EVENT(E_STATELOADINGEND, StateLoadingEnd)
	{
		PARAM(P_NEWSTATE, NewState); // New StateObject
	}

	EVENT(E_STATELOADINGCOMPLETE, StateLoadingComplete)
	{
	}

	EVENT(E_SWITCHSTATE, SwitchState)
	{
		PARAM(P_NEWSTATE, NewState); // New State
	}

	EVENT(E_SWITCHSUBSTATE, SwitchSubState)
	{
		PARAM(P_NEWSUBSTATE, NewSubState); // New SubState
	}

	EVENT(E_LOADLEVEL, LoadLevel)
	{
		PARAM(P_LEVELNAME, LevelName);
	}
}

#endif