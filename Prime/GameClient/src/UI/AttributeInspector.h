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
	class Node;
	class Component;
	class UIElement;

	class BorderImage;
	class ScrollBar;
	class XMLFile;
	class ResourceCache;
	class FileSystem;
	class Window;
	class UIElement;
	class ListView;
	class Serializable;
	class Text;
	class Button;
	class CheckBox;
	class LineEdit;
	class DropDownList;
	class Resource;
	class FileSelector;
}

namespace Prime
{
	namespace UI
	{
		class BasicAttributeUI;
		class AttributeContainer;
		class ResourcePickerManager;

		/// \todo Serialization
		class AttributeInspector : public Urho3D::Object
		{
			OBJECT(AttributeInspector);

		public:
			/// Construct.
			AttributeInspector(Urho3D::Context* context);
			/// Destruct.
			virtual ~AttributeInspector();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			Urho3D::UIElement*  Create();
			/// Update the whole attribute inspector window, when fullUpdate flag is set to true then first delete all
			///	the containers and repopulate them again from scratch.
			/// The fullUpdate flag is usually set to true when the structure of the attributes are different than
			/// the existing attributes in the list.
			void Update(bool fullUpdate = true);
			/// Disable all child containers in the inspector list.
			void DisableAllContainers();

			Urho3D::Window*				GetAttributewindow();
			Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>&		GetEditNodes();
			Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>& GetEditComponents();
			Urho3D::Vector<Urho3D::WeakPtr<Urho3D::UIElement>>&	GetEditUIElements();
		protected:
			/// Get node container in the inspector list, create the container if it is not yet available.
			AttributeContainer* CreateNodeContainer(Urho3D::Serializable* serializable);
			bool				DeleteNodeContainer(Urho3D::Serializable* serializable);
			/// Get component container at the specified index, create the container if it is not yet available at the specified index.
			AttributeContainer*		CreateComponentContainer(Urho3D::Serializable* serializable);
			Urho3D::String					ExtractVariableName(Urho3D::VariantMap& eventData, Urho3D::LineEdit* nameEdit);
			Urho3D::Variant					ExtractVariantType(Urho3D::VariantMap& eventData);
			Urho3D::Vector<Urho3D::Serializable*>	GetAttributeEditorTargets(BasicAttributeUI* attrEdit);

			void		StoreResourcePickerPath();
			Urho3D::Resource*	GetPickedResource(Urho3D::String resourceName);
			Urho3D::String		GetResourceNameFromFullName(const Urho3D::String& resourceName);

			/// Call after the attribute values in the target serializables have been edited. 
			void PostEditAttribute(Urho3D::Vector<Urho3D::Serializable*>& serializables, unsigned int index, const Urho3D::Vector<Urho3D::Variant>& oldValues);
			/// Call after the attribute values in the target serializables have been edited. 
			void PostEditAttribute(Urho3D::Serializable* serializable, unsigned int index);

			/// AttributeContainerUI Handlers
			void EditAttribute(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void PickResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void PickResourceDone(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void OpenResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void EditResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void TestResource(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// Handle reset to default event, sent when reset icon in the icon-panel is clicked.
			void HandleResetToDefault(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle create new user-defined variable event for node target.
			void CreateNodeVariable(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle delete existing user-defined variable event for node target.
			void DeleteNodeVariable(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// UI actions
			void HideWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// cached subsystem
			Urho3D::ResourceCache*	cache_;
			Urho3D::FileSystem*		fileSystem_;
			Urho3D::UIElement*		parentContainer_;
			ResourcePickerManager* editorResourcePicker_;

			/// Serialized Attributes
			Urho3D::Color	normalTextColor_;
			Urho3D::Color	modifiedTextColor_;
			Urho3D::Color	nonEditableTextColor_;
			bool	applyMaterialList_;
			bool	showNonEditableAttribute_;
			unsigned int numEditableComponentsPerNode_;

			/// other Attributes
			bool inLoadAttributeEditor_;
			bool inEditAttribute_;
			bool attributesDirty_;
			bool attributesFullDirty_;

			Urho3D::SharedPtr<Urho3D::Window>	attributewindow_;
			Urho3D::SharedPtr<Urho3D::XMLFile>	styleFile_;
			Urho3D::SharedPtr<Urho3D::XMLFile>	iconStyle_;
			///	Constants for accessing xmlResources
			Urho3D::Vector< Urho3D::SharedPtr<Urho3D::XMLFile> >	xmlResources_;
			/// Exceptions for string attributes that should not be continuously edited
			Urho3D::Vector<Urho3D::String>		noTextChangedAttrs_;

			/// node container
			Urho3D::HashMap< Urho3D::StringHash, Urho3D::SharedPtr<AttributeContainer> > nodeContainers_;
			/// cached Attribute Containers for different component Types
			Urho3D::HashMap< Urho3D::StringHash, Urho3D::SharedPtr<AttributeContainer> > componentContainers_;

			Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>>		editNodes_;
			Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Component>>	editComponents_;
			Urho3D::Vector<Urho3D::WeakPtr<Urho3D::UIElement>>	editUIElements_;

			Urho3D::SharedPtr<Urho3D::FileSelector> uiFileSelector_;
		};
	}
}