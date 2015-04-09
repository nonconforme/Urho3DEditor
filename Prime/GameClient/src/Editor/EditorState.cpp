#include "EditorState.h"
#include "EditorData.h"
#include "EditorView.h"
#include "EditorSelection.h"
#include "StateManager.h"
#include "Menu/Menu.h"
#include "Root.h"
#include "InputActionSystem.h"

#include "EditorPlugin.h"
#include "EPScene3D.h"
#include "EPScene2D.h"
#include "EPGame.h"
#include "ResourceBrowser.h"

#include "UI/ResourcePicker.h"
#include "UI/UIGlobals.h"
#include "UI/MenuBarUI.h"
#include "UI/ToolBarUI.h"
#include "UI/MiniToolBarUI.h"
#include "UI/HierarchyWindow.h"
#include "UI/AttributeInspector.h"
#include "UI/UIUtils.h"
#include "UI/ModalWindow.h"
#include "UI/TabWindow.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
//#include <Urho3D/Graphics/GraphicsImpl.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/UI/MessageBox.h>

namespace Prime
{
	namespace Editor
	{
		sInputAction AS_DELETE("EDITOR_Delete", Urho3D::KEY_DELETE);

		EditorState::EditorState(Urho3D::Context* context) :
			StateObject(context),
			_sceneRootUI(NULL),
			_editorPluginMain(NULL),
			_editorPluginOver(NULL)
		{

		}
		EditorState::~EditorState()
		{
			if (_rootUI)
				_rootUI->Remove();

			//_mainEditorPlugins.Clear();
			
			context_->RemoveSubsystem<EditorData>();
			context_->RemoveSubsystem<EditorView>();
			context_->RemoveSubsystem<EditorSelection>();
		}

		void EditorState::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<EditorState>();
		}

		void EditorState::RegisterEditor(Urho3D::Context* context)
		{
			EditorState::RegisterObject(context);
			EPScene3DView::RegisterObject(context);
			EPGameView::RegisterObject(context);
			EditorData::RegisterObject(context);
			EditorView::RegisterObject(context);
			EditorSelection::RegisterObject(context);

			UI::ModalWindow::RegisterObject(context);
			UI::ResourcePickerManager::RegisterObject(context);
			UI::MenuBarUI::RegisterObject(context);
			UI::ToolBarUI::RegisterObject(context);
			UI::MiniToolBarUI::RegisterObject(context);
			UI::TabWindow::RegisterObject(context);
		}

		void EditorState::CreateEditor()
		{
			Urho3D::UI* ui = context_->GetSubsystem<Urho3D::UI>();
			Urho3D::Input* input = context_->GetSubsystem<Urho3D::Input>();
			Urho3D::Graphics* graphics = context_->GetSubsystem<Urho3D::Graphics>();
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			Urho3D::XMLFile* xmlFile = cache->GetResource<Urho3D::XMLFile>("UI/DefaultStyle.xml");
			Urho3D::XMLFile* styleFile = cache->GetResource<Urho3D::XMLFile>("UI/IDEStyle.xml");
			Urho3D::XMLFile* iconStyle = cache->GetResource<Urho3D::XMLFile>("UI/IDEIcons.xml");
			Urho3D::Font* font = cache->GetResource<Urho3D::Font>("Fonts/Anonymous Pro.ttf");

			ui->GetRoot()->SetDefaultStyle(styleFile);

			// Create Cursor
			Urho3D::Cursor* cursor_ = new Urho3D::Cursor(context_);
			cursor_->SetStyleAuto(xmlFile);
			ui->SetCursor(cursor_);
			//if (Urho3D::GetPlatform() == "Android" || GetPlatform() == "iOS")
			//	ui_->GetCursor()->SetVisible(false);
			// Use OS mouse without grabbing it
			input->SetMouseVisible(true);

			// create main ui
			_rootUI = ui->GetRoot()->CreateChild<Urho3D::UIElement>("IDERoot");
			_rootUI->SetSize(ui->GetRoot()->GetSize());
			_rootUI->SetTraversalMode(Urho3D::TM_DEPTH_FIRST);     // This is needed for root-like element to prevent artifacts
			_rootUI->SetDefaultStyle(styleFile);

			/// create editable scene and ui
			_scene = new Urho3D::Scene(context_);
			_scene->GetOrCreateComponent<Urho3D::Octree>();
			_scene->GetOrCreateComponent<Urho3D::DebugRenderer>();
			// Allow access to the scene from the console
			//script.defaultScene = editorScene;
			// Always pause the scene, and do updates manually
			_scene->SetUpdateEnabled(false);

			if (!GetSubsystem<UI::ResourcePickerManager>())
			{
				/// ResourcePickerManager is needed for the Attribute Inspector, so don't forget to init it
				context_->RegisterSubsystem(new UI::ResourcePickerManager(context_));
				GetSubsystem<UI::ResourcePickerManager>()->Init();
			}

			context_->RegisterSubsystem(new EditorData(context_, this));
			_editorData = GetSubsystem<EditorData>();
			_editorData->Load();

			_editorData->SetEditorScene(_scene);
			context_->RegisterSubsystem(new EditorView(context_, _editorData));
			_editorView = GetSubsystem<EditorView>();

			UI::MenuBarUI* menubar = _editorView->GetGetMenuBar();

			menubar->CreateMenu("File");
			menubar->CreateMenuItem("File", "Quit", UI::A_QUITEDITOR_VAR);

			menubar->CreateMenu("Edit");
			menubar->CreateMenuItem("Edit", "Copy", UI::A_EDIT_COPY_VAR, 'C', Urho3D::QUAL_CTRL);
			menubar->CreateMenuItem("Edit", "Paste", UI::A_EDIT_PASTE_VAR, 'V', Urho3D::QUAL_CTRL);
			menubar->CreateMenuItem("Edit", "Delete", UI::A_EDIT_DELETE_VAR, Urho3D::KEY_DELETE);

			SubscribeToEvent(_editorView->GetGetMenuBar(), UI::E_MENUBAR_ACTION, HANDLER(EditorState, HandleMenuBarAction));

			context_->RegisterSubsystem(new EditorSelection(context_, this));
			_editorSelection = GetSubsystem<EditorSelection>();

			//////////////////////////////////////////////////////////////////////////
			/// create the hierarchy editor
			_hierarchyWindow = new UI::HierarchyWindow(context_);
			_hierarchyWindow->SetResizable(true);
			_hierarchyWindow->SetIconStyle(_editorData->iconStyle_);
			_hierarchyWindow->SetTitle("Scene Hierarchy");
			_hierarchyWindow->SetDefaultStyle(_editorData->defaultStyle_);
			_hierarchyWindow->SetStyleAuto();


			/// \todo
			// dont know why the auto style does not work ...
			_hierarchyWindow->SetTexture(cache->GetResource<Urho3D::Texture2D>("Textures/UI.png"));
			_hierarchyWindow->SetImageRect(Urho3D::IntRect(112, 0, 128, 16));
			_hierarchyWindow->SetBorder(Urho3D::IntRect(2, 2, 2, 2));
			_hierarchyWindow->SetResizeBorder(Urho3D::IntRect(0, 0, 0, 0));
			_hierarchyWindow->SetLayoutSpacing(0);
			_hierarchyWindow->SetLayoutBorder(Urho3D::IntRect(0, 4, 0, 0));
			/// remove the title bar from the window
			_hierarchyWindow->SetTitleBarVisible(false);

			SubscribeToEvent(_hierarchyWindow->GetHierarchyList(), Urho3D::E_SELECTIONCHANGED, HANDLER(EditorState, HandleHierarchyListSelectionChange));
			SubscribeToEvent(_hierarchyWindow->GetHierarchyList(), Urho3D::E_ITEMDOUBLECLICKED, HANDLER(EditorState, HandleHierarchyListDoubleClick));

			/// add Hierarchy inspector to the left side of the editor.
			_editorView->GetLeftFrame()->AddTab("Hierarchy", _hierarchyWindow);
			/// connect the hierarchy with the editable scene.
			_hierarchyWindow->SetScene(_scene);
			/// connect the hierarchy with the editable  ui.
			//_hierarchyWindow->SetUIElement(sceneUI);

			//////////////////////////////////////////////////////////////////////////
			/// create the attribute editor
			_attributeWindow = new UI::AttributeInspector(context_);
			Urho3D::Window* atrele = (Urho3D::Window*)_attributeWindow->Create();
			atrele->SetResizable(false);
			atrele->SetMovable(false);
			/// remove the title bar from the window
			Urho3D::UIElement* titlebar = atrele->GetChild("TitleBar", true);
			if (titlebar)
				titlebar->SetVisible(false);
			/// add Attribute inspector to the right side of the editor.
			_editorView->GetRightFrame()->AddTab("Inspector", atrele);


			////////////////////////////////////////////////////////////////////////
			// create Resource Browser

			//resourceBrowser_ = new ResourceBrowser(context_);
			//resourceBrowser_->CreateResourceBrowser();
			//resourceBrowser_->ShowResourceBrowserWindow();
			SubscribeToEvent(_editorView->GetMiddleFrame(), UI::E_ACTIVETABCHANGED, HANDLER(EditorState, HandleMainEditorTabChanged));
			SubscribeToEvent(Urho3D::E_UPDATE, HANDLER(EditorState, HandleUpdate));

			_editorView->SetToolBarVisible(true);
			_editorView->SetLeftFrameVisible(true);
			_editorView->SetRightFrameVisible(true);
			LoadPlugins();

			_visible = true;
		}

		void EditorState::CreateUI()
		{

		}

		Urho3D::Component* EditorState::GetListComponent(Urho3D::UIElement* item)
		{
			if (_scene.Null())
				return NULL;

			if (item == NULL)
				return NULL;

			if (item->GetVar(UI::TYPE_VAR).GetInt() != UI::ITEM_COMPONENT)
				return NULL;

			return _scene->GetComponent(item->GetVar(UI::COMPONENT_ID_VAR).GetUInt());
		}

		Urho3D::Node* EditorState::GetListNode(Urho3D::UIElement* item)
		{
			if (_scene.Null())
				return NULL;

			if (item == NULL)
				return NULL;

			if (item->GetVar(UI::TYPE_VAR).GetInt() != UI::ITEM_NODE)
				return NULL;

			return _scene->GetNode(item->GetVar(UI::NODE_ID_VAR).GetUInt());
		}

		Urho3D::UIElement* EditorState::GetListUIElement(Urho3D::UIElement*  item)
		{
			if (_scene.Null())
				return NULL;

			if (item == NULL)
				return NULL;

			// Get the text item's ID and use it to retrieve the actual UIElement the text item is associated to
			return GetUIElementByID(UI::UIUtils::GetUIElementID(item));
		}

		Urho3D::UIElement* EditorState::GetUIElementByID(const Urho3D::Variant& id)
		{
			return id == UI::UI_ELEMENT_BASE_ID ? NULL : _sceneRootUI->GetChild(UI::UI_ELEMENT_ID_VAR, id, true);
		}

		void EditorState::CreateFileSelector(const Urho3D::String& title, const Urho3D::String& ok, const Urho3D::String& cancel, const Urho3D::String& initialPath, Urho3D::Vector<Urho3D::String>& filters, unsigned int initialFilter)
		{
			Urho3D::Graphics* graphics = context_->GetSubsystem<Urho3D::Graphics>();
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			// Within the editor UI, the file selector is a kind of a "singleton". When the previous one is overwritten, also
			// the events subscribed from it are disconnected, so new ones are safe to subscribe.
			_uiFileSelector = new  Urho3D::FileSelector(context_);
			_uiFileSelector->SetDefaultStyle(cache->GetResource<Urho3D::XMLFile>("UI/DefaultStyle.xml"));
			_uiFileSelector->SetTitle(title);
			_uiFileSelector->SetPath(initialPath);
			_uiFileSelector->SetButtonTexts(ok, cancel);
			_uiFileSelector->SetFilters(filters, initialFilter);

			Urho3D::IntVector2 size = _uiFileSelector->GetWindow()->GetSize();
			_uiFileSelector->GetWindow()->SetPosition((graphics->GetWidth() - size.x_) / 2, (graphics->GetHeight() - size.y_) / 2);
		}

		void EditorState::CloseFileSelector(unsigned int& filterIndex, Urho3D::String& path)
		{
			// Save filter & path for next time
			filterIndex = _uiFileSelector->GetFilterIndex();
			path = _uiFileSelector->GetPath();

			_uiFileSelector = NULL;
		}

		void EditorState::CloseFileSelector()
		{
			_uiFileSelector = NULL;
		}

		Urho3D::FileSelector* EditorState::GetUIFileSelector()
		{
			return _uiFileSelector;
		}

		void EditorState::AddEditorPlugin(EditorPlugin* plugin)
		{
			if (plugin->HasMainScreen())
			{
				// push fist because tabwindow send tabchanged event on first add and that activates the first plugin.
				_mainEditorPlugins.Push(Urho3D::SharedPtr<EditorPlugin>(plugin));

				if (plugin->GetFramePosition() == EFP_LEFT)
				{
					_editorView->GetLeftFrame()->AddTab(plugin->GetName(), plugin->GetMainScreen());
				}
				else if (plugin->GetFramePosition() == EFP_RIGHT)
				{
					_editorView->GetRightFrame()->AddTab(plugin->GetName(), plugin->GetMainScreen());
				}
				else
				{
					_editorView->GetMiddleFrame()->AddTab(plugin->GetName(), plugin->GetMainScreen());
				}
			}
			_editorData->AddEditorPlugin(plugin);
		}

		void EditorState::RemoveEditorPlugin(EditorPlugin* plugin)
		{
			if (plugin->HasMainScreen())
			{
				_editorView->GetMiddleFrame()->RemoveTab(plugin->GetName());
				for (auto itr = _mainEditorPlugins.Begin(); itr != _mainEditorPlugins.End(); itr++)
				{
					if ((*itr).Get() == plugin)
					{
						_mainEditorPlugins.Remove(*itr);
					}
				}
				
			}
			_editorData->RemoveEditorPlugin(plugin);
		}

		void EditorState::LoadPlugins()
		{
			EPScene3D* plugin = new EPScene3D(context_);
			AddEditorPlugin(plugin);
			_editorPlugin3D = plugin;

			AddEditorPlugin(new EPGame(context_));
			//AddEditorPlugin(new EPScene2D(context_));
		}

		bool EditorState::LoadScene(const Urho3D::String& fileName)
		{
			Urho3D::UI* ui = context_->GetSubsystem<Urho3D::UI>();
			Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			if (fileName.Empty())
				return false;

			ui->GetCursor()->SetShape(Urho3D::CS_BUSY);

			// Always load the scene from the filesystem, not from resource paths
			if (!fileSystem->FileExists(fileName))
			{
				LOGERRORF("No such scene %s", fileName.CString());
				//MessageBox("No such scene.\n" + fileName);
				return false;
			}

			Urho3D::File file(context_);
			if (!file.Open(fileName, Urho3D::FILE_READ))
			{
				LOGERRORF("Could not open file %s", fileName.CString());
				//	MessageBox("Could not open file.\n" + fileName);
				return false;
			}

			// Reset stored script attributes.
			// 	scriptAttributes.Clear();
			//
			// 	// Add the scene's resource path in case it's necessary
			// 	String newScenePath = GetPath(fileName);
			// 	if (!rememberResourcePath || !sceneResourcePath.StartsWith(newScenePath, false))
			// 		SetResourcePath(newScenePath);

			_hierarchyWindow->SetSuppressSceneChanges(true);
			// 	sceneModified = false;
			// 	revertData = null;
			// 	StopSceneUpdate();

			Urho3D::String extension = GetExtension(fileName);
			bool loaded;
			if (extension != ".xml")
				loaded = _scene->Load(file);
			else
				loaded = _scene->LoadXML(file);

			// Release resources which are not used by the new scene
			/// \todo this creates an bug in the attribute inspector because the loaded xml files are released
			cache->ReleaseAllResources(false);

			// Always pause the scene, and do updates manually
			_scene->SetUpdateEnabled(false);

			// 	UpdateWindowTitle();
			// 	DisableInspectorLock();
			_hierarchyWindow->UpdateHierarchyItem(_scene, true);
			// 	ClearEditActions();
			//

			_hierarchyWindow->SetSuppressSceneChanges(false);
			/// \todo
			_editorSelection->ClearSelection();
			_attributeWindow->GetEditNodes() = _editorSelection->GetEditNodes();
			_attributeWindow->GetEditComponents() = _editorSelection->GetEditComponents();
			_attributeWindow->GetEditUIElements() = _editorSelection->GetEditUIElements();
			_attributeWindow->Update();
			//
			// 	// global variable to mostly bypass adding mru upon importing tempscene
			// 	if (!skipMruScene)
			// 		UpdateSceneMru(fileName);
			//
			// 	skipMruScene = false;
			//
			// 	ResetCamera();
			// 	CreateGizmo();
			// 	CreateGrid();
			// 	SetActiveViewport(viewports[0]);
			//
			// 	// Store all ScriptInstance and LuaScriptInstance attributes
			// 	UpdateScriptInstances();

			return loaded;
		}

		void EditorState::AddResourcePath(Urho3D::String newPath, bool usePreferredDir /*= true*/)
		{
			Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			if (newPath.Empty())
				return;
			if (!IsAbsolutePath(newPath))
				newPath = fileSystem->GetCurrentDir() + newPath;

			if (usePreferredDir)
				newPath = AddTrailingSlash(cache->GetPreferredResourceDir(newPath));
			else
				newPath = AddTrailingSlash(newPath);

			// If additive (path of a loaded prefab) check that the new path isn't already part of an old path
			Urho3D::Vector<Urho3D::String> resourceDirs = cache->GetResourceDirs();

			for (unsigned int i = 0; i < resourceDirs.Size(); ++i)
			{
				if (newPath.StartsWith(resourceDirs[i], false))
					return;
			}

			// Add resource path as first priority so that it takes precedence over the default data paths
			cache->AddResourceDir(newPath, 0);
			//RebuildResourceDatabase();
		}

		void EditorState::HandleMenuBarAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace UI::MenuBarAction;

			Urho3D::StringHash action = eventData[P_ACTION].GetStringHash();
			if (action == UI::A_QUITEDITOR_VAR)
			{
				EPScene3D* plugin = static_cast<EPScene3D*>(_editorPlugin3D.Get());

				if (plugin && plugin->IsSceneModified())
				{
					Urho3D::SharedPtr<Urho3D::MessageBox> messageBox(new Urho3D::MessageBox(context_, "Scene has been modified.\nDo you still want to exit?", "Warning"));
					messageBox->AddRef();
					if (messageBox->GetWindow() != NULL)
					{
						Urho3D::Button* cancelButton = (Urho3D::Button*)messageBox->GetWindow()->GetChild("CancelButton", true);
						cancelButton->SetVisible(true);
						cancelButton->SetFocus(true);
						SubscribeToEvent(messageBox, Urho3D::E_MESSAGEACK, HANDLER(EditorState, HandleMessageAcknowledgement));
					}
				}
				else
				{
					SetState(new Menu::MenuState(context_));
				}
			}
			else if (action == UI::A_SHOWHIERARCHY_VAR)
			{
			}
			else if (action == UI::A_SHOWATTRIBUTE_VAR)
			{
			}
			else if (action == UI::A_EDIT_COPY_VAR)
			{
				_editorSelection->SetCopiedNodes(_editorSelection->GetSelectedNodes());
				_editorSelection->SetCopiedComponents(_editorSelection->GetSelectedComponents());
			}
			else if (action == UI::A_EDIT_PASTE_VAR)
			{
				auto targetNodes = _editorSelection->GetSelectedNodes();

				if (targetNodes.Size() == 0)
					return;

				auto components = _editorSelection->GetCopiedComponents();
				for (auto itr = components.Begin(); itr != components.End(); itr++)
				{
					Urho3D::WeakPtr<Urho3D::Component> component = *itr;

					if (!component || !component->GetNode() || !component->GetScene())
						continue;

					for (auto targetItr = targetNodes.Begin(); targetItr != targetNodes.End(); targetItr++)
					{
						Urho3D::WeakPtr<Urho3D::Node> target = *targetItr;

						if (!target)
							continue;

						target->CloneComponent(component);
					}
				}

				auto nodes = _editorSelection->GetCopiedNodes();
				for (auto itr = nodes.Begin(); itr != nodes.End(); itr++)
				{
					Urho3D::WeakPtr<Urho3D::Node> node = *itr;

					if (!node || !node->GetParent() || !node->GetScene())
						continue;

					for (auto targetItr = targetNodes.Begin(); targetItr != targetNodes.End(); targetItr++)
					{
						Urho3D::WeakPtr<Urho3D::Node> target = *targetItr;

						if (!target)
							continue;

						target->AddChild(node->Clone());
					}
				}
			}
			else if (action == UI::A_EDIT_DELETE_VAR)
			{
				_hierarchyWindow->DisableLayoutUpdate();

				auto components = _editorSelection->GetSelectedComponents();
				for (auto itr = components.Begin(); itr != components.End(); itr++)
				{
					Urho3D::String typeName = (*itr)->GetTypeName();
					if (typeName == "Octree" ||
						typeName == "DebugRenderer" ||
						typeName == "MaterialCache2D" ||
						typeName == "DrawableProxy2D")
						continue;

					(*itr)->Remove();
				}
				components.Clear();

				//Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node>> nodePtrs;
				//auto nodes = _editorSelection->GetSelectedNodes();
				//for (auto itr = nodes.Begin(); itr != nodes.End(); itr++)
				//{
				//	Urho3D::WeakPtr<Urho3D::Node> node(*itr);
				//	nodePtrs.Push(node);
				//}
				auto nodes = _editorSelection->GetSelectedNodes();
				for (auto itr = nodes.Begin(); itr != nodes.End(); itr++)
				{
					Urho3D::WeakPtr<Urho3D::Node> node = *itr;

					if (!node || !node->GetParent() || !node->GetScene())
						continue;

					node->Remove();
				}
				nodes.Clear();

				_editorSelection->SetSelectedComponents(components);
				_editorSelection->SetSelectedNodes(nodes);

				_hierarchyWindow->EnableLayoutUpdate();

				HandleHierarchyListSelectionChange("", GetEventDataMap());
			}
		}

		void EditorState::HandleMainEditorTabChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace UI::ActiveTabChanged;

			unsigned index = eventData[P_TABINDEX].GetUInt();

			if (index >= _mainEditorPlugins.Size())
				return; // error ...

			EditorPlugin *new_editor = _mainEditorPlugins[index];
			if (!new_editor)
				return; // error

			if (_editorPluginMain == new_editor)
				return; // do nothing

			if (_editorPluginMain)
				_editorPluginMain->SetVisible(false);

			_editorPluginMain = new_editor;
			_editorPluginMain->SetVisible(true);
			//editorPluginMain_->selectedNotify();
		}

		void EditorState::HandleHierarchyListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			_editorSelection->OnHierarchyListSelectionChange(_hierarchyWindow->GetHierarchyList()->GetItems(), _hierarchyWindow->GetHierarchyList()->GetSelections());
			/// \todo dont copy
			_attributeWindow->GetEditNodes() = _editorSelection->GetEditNodes();
			_attributeWindow->GetEditComponents() = _editorSelection->GetEditComponents();
			_attributeWindow->GetEditUIElements() = _editorSelection->GetEditUIElements();
			_attributeWindow->Update();

			// 	OnSelectionChange();
			//
			// 	// 		PositionGizmo();
			// 	UpdateAttributeInspector();
			// 	// 		UpdateCameraPreview();
		}

		void EditorState::HandleHierarchyListDoubleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ItemDoubleClicked;

			Urho3D::UIElement* item = dynamic_cast<Urho3D::UIElement*>(eventData[P_ITEM].GetPtr());
			/// \todo
		}

		void EditorState::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Update;

			float timeStep = eventData[P_TIMESTEP].GetFloat();

			if (_editorPluginMain)
			{
				_editorPluginMain->Update(timeStep);
			}
			if (_editorPluginOver)
			{
				_editorPluginOver->Update(timeStep);
			}
		}

		void EditorState::HandlePostUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::PostUpdate;

			float timeStep = eventData[P_TIMESTEP].GetFloat();

		}

		void EditorState::HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			CreateEditor();
			PostLoadingComplete();
		}

		void EditorState::HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void EditorState::HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void EditorState::HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void EditorState::HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{

		}

		void EditorState::HandleMessageAcknowledgement(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MessageACK;

			if (eventData[P_OK].GetBool())
			{
				SetState(new Menu::MenuState(context_));
			}
		}
	}
}