#ifndef _STATEMANAGER_H
#define _STATEMANAGER_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>

namespace Prime
{

	/// %StateManager able to be set as a SubSystem in Urho3D
	class URHO3D_API StateManager : public Urho3D::Object
	{
		OBJECT(StateManager);

		enum InternalState
		{
			NO_TRANSITION,
			LOADINGSCREEN
		};

	public:
		/// Construct.
		StateManager(Urho3D::Context* context);
		/// Constructor with an initial loading state
		StateManager(Urho3D::Context* context, Urho3D::Object* loadingState);
		/// Destruct.
		~StateManager();

	public:
		Urho3D::Object* GetState();
		Urho3D::Object* GetLoadingState();

	protected:

		void HandleSwitchState(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleStateLoadingComplete(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::SharedPtr<Urho3D::Object> _state;
		Urho3D::SharedPtr<Urho3D::Object> _loadingState;

		InternalState _internalState;
	};
}
#endif