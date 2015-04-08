#ifndef _EDITOR_RESOURCEBROWSER_H
#define _EDITOR_RESOURCEBROWSER_H

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>

namespace Urho3D
{
	class Text;
	class ResourceCache;
	class FileSystem;
	class UIElement;
	class Window;
	class LineEdit;
	class ListView;
	class Node;
	class Scene;
	class Light;
	class Component;
	class View3D;
	class UI;
}

namespace Prime
{
	namespace Editor
	{
		class ResourceBrowser;
		class BrowserFile;

		class BrowserDir
		{
		public:
			BrowserDir(Urho3D::String path_, Urho3D::ResourceCache* cache, ResourceBrowser* resBrowser);
			virtual ~BrowserDir();
			int opCmp(BrowserDir& b);

			BrowserFile* AddFile(Urho3D::String name, unsigned int resourceSourceIndex, unsigned int sourceType);

			unsigned int id;
			Urho3D::String resourceKey;
			Urho3D::String name;
			Urho3D::Vector<BrowserDir*> children;
			Urho3D::Vector<BrowserFile*> files;
			static unsigned int browserDirIndex;
			Urho3D::ResourceCache* cache_;
			ResourceBrowser* resBrowser_;
		};

		class BrowserFile
		{
		public:
			BrowserFile(Urho3D::String path_, unsigned int resourceSourceIndex_, int sourceType_, Urho3D::ResourceCache* cache, ResourceBrowser* resBrowser_);
			virtual ~BrowserFile(){}

			int opCmp(BrowserFile& b);
			int opCmpScore(BrowserFile& b);

			Urho3D::String GetResourceSource();

			Urho3D::String GetFullPath();

			Urho3D::String GetPath();

			void DetermainResourceType();

			Urho3D::String ResourceTypeName();

			void FileChanged();


			unsigned int id;
			unsigned int resourceSourceIndex;
			Urho3D::String resourceKey;
			Urho3D::String name;
			Urho3D::String fullname;
			Urho3D::String extension;
			Urho3D::StringHash fileType;
			int resourceType = 0;
			int sourceType = 0;
			int sortScore = 0;
			Urho3D::WeakPtr<Urho3D::Text > browserFileListRow;
			Urho3D::ResourceCache* cache_;
			static unsigned int browserFileIndex;
			ResourceBrowser* resBrowser_;
		};


		class ResourceType
		{
		public:
			int id;
			Urho3D::String name;
			ResourceType(int id_ = 0, Urho3D::String name_ = Urho3D::String::EMPTY)
			{
				id = id_;
				name = name_;
			}
			int opCmp(ResourceType& b)
			{
				return name == (b.name);
			}
		};

		class ResourceBrowser : public Urho3D::Object
		{
			OBJECT(ResourceBrowser);
			friend class BrowserDir;
			friend class BrowserFile;
		public:
			/// Construct.
			ResourceBrowser(Urho3D::Context* context);
			/// Destruct.
			virtual ~ResourceBrowser();

			void CreateResourceBrowser();

			/// used to stop ui from blocking while determining file types. imp. of DoResourceBrowserWork
			void Update();
			bool IsVisible();

			void HideResourceBrowserWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			bool ShowResourceBrowserWindow();
			void ToggleResourceFilterWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HideResourceFilterWindow();
			void ShowResourceFilterWindow();

			void RefreshBrowserPreview();
			void RebuildResourceDatabase();
			void RefreshBrowserResults();
		protected:
			void InitResourceBrowserPreview();
			void InitializeBrowserFileListRow(Urho3D::Text* fileText, BrowserFile* file);

			void CreateResourceBrowserUI();
			void CreateResourceFilterUI();
			void CreateDirList(BrowserDir* dir, Urho3D::UIElement* parentUI = NULL);
			void CreateFileList(BrowserFile* file);
			void CreateResourcePreview(Urho3D::String path, Urho3D::Node* previewNode);

			void PopulateResourceDirFilters();
			void PopulateBrowserDirectories();
			void PopulateResourceBrowserFilesByDirectory(BrowserDir* dir);
			void PopulateResourceBrowserResults(Urho3D::Vector<BrowserFile*>& files);
			void PopulateResourceBrowserBySearch();

			void ScanResourceDirectories();
			void ScanResourceDir(unsigned int resourceDirIndex);
			void ScanResourceDirFiles(Urho3D::String path, unsigned int resourceDirIndex);


			void HandleMenuBarAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleRescanResourceBrowserClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceBrowserDirListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceBrowserFileListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceDirFilterToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceBrowserSearchTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceTypeFilterToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleHideResourceFilterWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceTypeFilterToggleAllTypesToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleResourceDirFilterToggleAllTypesToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleBrowserFileDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleBrowserFileDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			// Opens a contextual menu based on what resource item was actioned
			void HandleBrowserFileClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void RotateResourceBrowserPreview(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			BrowserDir* GetBrowserDir(Urho3D::String path);
			/// Makes sure the entire directory tree exists and new dir is linked to parent
			BrowserDir* InitBrowserDir(Urho3D::String path);
			BrowserFile* GetBrowserFileFromId(unsigned id);
			BrowserFile* GetBrowserFileFromUIElement(Urho3D::UIElement* element);
			BrowserFile* GetBrowserFileFromPath(Urho3D::String path);

			Urho3D::ResourceCache* cache_;
			Urho3D::UI* ui_;
			Urho3D::FileSystem* fileSystem_;

			BrowserDir* selectedBrowserDirectory;
			BrowserFile* selectedBrowserFile;
			Urho3D::SharedPtr<Urho3D::Text> browserStatusMessage;
			Urho3D::SharedPtr<Urho3D::Text> browserResultsMessage;
			bool ignoreRefreshBrowserResults = false;
			Urho3D::String resourceDirsCache;

			Urho3D::SharedPtr<Urho3D::UIElement> browserWindow;
			Urho3D::SharedPtr<Urho3D::Window> browserFilterWindow;
			Urho3D::SharedPtr<Urho3D::ListView>  browserDirList;
			Urho3D::SharedPtr<Urho3D::ListView> browserFileList;
			Urho3D::SharedPtr<Urho3D::LineEdit> browserSearch;
			BrowserFile* browserDragFile;
			Urho3D::SharedPtr<Urho3D::Node> browserDragNode;
			Urho3D::SharedPtr<Urho3D::Component> browserDragComponent;
			Urho3D::SharedPtr<Urho3D::View3D> resourceBrowserPreview;
			Urho3D::SharedPtr<Urho3D::Scene> resourcePreviewScene;
			Urho3D::SharedPtr<Urho3D::Node> resourcePreviewNode;
			Urho3D::SharedPtr<Urho3D::Node> resourcePreviewCameraNode;
			Urho3D::SharedPtr<Urho3D::Node> resourcePreviewLightNode;
			Urho3D::SharedPtr<Urho3D::Light> resourcePreviewLight;
			int browserSearchSortMode = 0;

			BrowserDir* rootDir;
			Urho3D::Vector<BrowserFile*> browserFiles;
			Urho3D::HashMap<Urho3D::String, BrowserDir*> browserDirs;
			Urho3D::Vector<int> activeResourceTypeFilters;
			Urho3D::Vector<int> activeResourceDirFilters;

			Urho3D::Vector<BrowserFile*> browserFilesToScan;


		};
	}
}

#endif