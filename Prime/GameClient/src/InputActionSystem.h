#pragma once

#include "UI/Macros.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>

namespace Prime
{

	EVENT(E_INPUTACTION, InputAction)
	{
		PARAM(P_ACTIONID, ActionID);        // StringHash
		PARAM(P_ISDOWN, IsDown);            // bool
	}

#define MouseButton_Mask 0x01;
#define Key_Mask 0x02;
#define MoveVert_Mask 0x04;
#define MoveHori_Mask 0x08;

	struct sInputAction
	{
		sInputAction(const Urho3D::String& name, int key, unsigned mouseButtons = 0, bool repeat = false);

		/// input definition
		int			key_;
		unsigned	mouseButtons_;
		Urho3D::StringHash	id_;
		Urho3D::String		name_;
		bool		repeat_;

		/// check if action is active
		unsigned	downMask_;
		unsigned	setMask_;
		bool IsActive(){ return ((downMask_ & setMask_) != 0); }

		/// Test for equality with another hash set.
		bool operator == (const sInputAction& rhs) const
		{		
			return id_ == rhs.id_;
		}
	};



	class ActionState : public Urho3D::Object
	{
		OBJECT(ActionState);
		BASEOBJECT(ActionState);
		friend class InputActionSystem;
	public:
		/// Construct.
		ActionState(Urho3D::Context* context);
		/// Destruct.
		virtual ~ActionState();

		void SetName(const Urho3D::String& name);

		void AddInputState(sInputAction* inAction);
		void AddKeyAction(const Urho3D::String& action, int key);
		void AddMouseButtonAction(const Urho3D::String& action, unsigned mouseButton);

		const Urho3D::String&		GetName() { return name_; }
		const Urho3D::StringHash&	GetID() { return nameHash_; }
		const Urho3D::String&		GetActionName(const Urho3D::StringHash& actionId);

	protected:

		Urho3D::StringHash	nameHash_;
		Urho3D::String		name_;

		Urho3D::HashMap<int, Urho3D::HashSet< sInputAction* > >			keyActionMapping2_;
		Urho3D::HashMap<unsigned, Urho3D::HashSet< sInputAction* > >			mouseButtonMapping2_;

		Urho3D::HashMap<int, Urho3D::HashSet<Urho3D::StringHash> >			keyActionMapping_;
		Urho3D::HashMap<unsigned, Urho3D::HashSet< Urho3D::StringHash> >	mouseButtonMapping_;

		Urho3D::HashMap<Urho3D::StringHash, Urho3D::String>		actionNameMapping_;
	};


	class InputActionSystem : public Urho3D::Object
	{
		OBJECT(InputActionSystem);
	public:
		/// Construct.
		InputActionSystem(Urho3D::Context* context);
		/// Destruct.
		virtual ~InputActionSystem();
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		bool RegisterActionState(ActionState* actionState);
		/// does not push same states on top of each other
		void Push(const Urho3D::StringHash& actionStateid);
		/// Returns the last active actions state
		ActionState* Pop(const Urho3D::StringHash& actionStateid);
		/// Returns the active actions state
		ActionState* Get();

	protected:
		
		void HandleMouseButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleMouseButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		void HandleJoystickButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleJoystickButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleJoystickHatMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		void HandleTouchEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTouchBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTouchMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Active finger touches.
		Urho3D::HashMap<Urho3D::StringHash, ActionState*> actionStates_;
		Urho3D::Vector<ActionState*>	stack_;
		ActionState*			currentState_;
	};


}