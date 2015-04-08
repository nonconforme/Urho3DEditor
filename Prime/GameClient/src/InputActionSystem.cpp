#include "InputActionSystem.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Core/Variant.h>

#include "SDL/SDL_stdinc.h"

namespace Prime
{
	InputActionSystem::InputActionSystem(Urho3D::Context* context) : Object(context),
		currentState_(NULL)
	{
		SubscribeToEvent(Urho3D::E_KEYDOWN, HANDLER(InputActionSystem, HandleKeyDown));
		SubscribeToEvent(Urho3D::E_KEYUP, HANDLER(InputActionSystem, HandleKeyUp));
		SubscribeToEvent(Urho3D::E_MOUSEBUTTONDOWN, HANDLER(InputActionSystem, HandleMouseButtonDown));
		SubscribeToEvent(Urho3D::E_MOUSEBUTTONUP, HANDLER(InputActionSystem, HandleMouseButtonUp));
	}

	InputActionSystem::~InputActionSystem()
	{
		Urho3D::HashMap<Urho3D::StringHash, ActionState*>::Iterator it;

		for (it = actionStates_.Begin(); it != actionStates_.End(); it++)
		{
			if (it->second_)
			{
				delete it->second_;
				it->second_ = NULL;
			}
		}
		actionStates_.Clear();
	}

	void InputActionSystem::RegisterObject(Urho3D::Context* context)
	{
		context->RegisterFactory<InputActionSystem>();
	}

	bool InputActionSystem::RegisterActionState(ActionState* actionState)
	{
		if (!actionState)
			return false;

		if (actionStates_.Contains(actionState->GetID()))
			return false;

		actionStates_[actionState->GetID()] = actionState;
		return true;
	}

	void InputActionSystem::Push(const Urho3D::StringHash& actionStateid)
	{
		Urho3D::HashMap<Urho3D::StringHash, ActionState*>::Iterator it = actionStates_.Find(actionStateid);
		if (it != actionStates_.End())
			if (stack_.Empty() || stack_.Back() != it->second_)
			{
				stack_.Push(it->second_);
				currentState_ = it->second_;
			}
	}

	ActionState* InputActionSystem::Pop(const Urho3D::StringHash& actionStateid)
	{
		if (stack_.Empty())
			return NULL;

		ActionState* last = stack_.Back();
		stack_.Pop();
		if (stack_.Empty())
		{
			currentState_ = NULL;
		}
		else
			currentState_ = stack_.Back();

		return last;
	}

	ActionState* InputActionSystem::Get()
	{
		return currentState_;
	}

	void InputActionSystem::HandleMouseButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::MouseButtonDown;

		int mousebutton = eventData[P_BUTTON].GetInt();
		unsigned mouseButtonDown = eventData[P_BUTTONS].GetUInt();

		if (currentState_)
		{
			Urho3D::HashMap<unsigned, Urho3D::HashSet<Urho3D::StringHash> >::Iterator it = currentState_->mouseButtonMapping_.Find(mouseButtonDown);

			if (it != currentState_->mouseButtonMapping_.End())
			{
				Urho3D::HashSet<Urho3D::StringHash>::Iterator it2;
				for (it2 = it->second_.Begin(); it2 != it->second_.End(); it2++)
				{
					using namespace InputAction;

					Urho3D::VariantMap& neweventData = GetEventDataMap();
					neweventData[P_ACTIONID] = (*it2);
					neweventData[P_ISDOWN] = true;

					SendEvent(E_INPUTACTION, neweventData);
				}
			}

			Urho3D::HashMap<unsigned, Urho3D::HashSet<sInputAction* > >::Iterator it1 = currentState_->mouseButtonMapping2_.Find(mousebutton);

			if (it1 != currentState_->mouseButtonMapping2_.End())
			{
				Urho3D::HashSet<sInputAction* >::Iterator it3;
				for (it3 = it1->second_.Begin(); it3 != it1->second_.End(); it3++)
				{
					(*it3)->downMask_ |= MouseButton_Mask;
					if ((*it3)->IsActive())
					{
						using namespace InputAction;
						Urho3D::VariantMap& neweventData = GetEventDataMap();
						neweventData[P_ACTIONID] = (*it3)->id_;
						neweventData[P_ISDOWN] = true;

						SendEvent(E_INPUTACTION, neweventData);
					}
				}
			}
		}
	}

	void InputActionSystem::HandleMouseButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::MouseButtonUp;
		int mousebutton = eventData[P_BUTTON].GetInt();
		unsigned mouseButtonDown = eventData[P_BUTTONS].GetUInt();

		if (currentState_)
		{
			Urho3D::HashMap<unsigned, Urho3D::HashSet<Urho3D::StringHash> >::Iterator it = currentState_->mouseButtonMapping_.Find(mousebutton);

			if (it != currentState_->mouseButtonMapping_.End())
			{
				Urho3D::HashSet<Urho3D::StringHash>::Iterator it2;
				for (it2 = it->second_.Begin(); it2 != it->second_.End(); it2++)
				{
					using namespace InputAction;

					Urho3D::VariantMap& neweventData = GetEventDataMap();
					neweventData[P_ACTIONID] = (*it2);
					neweventData[P_ISDOWN] = false;

					SendEvent(E_INPUTACTION, neweventData);
				}
			}

			Urho3D::HashMap<unsigned, Urho3D::HashSet< sInputAction* > >::Iterator it1 = currentState_->mouseButtonMapping2_.Find(mousebutton);

			if (it1 != currentState_->mouseButtonMapping2_.End())
			{
				Urho3D::HashSet< sInputAction* >::Iterator it3;
				for (it3 = it1->second_.Begin(); it3 != it1->second_.End(); it3++)
				{
					(*it3)->downMask_ &= ~MouseButton_Mask;
					if (!(*it3)->IsActive())
					{
						using namespace InputAction;
						Urho3D::VariantMap& neweventData = GetEventDataMap();
						neweventData[P_ACTIONID] = (*it3)->id_;
						neweventData[P_ISDOWN] = false;

						SendEvent(E_INPUTACTION, neweventData);
					}
				}
			}
		}
	}

	void InputActionSystem::HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::MouseMove;
	}

	void InputActionSystem::HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::MouseWheel;
	}

	void InputActionSystem::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::KeyDown;
		int key = eventData[P_KEY].GetInt();
		bool repeat = eventData[P_REPEAT].GetBool();

		if (currentState_ && !repeat)
		{
			Urho3D::HashMap<int, Urho3D::HashSet<Urho3D::StringHash> >::Iterator it = currentState_->keyActionMapping_.Find(key);

			if (it != currentState_->keyActionMapping_.End())
			{
				Urho3D::HashSet<Urho3D::StringHash>::Iterator it2;
				for (it2 = it->second_.Begin(); it2 != it->second_.End(); it2++)
				{
					using namespace InputAction;

					Urho3D::VariantMap& neweventData = GetEventDataMap();
					neweventData[P_ACTIONID] = (*it2);
					neweventData[P_ISDOWN] = true;

					SendEvent(E_INPUTACTION, neweventData);
				}
			}

			Urho3D::HashMap<int, Urho3D::HashSet< sInputAction* > >::Iterator it1 = currentState_->keyActionMapping2_.Find(key);

			if (it1 != currentState_->keyActionMapping2_.End())
			{
				Urho3D::HashSet< sInputAction* >::Iterator it3;
				for (it3 = it1->second_.Begin(); it3 != it1->second_.End(); it3++)
				{
					(*it3)->downMask_ |= Key_Mask;
					if ((*it3)->IsActive())
					{
						using namespace InputAction;

						Urho3D::VariantMap& neweventData = GetEventDataMap();
						neweventData[P_ACTIONID] = (*it3)->id_;
						neweventData[P_ISDOWN] = true;

						SendEvent(E_INPUTACTION, neweventData);
					}
				}
			}
		}
	}

	void InputActionSystem::HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::KeyUp;
		int key = eventData[P_KEY].GetInt();

		if (currentState_)
		{
			Urho3D::HashMap<int, Urho3D::HashSet<Urho3D::StringHash> >::Iterator it = currentState_->keyActionMapping_.Find(key);

			if (it != currentState_->keyActionMapping_.End())
			{
				Urho3D::HashSet<Urho3D::StringHash>::Iterator it2;
				for (it2 = it->second_.Begin(); it2 != it->second_.End(); it2++)
				{
					using namespace InputAction;

					Urho3D::VariantMap& neweventData = GetEventDataMap();
					neweventData[P_ACTIONID] = (*it2);
					neweventData[P_ISDOWN] = false;

					SendEvent(E_INPUTACTION, neweventData);
				}
			}

			Urho3D::HashMap<int, Urho3D::HashSet< sInputAction* > >::Iterator it1 = currentState_->keyActionMapping2_.Find(key);

			if (it1 != currentState_->keyActionMapping2_.End())
			{
				Urho3D::HashSet< sInputAction* >::Iterator it3;
				for (it3 = it1->second_.Begin(); it3 != it1->second_.End(); it3++)
				{
					(*it3)->downMask_ &= ~Key_Mask;

					if (!(*it3)->IsActive())
					{
						using namespace InputAction;

						Urho3D::VariantMap& neweventData = GetEventDataMap();
						neweventData[P_ACTIONID] = (*it3)->id_;
						neweventData[P_ISDOWN] = false;

						SendEvent(E_INPUTACTION, neweventData);
					}
				}
			}
		}
	}

	void InputActionSystem::HandleJoystickButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::JoystickButtonDown;
	}

	void InputActionSystem::HandleJoystickButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::JoystickButtonUp;
	}

	void InputActionSystem::HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::JoystickAxisMove;
	}

	void InputActionSystem::HandleJoystickHatMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::JoystickHatMove;
	}

	void InputActionSystem::HandleTouchEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::TouchEnd;
	}

	void InputActionSystem::HandleTouchBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::TouchBegin;
	}

	void InputActionSystem::HandleTouchMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
	{
		using namespace Urho3D::TouchMove;
	}

	ActionState::ActionState(Urho3D::Context* context) : Object(context)
	{
	}

	ActionState::~ActionState()
	{
	}

	void ActionState::AddKeyAction(const Urho3D::String& action, int key)
	{
		int key_ = SDL_toupper(key);
		Urho3D::StringHash actionHash(action);
		actionNameMapping_[actionHash] = action;
		keyActionMapping_[key_].Insert(actionHash);
	}

	void ActionState::AddMouseButtonAction(const Urho3D::String& action, unsigned mouseButton)
	{
		Urho3D::StringHash actionHash(action);
		actionNameMapping_[actionHash] = action;
		mouseButtonMapping_[mouseButton].Insert(actionHash);
	}

	const Urho3D::String& ActionState::GetActionName(const Urho3D::StringHash& actionId)
	{
		Urho3D::HashMap<Urho3D::StringHash, Urho3D::String>::Iterator it = actionNameMapping_.Find(actionId);

		if (it != actionNameMapping_.End())
			return it->second_;

		return Urho3D::String::EMPTY;
	}

	void ActionState::SetName(const Urho3D::String& name)
	{
		name_ = name;
		nameHash_ = Urho3D::StringHash(name);
	}

	void ActionState::AddInputState(sInputAction* inAction)
	{
		if (inAction)
		{
			if (inAction->key_ != 0)
			{
				keyActionMapping2_[inAction->key_].Insert(inAction);
				//AddKeyAction(inAction->name_, inAction->key_);
			}

			if (inAction->mouseButtons_ != 0)
			{
				mouseButtonMapping2_[inAction->mouseButtons_].Insert(inAction);
				//AddMouseButtonAction(inAction->name_, inAction->mouseButtons_);
			}
		}
	}

	sInputAction::sInputAction(const Urho3D::String& name, int key, unsigned mouseButtons, bool repeat) : mouseButtons_(0), name_(name)
	{
		mouseButtons_ = mouseButtons;
		key_ = SDL_toupper(key);
		id_ = Urho3D::StringHash(name);
		repeat_ = repeat;
		downMask_ = 0;
		setMask_ = 0;
		if (key_ != 0)
			setMask_ |= Key_Mask;
		if (mouseButtons_ != 0)
			setMask_ |= MouseButton_Mask;
	}
}