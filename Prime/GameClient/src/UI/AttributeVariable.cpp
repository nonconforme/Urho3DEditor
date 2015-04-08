//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "AttributeVariable.h"
#include "AttributeVariableEvents.h"
#include "UIGlobals.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/ScrollBar.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/DebugNew.h>

// Resource picker functionality
const unsigned int  ACTION_PICK = 1;
const unsigned int  ACTION_OPEN = 2;
const unsigned int  ACTION_EDIT = 4;
const unsigned int  ACTION_TEST = 8;

namespace Prime
{
	namespace UI
	{

		BasicAttributeUI* CreateAttributeUI(Urho3D::Serializable* serializable, const Urho3D::AttributeInfo& info, unsigned int index, Urho3D::XMLFile* defaultstyle, unsigned int subIndex)
		{
			if (!serializable)
				return NULL;

			if (index >= serializable->GetNumAttributes())
				return NULL;
			BasicAttributeUI* attrui = NULL;

			Urho3D::VariantType type = info.type_;
			if (type == Urho3D::VAR_STRING || type == Urho3D::VAR_BUFFER)
			{
				StringAttributeUI* attr = StringAttributeUI::Create(serializable, info.name_, index, defaultstyle, subIndex);
				attrui = attr;
			}
			else if (type == Urho3D::VAR_BOOL)
			{
				BoolAttributeUI* attr = BoolAttributeUI::Create(serializable, info.name_, index, defaultstyle, subIndex);
				attrui = attr;
			}
			else if ((type >= Urho3D::VAR_FLOAT && type <= Urho3D::VAR_VECTOR4) || type == Urho3D::VAR_QUATERNION || type == Urho3D::VAR_COLOR || type == Urho3D::VAR_INTVECTOR2 || type == Urho3D::VAR_INTRECT)
			{
				NumberAttributeUI* attr = NumberAttributeUI::Create(serializable, info.name_, index, type, defaultstyle, subIndex);

				attrui = attr;
			}
			else if (type == Urho3D::VAR_INT)
			{
				// get  enums names
				if (info.enumNames_ != NULL)
				{
					Urho3D::Vector<Urho3D::String> enumnames;
					const char** enumPtr = info.enumNames_;
					while (*enumPtr)
					{
						enumnames.Push(Urho3D::String(*enumPtr));
						++enumPtr;
					}

					EnumAttributeUI* attr = EnumAttributeUI::Create(serializable, info.name_, index, enumnames, defaultstyle);
					attrui = attr;
				}
				else
				{
					NumberAttributeUI* attr = NumberAttributeUI::Create(serializable, info.name_, index, type, defaultstyle, subIndex);
					attrui = attr;
				}
			}
			else if (type == Urho3D::VAR_RESOURCEREF)
			{
				Urho3D::StringHash resourceType;

				// Get the real attribute info from the serializable for the correct resource type
				Urho3D::AttributeInfo attrInfo = serializable->GetAttributes()->At(index);
				if (attrInfo.type_ == Urho3D::VAR_RESOURCEREF)
					resourceType = serializable->GetAttribute(index).GetResourceRef().type_;
				else if (attrInfo.type_ == Urho3D::VAR_RESOURCEREFLIST)
					resourceType = serializable->GetAttribute(index).GetResourceRefList().type_;
				else if (attrInfo.type_ == Urho3D::VAR_VARIANTVECTOR)
					resourceType = serializable->GetAttribute(index).GetVariantVector()[subIndex].GetResourceRef().type_;

				ResourceRefAttributeUI* attr = ResourceRefAttributeUI::Create(serializable, info.name_, attrInfo.type_,
					resourceType, index, subIndex, defaultstyle, 0);
				attrui = attr;
			}
			else if (type == Urho3D::VAR_RESOURCEREFLIST)
			{
			}
			else if (type == Urho3D::VAR_VARIANTVECTOR)
			{
			}
			else if (type == Urho3D::VAR_VARIANTMAP)
			{
			}

			return attrui;
		}

		//////////////////////////////////////////////////////////////////////////
		/// BasicAttributeUI
		//////////////////////////////////////////////////////////////////////////
		void BasicAttributeUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<BoolAttributeUI>();
			COPY_BASE_ATTRIBUTES(UIElement);
		}

		BasicAttributeUI::BasicAttributeUI(Urho3D::Context* context) : UIElement(context)
		{
			inUpdated_ = false;
			index_ = 0;
			subIndex_ = 0;
			SetEnabled(true);

			SetLayout(Urho3D::LM_HORIZONTAL);
			SetFixedHeight(19);

			varName_ = CreateChild<Urho3D::Text>("A_VarName");
			varName_->SetInternal(true);
			varName_->SetStyle("EditorAttributeText");
			varName_->SetText("Variable");
			varName_->SetFixedWidth(150);
		}

		BasicAttributeUI::~BasicAttributeUI()
		{
		}

		void BasicAttributeUI::UpdateVar(Serializable* serializable)
		{
			if (!serializable)
				return;
			inUpdated_ = true;
			Urho3D::Variant var = serializable->GetAttribute(index_);
			if (var.GetType() == Urho3D::VAR_VARIANTMAP)
			{
				Urho3D::VariantMap map = var.GetVariantMap();
				Urho3D::Vector<Urho3D::StringHash> keys = map.Keys();

				var = map[keys[subIndex_]];
			}

			SetVarValue(var);
			inUpdated_ = false;
		}

		void BasicAttributeUI::SetVarName(const Urho3D::String& name)
		{
			varName_->SetText(name);
		}

		const Urho3D::String& BasicAttributeUI::GetVarName()
		{
			return varName_->GetText();
		}

		void BasicAttributeUI::SetVarValue(Urho3D::Variant& var)
		{
		}

		Urho3D::Variant BasicAttributeUI::GetVariant()
		{
			return Urho3D::Variant::EMPTY;
		}

		Urho3D::Text* BasicAttributeUI::GetVarNameUI()
		{
			return varName_;
		}

		//////////////////////////////////////////////////////////////////////////
		/// BoolAttributeUI
		//////////////////////////////////////////////////////////////////////////
		void BoolAttributeUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<BoolAttributeUI>();
			COPY_BASE_ATTRIBUTES(BasicAttributeUI);
		}

		BoolAttributeUI::~BoolAttributeUI()
		{
		}

		BoolAttributeUI::BoolAttributeUI(Urho3D::Context* context) : BasicAttributeUI(context)
		{
			varEdit_ = CreateChild<Urho3D::CheckBox>("A_VarValue");
			varEdit_->SetInternal(true);
			SubscribeToEvent(varEdit_, Urho3D::E_TOGGLED, HANDLER(BoolAttributeUI, HandleToggled));
			oldValue_ = false;
		}

		void BoolAttributeUI::SetVarValue(bool b)
		{
			varEdit_->SetChecked(b);
		}

		void BoolAttributeUI::SetVarValue(Urho3D::Variant& var)
		{
			if (var.GetType() == Urho3D::VAR_BOOL)
			{
				varEdit_->SetChecked(var.GetBool());
				oldValue_ = var.GetBool();
			}
		}

		bool BoolAttributeUI::GetVarValue()
		{
			return varEdit_->IsChecked();
		}

		BoolAttributeUI* BoolAttributeUI::Create(Serializable* serializable, const Urho3D::String& name, unsigned int index, Urho3D::XMLFile* defaultstyle, unsigned int subIndex)
		{
			if (!serializable)
				return NULL;
			BoolAttributeUI* boolattr = new BoolAttributeUI(serializable->GetContext());
			boolattr->SetIndex(index);
			boolattr->SetSubIndex(subIndex);
			boolattr->SetVarName(name);
			if (defaultstyle)
			{
				boolattr->SetDefaultStyle(defaultstyle);
				boolattr->SetStyle("BoolAttributeUI");
			}
			boolattr->UpdateVar(serializable);
			return boolattr;
		}

		Urho3D::CheckBox* BoolAttributeUI::GetVarValueUI()
		{
			return varEdit_;
		}

		void BoolAttributeUI::HandleToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (inUpdated_)
				return;
			using namespace BoolVarChanged;
			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_BOOLVARCHANGED, eventData_);

			oldValue_ = varEdit_->IsChecked();
		}

		bool BoolAttributeUI::GetOldValue()
		{
			return oldValue_;
		}

		Urho3D::Variant BoolAttributeUI::GetVariant()
		{
			return Urho3D::Variant(varEdit_->IsChecked());
		}

		//////////////////////////////////////////////////////////////////////////
		/// StringAttributeUI
		//////////////////////////////////////////////////////////////////////////
		StringAttributeUI::StringAttributeUI(Urho3D::Context* context) : BasicAttributeUI(context)
		{
			varEdit_ = CreateChild<Urho3D::LineEdit>("A_VarValue");
			varEdit_->SetInternal(true);
			varEdit_->SetFixedHeight(17);
			varEdit_->SetDragDropMode(Urho3D::DD_TARGET);

			// Do not subscribe to continuous edits of certain attributes (script class names) to prevent unnecessary errors getting printed

			SubscribeToEvent(varEdit_, Urho3D::E_TEXTCHANGED, HANDLER(StringAttributeUI, HandleTextChange));
			SubscribeToEvent(varEdit_, Urho3D::E_TEXTFINISHED, HANDLER(StringAttributeUI, HandleTextChange));
		}

		StringAttributeUI::~StringAttributeUI()
		{
		}

		void StringAttributeUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<StringAttributeUI>();
			COPY_BASE_ATTRIBUTES(BasicAttributeUI);
		}

		StringAttributeUI* StringAttributeUI::Create(Serializable* serializable, const Urho3D::String& name, unsigned int index, Urho3D::XMLFile* defaultstyle, unsigned int subIndex)
		{
			if (!serializable)
				return NULL;
			StringAttributeUI* attr = new StringAttributeUI(serializable->GetContext());
			attr->SetIndex(index);
			attr->SetSubIndex(subIndex);
			attr->SetVarName(name);
			if (defaultstyle)
			{
				attr->SetDefaultStyle(defaultstyle);
				attr->SetStyle("StringAttributeUI");
			}
			attr->UpdateVar(serializable);
			return attr;
		}

		void StringAttributeUI::SetVarValue(Urho3D::Variant& var)
		{
			if (var.GetType() == Urho3D::VAR_STRING)
			{
				varEdit_->SetText(var.GetString());
				oldValue_ = var.GetString();
				varEdit_->SetCursorPosition(0);
			}
			else if (var.GetType() == Urho3D::VAR_BUFFER)
			{
				///  \todo
				varEdit_->SetText("VAR_BUFFER");
				varEdit_->SetCursorPosition(0);
			}
		}

		void StringAttributeUI::SetVarValue(const Urho3D::String& b)
		{
			varEdit_->SetText(b);
		}

		const Urho3D::String& StringAttributeUI::GetVarValue()
		{
			return varEdit_->GetText();
		}

		Urho3D::LineEdit* StringAttributeUI::GetVarValueUI()
		{
			return varEdit_;
		}

		void StringAttributeUI::HandleTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (inUpdated_)
				return;
			using namespace StringVarChanged;
			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_STRINGVARCHANGED, eventData_);
			oldValue_ = varEdit_->GetText();
		}

		const Urho3D::String& StringAttributeUI::GetOldValue()
		{
			return oldValue_;
		}

		Urho3D::Variant StringAttributeUI::GetVariant()
		{
			return Urho3D::Variant(varEdit_->GetText());
		}

		//////////////////////////////////////////////////////////////////////////
		/// NumberAttributeUI
		//////////////////////////////////////////////////////////////////////////
		NumberAttributeUI::NumberAttributeUI(Urho3D::Context* context) : BasicAttributeUI(context)
		{
			type_ = Urho3D::VAR_NONE;
		}

		NumberAttributeUI::~NumberAttributeUI()
		{
		}

		void NumberAttributeUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<NumberAttributeUI>();
			COPY_BASE_ATTRIBUTES(BasicAttributeUI);
		}

		NumberAttributeUI* NumberAttributeUI::Create(Serializable* serializable, const Urho3D::String& name, unsigned int index, Urho3D::VariantType type, Urho3D::XMLFile* defaultstyle, unsigned int subIndex)
		{
			if (!serializable)
				return NULL;

			NumberAttributeUI* attr = new NumberAttributeUI(serializable->GetContext());
			attr->SetIndex(index);
			attr->SetSubIndex(subIndex);
			attr->SetVarName(name);
			attr->SetType(type);
			if (defaultstyle)
			{
				attr->SetDefaultStyle(defaultstyle);
				attr->SetStyle("BasicAttributeUI");
				for (int i = 0; i < attr->GetNumCoords(); ++i)
				{
					attr->GetVarValueUI()[i]->SetStyle("EditorAttributeEdit");
					attr->GetVarValueUI()[i]->SetMinWidth(32);
				}
			}

			attr->UpdateVar(serializable);
			return attr;
		}

		void NumberAttributeUI::SetVarValue(Urho3D::Variant& var)
		{
			if (type_ != var.GetType())
				return;

			oldValue_ = var;

			if (type_ == Urho3D::VAR_INT)
			{
				varEdit_[0]->SetText(Urho3D::String(var.GetInt()));
				varEdit_[0]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_FLOAT)
			{
				varEdit_[0]->SetText(Urho3D::String(var.GetFloat()));
				varEdit_[0]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_VECTOR2)
			{
				const Urho3D::Vector2& v = var.GetVector2();

				varEdit_[0]->SetText(Urho3D::String(v.x_));
				varEdit_[1]->SetText(Urho3D::String(v.y_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_VECTOR3)
			{
				const Urho3D::Vector3& v = var.GetVector3();

				varEdit_[0]->SetText(Urho3D::String(v.x_));
				varEdit_[1]->SetText(Urho3D::String(v.y_));
				varEdit_[2]->SetText(Urho3D::String(v.z_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
				varEdit_[2]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_VECTOR4)
			{
				const Urho3D::Vector4& v = var.GetVector4();

				varEdit_[0]->SetText(Urho3D::String(v.x_));
				varEdit_[1]->SetText(Urho3D::String(v.y_));
				varEdit_[2]->SetText(Urho3D::String(v.z_));
				varEdit_[3]->SetText(Urho3D::String(v.w_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
				varEdit_[2]->SetCursorPosition(0);
				varEdit_[3]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_QUATERNION)
			{
				const Urho3D::Quaternion& q = var.GetQuaternion();
				Urho3D::Vector3 euler = q.EulerAngles();
				varEdit_[0]->SetText(Urho3D::String(euler.x_));
				varEdit_[1]->SetText(Urho3D::String(euler.y_));
				varEdit_[2]->SetText(Urho3D::String(euler.z_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
				varEdit_[2]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_COLOR)
			{
				const Urho3D::Color& v = var.GetColor();

				varEdit_[0]->SetText(Urho3D::String(v.r_));
				varEdit_[1]->SetText(Urho3D::String(v.g_));
				varEdit_[2]->SetText(Urho3D::String(v.b_));
				varEdit_[3]->SetText(Urho3D::String(v.a_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
				varEdit_[2]->SetCursorPosition(0);
				varEdit_[3]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_INTVECTOR2)
			{
				const Urho3D::IntVector2& v = var.GetIntVector2();

				varEdit_[0]->SetText(Urho3D::String(v.x_));
				varEdit_[1]->SetText(Urho3D::String(v.y_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
			}
			else if (type_ == Urho3D::VAR_INTRECT)
			{
				const Urho3D::IntRect& v = var.GetIntRect();

				varEdit_[0]->SetText(Urho3D::String(v.left_));
				varEdit_[1]->SetText(Urho3D::String(v.top_));
				varEdit_[2]->SetText(Urho3D::String(v.right_));
				varEdit_[3]->SetText(Urho3D::String(v.bottom_));
				varEdit_[0]->SetCursorPosition(0);
				varEdit_[1]->SetCursorPosition(0);
				varEdit_[2]->SetCursorPosition(0);
				varEdit_[3]->SetCursorPosition(0);
			}
		}

		Urho3D::Variant NumberAttributeUI::GetVarValue()
		{
			if (type_ == Urho3D::VAR_INT)
			{
				return Urho3D::Variant(ToInt(varEdit_[0]->GetText()));
			}
			else if (type_ == Urho3D::VAR_FLOAT)
			{
				return Urho3D::Variant(ToFloat(varEdit_[0]->GetText()));
			}
			else if (type_ == Urho3D::VAR_VECTOR2)
			{
				return Urho3D::Variant(ToVector2(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText()));
			}
			else if (type_ == Urho3D::VAR_VECTOR3)
			{
				return Urho3D::Variant(ToVector3(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText() + " " + varEdit_[2]->GetText()));
			}
			else if (type_ == Urho3D::VAR_VECTOR4)
			{
				return Urho3D::Variant(ToVector4(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText() + " " + varEdit_[2]->GetText() + " " + varEdit_[3]->GetText()));
			}
			else if (type_ == Urho3D::VAR_QUATERNION)
			{
				return Urho3D::Variant(ToQuaternion(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText() + " " + varEdit_[2]->GetText()));
			}
			else if (type_ == Urho3D::VAR_COLOR)
			{
				return Urho3D::Variant(ToColor(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText() + " " + varEdit_[2]->GetText() + " " + varEdit_[3]->GetText()));
			}
			else if (type_ == Urho3D::VAR_INTVECTOR2)
			{
				return Urho3D::Variant(ToIntVector2(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText()));
			}
			else if (type_ == Urho3D::VAR_INTRECT)
			{
				return Urho3D::Variant(ToIntRect(varEdit_[0]->GetText() + " " + varEdit_[1]->GetText() + " " + varEdit_[2]->GetText() + " " + varEdit_[3]->GetText()));
			}
			return Urho3D::Variant();
		}

		const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::LineEdit> >& NumberAttributeUI::GetVarValueUI()
		{
			return varEdit_;
		}

		void NumberAttributeUI::SetType(Urho3D::VariantType type)
		{
			/// if VAR_NONE remove all
			if (type == Urho3D::VAR_NONE)
			{
				varEdit_.Clear();
				numCoords_ = 0;
				type_ = type;
				return;
			}

			/// if same type, dont do anything
			if (type_ == type)
				return;

			type_ = type;

			if (type_ == Urho3D::VAR_INT)
			{
				varEdit_.Clear();
				numCoords_ = 1;
				Urho3D::LineEdit* attrEdit = CreateChild<Urho3D::LineEdit>("A_VarValue");

				attrEdit->SetStyle("EditorAttributeEdit");
				attrEdit->SetFixedHeight(17);
				attrEdit->SetVar(ATTR_COORDINATE, 1);
				SubscribeToEvent(attrEdit, Urho3D::E_TEXTCHANGED, HANDLER(NumberAttributeUI, HandleTextChange));
				SubscribeToEvent(attrEdit, Urho3D::E_TEXTFINISHED, HANDLER(NumberAttributeUI, HandleTextChange));
				varEdit_.Push(Urho3D::SharedPtr<Urho3D::LineEdit>(attrEdit));
				return;
			}

			unsigned int numCoords = type - Urho3D::VAR_FLOAT + 1;
			if (type == Urho3D::VAR_QUATERNION)
				numCoords = 3;
			else if (type == Urho3D::VAR_COLOR || type == Urho3D::VAR_INTRECT)
				numCoords = 4;
			else if (type == Urho3D::VAR_INTVECTOR2)
				numCoords = 2;

			varEdit_.Clear();
			numCoords_ = numCoords;
			for (unsigned int i = 0; i < numCoords; ++i)
			{
				Urho3D::LineEdit* attrEdit = CreateChild<Urho3D::LineEdit>("A_VarValue_" + Urho3D::String(i));

				attrEdit->SetStyle("EditorAttributeEdit");
				attrEdit->SetFixedHeight(17);
				attrEdit->SetVar(ATTR_COORDINATE, i);
				SubscribeToEvent(attrEdit, Urho3D::E_TEXTCHANGED, HANDLER(NumberAttributeUI, HandleTextChange));
				SubscribeToEvent(attrEdit, Urho3D::E_TEXTFINISHED, HANDLER(NumberAttributeUI, HandleTextChange));
				varEdit_.Push(Urho3D::SharedPtr<Urho3D::LineEdit>(attrEdit));
			}
		}

		void NumberAttributeUI::HandleTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (inUpdated_)
				return;
			using namespace NumberVarChanged;
			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_NUMBERVARCHANGED, eventData_);
			oldValue_ = GetVarValue();
		}

		Urho3D::Variant NumberAttributeUI::GetOldValue()
		{
			return oldValue_;
		}

		Urho3D::Variant NumberAttributeUI::GetVariant()
		{
			return GetVarValue();
		}

		//////////////////////////////////////////////////////////////////////////
		/// EnumAttributeUI
		//////////////////////////////////////////////////////////////////////////
		EnumAttributeUI::EnumAttributeUI(Urho3D::Context* context) : BasicAttributeUI(context)
		{
			varEdit_ = CreateChild<Urho3D::DropDownList>("A_VarValue");

			varEdit_->SetInternal(true);
			varEdit_->SetFixedHeight(17);
			varEdit_->SetResizePopup(true);
			varEdit_->SetPlaceholderText("--");
			varEdit_->SetLayout(Urho3D::LM_HORIZONTAL, 0, Urho3D::IntRect(4, 1, 4, 1));
			SubscribeToEvent(varEdit_, Urho3D::E_ITEMSELECTED, HANDLER(EnumAttributeUI, HandleItemSelected));
			oldValue_ = 0;
		}

		EnumAttributeUI::~EnumAttributeUI()
		{
		}

		void EnumAttributeUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<EnumAttributeUI>();
			COPY_BASE_ATTRIBUTES(BasicAttributeUI);
		}

		EnumAttributeUI* EnumAttributeUI::Create(Serializable* serializable, const Urho3D::String& name, unsigned int index, const Urho3D::Vector<Urho3D::String>& enums, Urho3D::XMLFile* defaultstyle)
		{
			if (!serializable)
				return NULL;
			EnumAttributeUI* attr = new EnumAttributeUI(serializable->GetContext());
			attr->SetIndex(index);
			attr->SetVarName(name);
			if (defaultstyle)
			{
				attr->SetDefaultStyle(defaultstyle);
				attr->SetStyle("EnumAttributeUI");
			}
			attr->SetEnumNames(enums);
			attr->UpdateVar(serializable);
			return attr;
		}

		void EnumAttributeUI::SetVarValue(Urho3D::Variant& var)
		{
			if (var.GetType() == Urho3D::VAR_INT)
			{
				varEdit_->SetSelection(var.GetUInt());
				oldValue_ = var.GetInt();
			}
		}

		Urho3D::DropDownList* EnumAttributeUI::GetVarValueUI()
		{
			return varEdit_;
		}

		void EnumAttributeUI::SetEnumNames(const Urho3D::Vector<Urho3D::String>& enums)
		{
			enumNames_ = enums;
			varEdit_->RemoveAllItems();

			for (unsigned int i = 0; i < enums.Size(); i++)
			{
				Urho3D::Text* choice = new Urho3D::Text(context_);
				varEdit_->AddItem(choice);
				choice->SetStyle("EditorEnumAttributeText");
				choice->SetText(enums[i]);
			}
		}

		void EnumAttributeUI::HandleItemSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (inUpdated_)
				return;
			using namespace EnumVarChanged;

			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_ENUMVARCHANGED, eventData_);

			oldValue_ = varEdit_->GetSelection();
		}

		int EnumAttributeUI::GetVarValue()
		{
			return varEdit_->GetSelection();
		}

		int EnumAttributeUI::GetOldValue()
		{
			return oldValue_;
		}

		Urho3D::Variant EnumAttributeUI::GetVariant()
		{
			return Urho3D::Variant(varEdit_->GetSelection());
		}

		ResourceRefAttributeUI::ResourceRefAttributeUI(Urho3D::Context* context) : BasicAttributeUI(context)
		{
			SetLayout(Urho3D::LM_VERTICAL, 2);
			container_ = CreateChild<UIElement>();

			container_->SetInternal(true);
			container_->SetLayout(Urho3D::LM_HORIZONTAL, 4, Urho3D::IntRect(10, 0, 4, 0));    // Left margin is indented more when the name is so
			container_->SetHeight(19);

			varEdit_ = container_->CreateChild<Urho3D::LineEdit>("A_VarValue");
			varEdit_->SetInternal(true);
			varEdit_->SetFixedHeight(17);
			varEdit_->SetDragDropMode(Urho3D::DD_TARGET);

			SubscribeToEvent(varEdit_, Urho3D::E_TEXTFINISHED, HANDLER(ResourceRefAttributeUI, HandleTextChange));
			//SubscribeToEvent(varEdit_, E_TEXTCHANGED, HANDLER(ResourceRefAttributeUI, HandleTextChange));

		}

		ResourceRefAttributeUI::~ResourceRefAttributeUI()
		{
		}

		void ResourceRefAttributeUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<ResourceRefAttributeUI>();
			COPY_BASE_ATTRIBUTES(BasicAttributeUI);
		}

		ResourceRefAttributeUI* ResourceRefAttributeUI::Create(Serializable* serializable, const Urho3D::String& name, Urho3D::VariantType type, Urho3D::StringHash resourceType, unsigned int index, unsigned int subindex, Urho3D::XMLFile* defaultstyle, unsigned int action)
		{
			if (!serializable)
				return NULL;
			ResourceRefAttributeUI* attr = new ResourceRefAttributeUI(serializable->GetContext());
			attr->SetDefaultStyle(defaultstyle);
			attr->SetIndex(index);
			attr->SetType(type);
			attr->SetSubIndex(subindex);
			attr->SetResourceType(resourceType);
			attr->SetVarName(name);
			attr->SetStyle("ResourceRefAttributeUI");
			attr->SetActions(action);
			attr->UpdateVar(serializable);
			attr->SetFixedHeight(36);
			return attr;
		}

		void ResourceRefAttributeUI::SetVarValue(Urho3D::Variant& var)
		{
			if (type_ != var.GetType())
				return;

			if (var.GetType() == Urho3D::VAR_RESOURCEREF)
			{
				varEdit_->SetText(var.GetResourceRef().name_);
				varEdit_->SetCursorPosition(0);
				oldValue_ = var.GetResourceRef().name_;
			}
			else if (var.GetType() == Urho3D::VAR_RESOURCEREFLIST)
			{
				varEdit_->SetText(var.GetResourceRefList().names_[subIndex_]);
				varEdit_->SetCursorPosition(0);
				oldValue_ = var.GetResourceRefList().names_[subIndex_];
			}
		}

		void ResourceRefAttributeUI::SetVarValue(const Urho3D::String& b)
		{
			varEdit_->SetText(b);
		}

		Urho3D::Variant ResourceRefAttributeUI::GetVariant()
		{
			if (type_ == Urho3D::VAR_RESOURCEREF)
			{
				Urho3D::ResourceRef ref;
				ref.name_ = varEdit_->GetText().Trimmed();
				ref.type_ = resType_;
				return Urho3D::Variant(ref);
			}
			else if (type_ == Urho3D::VAR_RESOURCEREFLIST)
			{
				Urho3D::ResourceRefList ref;
				ref.type_ = resType_;
				ref.names_.Push(varEdit_->GetText().Trimmed());
				return Urho3D::Variant(ref);
			}

			return Urho3D::Variant::EMPTY;
		}

		Urho3D::LineEdit* ResourceRefAttributeUI::GetVarValueUI()
		{
			return varEdit_;
		}

		void ResourceRefAttributeUI::HandleTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (inUpdated_)
				return;
			using namespace ResourceRefVarChanged;

			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_RESREFVARCHANGED, eventData_);

			oldValue_ = varEdit_->GetText();
		}

		void ResourceRefAttributeUI::SetActions(unsigned int action)
		{
			if (action != action_)
			{
				if (pick_.NotNull())
				{
					pick_->Remove();
					pick_.Reset();
				}
				if (open_.NotNull())
				{
					open_->Remove();
					open_.Reset();
				}
				if (edit_.NotNull())
				{
					edit_->Remove();
					edit_.Reset();
				}
				if (test_.NotNull())
				{
					test_->Remove();
					test_.Reset();
				}
			}

			action_ = action;

			if ((action_ & ACTION_PICK) != 0)
			{
				pick_ = CreateResourcePickerButton(container_, "Pick");
				SubscribeToEvent(pick_, Urho3D::E_RELEASED, HANDLER(ResourceRefAttributeUI, HandlePick));
			}
			if ((action_ & ACTION_OPEN) != 0)
			{
				open_ = CreateResourcePickerButton(container_, "Open");
				SubscribeToEvent(open_, Urho3D::E_RELEASED, HANDLER(ResourceRefAttributeUI, HandleOpen));
			}
			if ((action_ & ACTION_EDIT) != 0)
			{
				edit_ = CreateResourcePickerButton(container_, "Edit");
				SubscribeToEvent(edit_, Urho3D::E_RELEASED, HANDLER(ResourceRefAttributeUI, HandleEdit));
			}
			if ((action_ & ACTION_TEST) != 0)
			{
				test_ = CreateResourcePickerButton(container_, "Test");
				SubscribeToEvent(test_, Urho3D::E_RELEASED, HANDLER(ResourceRefAttributeUI, HandleTest));
			}
		}

		Urho3D::Button* ResourceRefAttributeUI::CreateResourcePickerButton(UIElement* container, const Urho3D::String& text)
		{
			Urho3D::Button* button = new Urho3D::Button(context_);
			container->AddChild(button);
			button->SetStyleAuto();
			button->SetFixedSize(36, 17);

			Urho3D::Text* buttonText = new Urho3D::Text(context_);
			button->AddChild(buttonText);
			buttonText->SetStyle("EditorAttributeText");
			buttonText->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
			buttonText->SetText(text);

			return button;
		}

		void ResourceRefAttributeUI::HandlePick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace PickResource;

			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_PICKRESOURCE, eventData_);
		}

		void ResourceRefAttributeUI::HandleOpen(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace OpenResource;

			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_OPENRESOURCE, eventData_);
		}

		void ResourceRefAttributeUI::HandleEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace EditResource;

			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_EDITRESOURCE, eventData_);
		}

		void ResourceRefAttributeUI::HandleTest(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace TestResource;

			Urho3D::VariantMap& eventData_ = GetEventDataMap();
			eventData_[P_ATTEDIT] = this;
			SendEvent(AEE_TESTRESOURCE, eventData_);
		}

		void ResourceRefAttributeUI::SetType(Urho3D::VariantType type)
		{
			type_ = type;
		}

		const Urho3D::String& ResourceRefAttributeUI::GetVarValue()
		{
			return  varEdit_->GetText();
		}

		const Urho3D::String& ResourceRefAttributeUI::GetOldValue()
		{
			return oldValue_;
		}
	}
}