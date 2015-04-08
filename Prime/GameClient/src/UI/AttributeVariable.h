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

/*!
 * \file AttributeVariableUI.h
 *
 * \author vitali
 * \date März 2015
 *
 * 
 */

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Core/Attribute.h>

namespace Urho3D
{
	class BorderImage;
	class ScrollBar;
	class XMLFile;
	class Editor;
	class ProjectManager;
	class ResourceCache;
	class FileSystem;
	class Window;
	class UIElement;
	class EditorSelection;
	class ResourcePickerManager;
	class ListView;
	class Serializable;
	class Text;
	class Button;
	class CheckBox;
	class LineEdit;
	class DropDownList;
}

namespace Prime
{
	namespace UI
	{
		class BasicAttributeUI;

		BasicAttributeUI* CreateAttributeUI(Urho3D::Serializable* serializable, const Urho3D::AttributeInfo& info, unsigned int index, Urho3D::XMLFile* defaultstyle, unsigned int subIndex = 0);

		class BasicAttributeUI : public Urho3D::UIElement
		{
			OBJECT(BasicAttributeUI);

		public:
			/// Construct.
			BasicAttributeUI(Urho3D::Context* context);
			/// Destruct.
			virtual ~BasicAttributeUI();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			virtual void	SetVarValue(Urho3D::Variant& var);
			virtual Urho3D::Variant GetVariant();

			void UpdateVar(Serializable* serializable);

			bool IsInUpdated(){ return inUpdated_; }

			void			SetVarName(const Urho3D::String& name);
			const Urho3D::String&	GetVarName();
			Urho3D::Text*			GetVarNameUI();

			void			SetIndex(unsigned int i) { index_ = i; }
			unsigned int	GetIndex() const { return index_; }

			void			SetSubIndex(unsigned int si){ subIndex_ = si; }
			unsigned int	GetSubIndex() const { return subIndex_; }

		protected:
			/// Attribute Index in the Serializable
			unsigned int index_;
			/// Used for VariantMap/VariantVector/ResourceList Attribute Types
			unsigned int subIndex_;
			bool inUpdated_;
			Urho3D::SharedPtr<Urho3D::Text>	varName_;
		};

		/// see the Create function to see how to use it 
		class BoolAttributeUI : public BasicAttributeUI
		{
			OBJECT(BoolAttributeUI);

		public:
			/// Construct.
			BoolAttributeUI(Urho3D::Context* context);
			/// Destruct.
			virtual ~BoolAttributeUI();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			static BoolAttributeUI* Create(Serializable* serializable, const Urho3D::String& name,
				unsigned int index, Urho3D::XMLFile* defaultstyle = NULL, unsigned int subIndex = 0);

			virtual void	SetVarValue(Urho3D::Variant& var);
			virtual Urho3D::Variant GetVariant();

			void SetVarValue(bool b);
			bool GetVarValue();
			/// get the old value, it will be updated on UpdateVar and after the Toggled event 
			bool GetOldValue();

			Urho3D::CheckBox* GetVarValueUI();

		protected:
			void HandleToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			Urho3D::SharedPtr<Urho3D::CheckBox>	varEdit_;
			bool oldValue_;
		};

		/// see the Create function to see how to use it 
		class StringAttributeUI : public BasicAttributeUI
		{
			OBJECT(BoolAttributeUI);

		public:
			/// Construct.
			StringAttributeUI(Urho3D::Context* context);
			/// Destruct.
			virtual ~StringAttributeUI();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			static StringAttributeUI* Create(Urho3D::Serializable* serializable, const Urho3D::String& name,
				unsigned int index, Urho3D::XMLFile* defaultstyle = NULL, unsigned int subIndex = 0);

			virtual void SetVarValue(Urho3D::Variant& var);
			virtual Urho3D::Variant GetVariant();

			void SetVarValue(const Urho3D::String& b);
			const Urho3D::String& GetVarValue();
			/// get the old value, it will be updated on UpdateVar and after the TextChange events
			const Urho3D::String& GetOldValue();

			Urho3D::LineEdit* GetVarValueUI();

		protected:
			void HandleTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			Urho3D::SharedPtr<Urho3D::LineEdit>	varEdit_;
			Urho3D::String oldValue_;

		};

		/// see the Create function to see how to use it 
		class NumberAttributeUI : public BasicAttributeUI
		{
			OBJECT(NumberAttributeUI);

		public:
			/// Construct.
			NumberAttributeUI(Urho3D::Context* context);
			/// Destruct.
			virtual ~NumberAttributeUI();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			static NumberAttributeUI* Create(Serializable* serializable, const Urho3D::String& name,
				unsigned int index, Urho3D::VariantType type, Urho3D::XMLFile* defaultstyle = NULL, unsigned int subIndex = 0);

			virtual void SetVarValue(Urho3D::Variant& var);
			virtual Urho3D::Variant GetVariant();

			void SetType(Urho3D::VariantType type);

			Urho3D::Variant		GetVarValue();
			/// get the old value, it will be updated on UpdateVar and after the TextChange events
			Urho3D::Variant		GetOldValue();

			const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::LineEdit> >& GetVarValueUI();
			int			GetNumCoords() { return numCoords_; }
			Urho3D::VariantType GetType() { return type_; }
		protected:
			void HandleTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			Urho3D::Vector<Urho3D::SharedPtr<Urho3D::LineEdit> >	varEdit_;
			int numCoords_;
			Urho3D::VariantType type_;
			Urho3D::Variant oldValue_;
		};

		/// see the Create function to see how to use it 
		class EnumAttributeUI : public BasicAttributeUI
		{
			OBJECT(EnumAttributeUI);

		public:
			/// Construct.
			EnumAttributeUI(Urho3D::Context* context);
			/// Destruct.
			virtual ~EnumAttributeUI();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			static EnumAttributeUI* Create(Serializable* serializable, const Urho3D::String& name, unsigned int index, const Urho3D::Vector<Urho3D::String>& enums, Urho3D::XMLFile* defaultstyle);

			virtual void	SetVarValue(Urho3D::Variant& var);
			virtual Urho3D::Variant GetVariant();

			int		GetVarValue();
			/// get the old value, it will be updated on UpdateVar and after the TextChange events
			int		GetOldValue();

			Urho3D::DropDownList*	GetVarValueUI();
			void			SetEnumNames(const Urho3D::Vector<Urho3D::String>& enums);
		protected:
			void HandleItemSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			Urho3D::SharedPtr<Urho3D::DropDownList> 	varEdit_;
			Urho3D::Vector<Urho3D::String> enumNames_;
			int oldValue_;
		};

		/// see the Create function to see how to use it 
		class ResourceRefAttributeUI : public BasicAttributeUI
		{
			OBJECT(ResourceRefAttributeUI);

		public:
			/// Construct.
			ResourceRefAttributeUI(Urho3D::Context* context);
			/// Destruct.
			virtual ~ResourceRefAttributeUI();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			static ResourceRefAttributeUI* Create(Urho3D::Serializable* serializable, const Urho3D::String& name, Urho3D::VariantType type, Urho3D::StringHash resourceType, unsigned int index, unsigned int subindex, Urho3D::XMLFile* defaultstyle, unsigned int action = 0);

			virtual void	SetVarValue(Urho3D::Variant& var);
			virtual Urho3D::Variant GetVariant();

			Urho3D::LineEdit* GetVarValueUI();

			void			SetActions(unsigned int action);
			unsigned int	GetActions() { return action_; }

			void		SetType(Urho3D::VariantType type);
			Urho3D::VariantType GetType() { return type_; }

			void		SetResourceType(Urho3D::StringHash restype) { resType_ = restype; }
			Urho3D::StringHash	GetResourceType() { return resType_; }

			void SetVarValue(const Urho3D::String& b);
			const Urho3D::String& GetVarValue();
			/// get the old value, it will be updated on UpdateVar and after the TextChange events
			const Urho3D::String& GetOldValue();
		protected:
			void HandleTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandlePick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleOpen(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleTest(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			Urho3D::Button* CreateResourcePickerButton(Urho3D::UIElement* container, const Urho3D::String& text);

			Urho3D::SharedPtr<Urho3D::LineEdit> 	varEdit_;
			Urho3D::SharedPtr<UIElement>	container_;
			Urho3D::Vector<Urho3D::String>			enumNames_;
			Urho3D::VariantType			type_;
			Urho3D::SharedPtr<Urho3D::Button> 	pick_;
			Urho3D::SharedPtr<Urho3D::Button>	open_;
			Urho3D::SharedPtr<Urho3D::Button> 	edit_;
			Urho3D::SharedPtr<Urho3D::Button>	test_;
			unsigned int	action_;
			Urho3D::StringHash		resType_;
			Urho3D::String			oldValue_;
		};

	}
}