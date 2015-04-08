#ifndef _EDITOR_EDITORSTATE_H
#define _EDITOR_EDITORSTATE_H

#include "StateObject.h"

namespace Urho3D
{
	class Camera;
	class Component;
	class CustomGeometry;
	class FileSelector;
	class Menu;
	class Node;
	class Scene;
	class Slider;
	class Text;
	class UIElement;
	class Window;
}

namespace Prime
{
	namespace UI
	{
		
		class MenuBarUI;
		class ToolBarUI;
		class MiniToolBarUI;
		class HierarchyWindow;
		class AttributeInspector;
	}
	namespace Editor
	{
		class Editor;
		class EditorPlugin;
		class EditorData;
		class EditorView;
		class EditorSelection;
		class ResourceBrowser;

		const Urho3D::StringHash A_PROJECT_NEW_VAR("Project_New");
		const Urho3D::StringHash A_PROJECT_OPEN_VAR("Project_Open");
		const Urho3D::StringHash A_PROJECT_SAVE_VAR("Project_Save");
		const Urho3D::StringHash A_PROJECT_EXPORT_VAR("Project_Export");

		class EditorState : public Prime::StateObject
		{
			OBJECT(EditorState);
		public:
			EditorState(Urho3D::Context* context);
			~EditorState();

			static void RegisterObject(Urho3D::Context* context);
			static void RegisterEditor(Urho3D::Context* context);

			/// adds the plugin to the editor data, if plugin has the main screen then add it to the middle frame tabs.
			void AddEditorPlugin(EditorPlugin* plugin);
			/// remove the plugin
			void RemoveEditorPlugin(EditorPlugin* plugin);

			/// load plugins 
			void LoadPlugins();
			/// load scene 
			bool LoadScene(const Urho3D::String& fileName);

			// UI Stuff
			/// create/add a Menu Item to the Menu Bar
			//bool AddToMenuBar(const Urho3D::String& menuTitle, const Urho3D::String& itemTitle, const Urho3D::StringHash& action, int accelKey, int accelQual, bool addToQuickMenu, Urho3D::String quickMenuText);
			/// create/add a Toggle to the Tool Bar
			//bool AddToToolBar(const Urho3D::String& groupname, const Urho3D::String& title);
			/// create/add a Spacer to the Tool Bar
			//void AddSpacerToToolBar(int width);

			void CreateFileSelector(const Urho3D::String& title, const Urho3D::String& ok, const Urho3D::String& cancel,
				const Urho3D::String& initialPath, Urho3D::Vector<Urho3D::String>& filters, unsigned int initialFilter);
			void			CloseFileSelector(unsigned int& filterIndex, Urho3D::String& path);
			void			CloseFileSelector();
			Urho3D::FileSelector*	GetUIFileSelector();

			void AddResourcePath(Urho3D::String newPath, bool usePreferredDir = true);

			/// Getters
			Urho3D::Scene*		GetScene() { return _scene; };
			Urho3D::UIElement*	GetSceneUI() { return _sceneRootUI; }
			Urho3D::Component*	GetListComponent(Urho3D::UIElement*  item);
			Urho3D::Node*		GetListNode(Urho3D::UIElement*  item);
			Urho3D::UIElement*	GetListUIElement(Urho3D::UIElement*  item);
			Urho3D::UIElement*	GetUIElementByID(const Urho3D::Variant& id);
			UI::HierarchyWindow*	GetHierarchyWindow() { return _hierarchyWindow; }
			UI::AttributeInspector* GetAttributeWindow() { return _attributeWindow; }
			EditorSelection*	GetEditorSelection() { return _editorSelection; }
			EditorData*			GetEditorData() { return _editorData; }
			EditorView*			GetEditorView() { return _editorView; }

		protected: // Internal functions

			void CreateEditor();
			void CreateUI();

		protected: // Event handlers

			void SubscribeToEvents();

			void HandleInputAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// Handle Menu Bar Events
			void HandleMenuBarAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle Events
			void HandleMainEditorTabChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// handle Hierarchy Events
			void HandleHierarchyListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleHierarchyListDoubleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandlePostUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		private:
			/// is the editor visible, used for the in game editor
			bool _visible;
			/// currently edited
			Urho3D::SharedPtr<Urho3D::Scene>		_scene;
			Urho3D::SharedPtr<Urho3D::UIElement>	_sceneRootUI;

			/// editor plugin handling
			Urho3D::Vector<EditorPlugin*>	_mainEditorPlugins;
			EditorPlugin*			_editorPluginMain;
			EditorPlugin*			_editorPluginOver;

			/// ui stuff
			Urho3D::SharedPtr<Urho3D::UIElement>	_rootUI;
			Urho3D::SharedPtr<EditorView>	_editorView;
			Urho3D::SharedPtr<Urho3D::FileSelector> _uiFileSelector;

			/// split editor functionality in different classes
			Urho3D::SharedPtr<EditorSelection>	_editorSelection;
			Urho3D::SharedPtr<EditorData>		_editorData;

			/// default IDE Editors
			Urho3D::SharedPtr<UI::HierarchyWindow>		_hierarchyWindow;
			Urho3D::SharedPtr<UI::AttributeInspector>	_attributeWindow;
			Urho3D::SharedPtr<ResourceBrowser>		_resourceBrowser;
		};
	}
}

#endif