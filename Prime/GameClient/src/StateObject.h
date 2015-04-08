#ifndef _STATEOBJECT_H
#define _STATEOBJECT_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>

namespace Prime
{
	/// %StateObject registers event handlers by default
	class StateObject : public Urho3D::Object
	{
		OBJECT(StateObject);

	public:
		StateObject(Urho3D::Context* context);

		void SetState();

	protected:
		virtual void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		virtual void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		virtual void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		virtual void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		virtual void HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	protected:

		void SetState(StateObject* state);
		void PostLoadingUpdate(const Urho3D::String& msg);
		void PostLoadingComplete();

	private:

	};
}

#endif