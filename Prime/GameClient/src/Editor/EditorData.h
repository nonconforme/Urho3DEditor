#ifndef _EDITOR_EDITORDATA_H
#define _EDITOR_EDITORDATA_H

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>

namespace Urho3D
{
	class UIElement;
	class ResourceCache;
	class UI;
	class Graphics;
	class Scene;
	class View3D;
	class Window;
	class Node;
	class Component;
	class Resource;
	class XMLFile;
	class FileSelector;
	class Camera;
}
namespace Prime
{
	namespace UI
	{
		class MenuBarUI;
		class ToolBarUI;
		class MiniToolBarUI;
		class AttributeWindowUI;
		class AttributeInspector;
		class HierarchyWindow;
	}

	namespace Editor
	{
		class ProjectManager;
		class EditorPlugin;

		class EditorData : public Urho3D::Object
		{
			friend class EditorState;

			OBJECT(EditorData);
		public:
			EditorData(Urho3D::Context* context, EditorState* editor = NULL);
			virtual ~EditorData();
			static void RegisterObject(Urho3D::Context* context);
			void Load();

			void			SetGlobalVarNames(const Urho3D::String& name);
			const Urho3D::Variant&	GetGlobalVarNames(Urho3D::StringHash& name);

			/// Get the editable scene.
			Urho3D::Scene*	GetEditorScene();
			/// Set the editable scene.
			void	SetEditorScene(Urho3D::Scene* scene);
			// Editor Plugin handling.
			/// return an editor plugin that can handle this object and has the main screen (middle frame).
			EditorPlugin* GetEditor(Object *object);
			/// return an editor plugin that can handle this object and is not in the middle frame tabs.
			EditorPlugin* GetSubeditor(Object *object);
			/// return an editor plugin by name.
			EditorPlugin* GetEditor(const Urho3D::String& name);
			/// add editor plugin
			void AddEditorPlugin(EditorPlugin* plugin);
			/// remove editor plugin
			void RemoveEditorPlugin(EditorPlugin* plugin);


			Urho3D::XMLFile*	GetDefaultStyle(){ return defaultStyle_; }
			Urho3D::XMLFile*	GetIconStyle() { return iconStyle_; }
			Urho3D::UIElement*	GetEdiorRootUI() { return rootUI_; }
			EditorState*		GetEditor();

			Urho3D::Vector<Urho3D::String> uiSceneFilters;
			Urho3D::Vector<Urho3D::String> uiElementFilters;
			Urho3D::Vector<Urho3D::String> uiAllFilters;
			Urho3D::Vector<Urho3D::String> uiScriptFilters;
			Urho3D::Vector<Urho3D::String> uiParticleFilters;
			Urho3D::Vector<Urho3D::String> uiRenderPathFilters;
			unsigned int uiSceneFilter;
			unsigned int  uiElementFilter;
			unsigned int  uiNodeFilter;
			unsigned int  uiImportFilter;
			unsigned int  uiScriptFilter;
			unsigned int  uiParticleFilter;
			unsigned int  uiRenderPathFilter;
			Urho3D::String uiScenePath;
			Urho3D::String uiElementPath;
			Urho3D::String uiNodePath;
			Urho3D::String uiImportPath;
			Urho3D::String uiScriptPath;
			Urho3D::String uiParticlePath;
			Urho3D::String uiRenderPathPath;
			Urho3D::Vector<Urho3D::String> uiRecentScenes;
			Urho3D::String screenshotDir;

		protected:

			Urho3D::SharedPtr<Urho3D::Scene> scene_;
			/// loaded 
			Urho3D::SharedPtr<Urho3D::XMLFile> defaultStyle_;
			Urho3D::SharedPtr<Urho3D::XMLFile> iconStyle_;
			Urho3D::SharedPtr<Urho3D::UIElement> rootUI_;
			EditorState* editor_;
			// Node or UIElement hash-to-varname reverse mapping
			Urho3D::VariantMap globalVarNames_;
			/// Editor plugin handling
			Urho3D::Vector<Urho3D::SharedPtr<EditorPlugin> > editorPlugins_;
		};
	}
}

#endif