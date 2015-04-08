#ifndef _GAME_EVENTS_H
#define _GAME_EVENTS_H

#include <Urho3D/Core/Object.h>

namespace Prime
{
	namespace GamePlay
	{

		EVENT(E_SETCAMERA, SetCamera)
		{
			PARAM(P_CAMERA_NODE, CameraNode); // Node ptr
		}

		EVENT(E_KILLPLAYER, KillPlayer)
		{
		}

		EVENT(E_LEVELCOMPLETE, LevelComplete)
		{
		}

		EVENT(E_SET_RESPAWN, SetRespawn)
		{
			PARAM(P_HANDLED, Handled); // bool
		}
	}
}
#endif