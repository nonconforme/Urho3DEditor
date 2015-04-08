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
#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Core/Attribute.h>
#include <Urho3D/Core/Object.h>

namespace Urho3D
{
	class XMLFile;
	class ResourceCache;
	class FileSystem;
	class Resource;

	class BorderImage;
	class ScrollBar;
	class Window;
	class UIElement;
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
		class EditorResourcePicker;
		class ResourcePickerManager;

		class AttributeContainer : public Urho3D::UIElement
		{
			OBJECT(AttributeContainer);

		public:
			/// Construct.
			AttributeContainer(Urho3D::Context* context);
			/// Destruct.
			virtual ~AttributeContainer();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);
			/// React to resize.
			virtual void OnResize();

			void SetTitle(const Urho3D::String& title);
			void SetIcon(Urho3D::XMLFile* iconStyle_, const Urho3D::String& iconType);

			void SetNoTextChangedAttrs(const Urho3D::Vector<Urho3D::String>& noTextChangedAttrs);

			void SetSerializableAttributes(Urho3D::Serializable* serializable, bool createNew = false);

			Urho3D::ListView*	GetAttributeList();

			void UpdateVariantMap(Urho3D::Serializable* serializable);
			void UpdateVariantMap(Urho3D::Serializable* serializable, unsigned int index);

			Urho3D::Serializable*	GetSerializable();
			Urho3D::Button*			GetResetToDefault() { return resetToDefault_; }
		protected:

			void CreateSerializableAttributes(Serializable* serializable);
			void UpdateSerializableAttributes(Serializable* serializable);

			Urho3D::UIElement*	CreateAttribute(Serializable* serializable, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex, bool suppressedSeparatedLabel = false);
			void		UpdateAttribute(Serializable* serializable, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex, bool suppressedSeparatedLabel = false);

			Urho3D::String	GetVariableName(Serializable* serializable, Urho3D::StringHash hash);

			void EditEnabledAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void EditStringAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void EditBoolAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void EditEnumAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void EditNumberAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void EditResRefAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// UI Attributes
			Urho3D::SharedPtr<Urho3D::Text>			titleText_;
			Urho3D::SharedPtr<Urho3D::UIElement>	iconsPanel_;
			Urho3D::SharedPtr<Urho3D::UIElement>	spacer_;
			Urho3D::SharedPtr<Urho3D::Button>		resetToDefault_;
			Urho3D::SharedPtr<Urho3D::ListView>		attributeList_;
			Urho3D::Vector< Urho3D::Vector< BasicAttributeUI* > >	attributes_;

			/// other Attributes
			Urho3D::StringHash				serializableType_;
			Urho3D::Serializable*			serializable_;
			ResourcePickerManager*   editorResourcePicker_;

			/// Exceptions for string attributes that should not be continuously edited
			Urho3D::Vector<Urho3D::String>		noTextChangedAttrs_;


			/// Serialized Attributes
			bool	showNonEditableAttribute_;
			int		attrNameWidth_;
			int		attrHeight_;
		};
	}
}