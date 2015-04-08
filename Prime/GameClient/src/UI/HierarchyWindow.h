#pragma once

#include "UIGlobals.h"
#include "Macros.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Core/Context.h>

namespace Urho3D
{
	class Text;
	class Button;
	class ListView;
	class CheckBox;
	class UIElement;
	class Component;
	class Node;
	class Scene;
	class FileSystem;
	class ResourceCache;
	class XMLFile;
}

namespace Prime
{
	namespace UI
	{
		/// \todo redirect Double/Click, SelectionChange  ... event  
		class HierarchyWindow : public Urho3D::Window
		{
			OBJECT(HierarchyWindow);
		public:
			/// Construct.
			HierarchyWindow(Urho3D::Context* context);
			/// Destruct.
			virtual ~HierarchyWindow();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			/// Update 
			void UpdateHierarchyItem(Urho3D::Serializable* serializable, bool clear = false);
			void SetTitleBarVisible(bool show);
			/// Setters
			void SetTitle(const Urho3D::String& title);
			void EnableToolButtons(bool enable);
			void SetVisibleToolButtons(bool visible);
			void SetScene(Urho3D::Scene* scene);
			void SetUIElement(Urho3D::UIElement* rootui);
			void SetIconStyle(Urho3D::XMLFile* iconstyle);

			/// Getters
			const Urho3D::String&	GetTitle();
			unsigned int	GetListIndex(Urho3D::Serializable* serializable);
			unsigned int	GetComponentListIndex(Urho3D::Component* component);
			Urho3D::Scene*			GetScene();
			Urho3D::UIElement*		GetUIElement();
			Urho3D::XMLFile*		GetIconStyle();
			Urho3D::ListView*		GetHierarchyList();
			Urho3D::UIElement*		GetTitleBar();
			// Serializable Attributes
			U_PROPERTY_IMP_PASS_BY_REF(Urho3D::Color, normalTextColor_, NormalTextColor)
				U_PROPERTY_IMP_PASS_BY_REF(Urho3D::Color, nonEditableTextColor_, NonEditableTextColor)
				U_PROPERTY_IMP_PASS_BY_REF(Urho3D::Color, modifiedTextColor_, ModifiedTextColor)
				U_PROPERTY_IMP_PASS_BY_REF(Urho3D::Color, nodeTextColor_, NodeTextColor)
				U_PROPERTY_IMP_PASS_BY_REF(Urho3D::Color, componentTextColor_, ComponentTextColor)
				U_PROPERTY_IMP(bool, showInternalUIElement_, ShowInternalUIElement)
				U_PROPERTY_IMP(bool, showTemporaryObject_, ShowTemporaryObject)
				U_PROPERTY_IMP(bool, suppressSceneChanges_, SuppressSceneChanges)
				U_PROPERTY_IMP(bool, suppressUIElementChanges_, SuppressUIElementChanges)

		protected:
			void ClearListView();
			bool TestDragDrop(Urho3D::UIElement* source, Urho3D::UIElement* target, int& itemType);
			void SetID(Urho3D::Text* text, Urho3D::Serializable* serializable, int itemType = ITEM_NONE);
			void AddComponentItem(unsigned int compItemIndex, Urho3D::Component* component, Urho3D::UIElement* parentItem);

			/// Update 
			unsigned int	UpdateHierarchyItem(unsigned int itemIndex, Serializable* serializable, UIElement* parentItem);
			void			UpdateHierarchyItemText(unsigned int itemIndex, bool iconEnabled, const Urho3D::String& textTitle = NO_CHANGE);
			void			UpdateDirtyUI();

			/// UI actions
			void HideHierarchyWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void ExpandCollapseHierarchy(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleHierarchyListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleHierarchyListDoubleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleDragDropTest(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleDragDropFinish(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleTemporaryChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// Scene Events
			void HandleNodeAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleNodeRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleComponentAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleNodeNameChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleNodeEnabledChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleComponentEnabledChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// UI Events
			void HandleUIElementNameChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleUIElementVisibilityChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleUIElementAttributeChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleUIElementAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleUIElementRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			// UI Attributes
			Urho3D::SharedPtr<Urho3D::Text>		titleText_;
			Urho3D::SharedPtr<Urho3D::Button>	closeButton_;
			Urho3D::SharedPtr<Urho3D::Button>	expandButton_;
			Urho3D::SharedPtr<Urho3D::Button>	collapseButton_;
			Urho3D::SharedPtr<Urho3D::CheckBox> allCheckBox_;
			Urho3D::SharedPtr<Urho3D::ListView> hierarchyList_;
			Urho3D::SharedPtr<Urho3D::UIElement>	titleBar_;
			Urho3D::SharedPtr<Urho3D::BorderImage>	img_;
			// Serializable Attributes
			Urho3D::SharedPtr<Urho3D::XMLFile> iconStyle_;
			// other Attributes 
			Urho3D::PODVector<unsigned int> hierarchyUpdateSelections_;
			/// \todo use weakptr
			Urho3D::WeakPtr<Urho3D::Scene> scene_;
			Urho3D::WeakPtr<Urho3D::UIElement> mainUI_;

		};
	}
}