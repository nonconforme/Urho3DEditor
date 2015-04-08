#include "ResourceBrowser.h"
#include "../UI/MenuBarUI.h"
#include "../UI/UIGlobals.h"
#include "../UI/TabWindow.h"
#include "EditorView.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/UI/View3D.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Graphics/Texture.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/IO/Serializer.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Math/BoundingBox.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Input/InputEvents.h>


namespace Prime
{
	namespace Editor
	{
		const unsigned int BROWSER_WORKER_ITEMS_PER_TICK = 10;
		const unsigned int BROWSER_SEARCH_LIMIT = 50;
		const int BROWSER_SORT_MODE_ALPHA = 1;
		const int BROWSER_SORT_MODE_SEARCH = 2;

		const int RESOURCE_TYPE_UNUSABLE = -2;
		const int RESOURCE_TYPE_UNKNOWN = -1;
		const int RESOURCE_TYPE_NOTSET = 0;
		const int RESOURCE_TYPE_SCENE = 1;
		const int RESOURCE_TYPE_SCRIPTFILE = 2;
		const int RESOURCE_TYPE_MODEL = 3;
		const int RESOURCE_TYPE_MATERIAL = 4;
		const int RESOURCE_TYPE_ANIMATION = 5;
		const int RESOURCE_TYPE_IMAGE = 6;
		const int RESOURCE_TYPE_SOUND = 7;
		const int RESOURCE_TYPE_TEXTURE = 8;
		const int RESOURCE_TYPE_FONT = 9;
		const int RESOURCE_TYPE_PREFAB = 10;
		const int RESOURCE_TYPE_TECHNIQUE = 11;
		const int RESOURCE_TYPE_PARTICLEEFFECT = 12;
		const int RESOURCE_TYPE_UIELEMENT = 13;
		const int RESOURCE_TYPE_UIELEMENTS = 14;
		const int RESOURCE_TYPE_ANIMATION_SETTINGS = 15;
		const int RESOURCE_TYPE_RENDERPATH = 16;
		const int RESOURCE_TYPE_TEXTURE_ATLAS = 17;
		const int RESOURCE_TYPE_2D_PARTICLE_EFFECT = 18;
		const int RESOURCE_TYPE_TEXTURE_3D = 19;
		const int RESOURCE_TYPE_CUBEMAP = 20;
		const int RESOURCE_TYPE_PARTICLEEMITTER = 21;
		const int RESOURCE_TYPE_2D_ANIMATION_SET = 22;

		// any resource type > 0 is valid
		const int NUMBER_OF_VALID_RESOURCE_TYPES = 22;

		const Urho3D::StringHash XML_TYPE_SCENE("scene");
		const Urho3D::StringHash XML_TYPE_NODE("node");
		const Urho3D::StringHash XML_TYPE_MATERIAL("material");
		const Urho3D::StringHash XML_TYPE_TECHNIQUE("technique");
		const Urho3D::StringHash XML_TYPE_PARTICLEEFFECT("particleeffect");
		const Urho3D::StringHash XML_TYPE_PARTICLEEMITTER("particleemitter");
		const Urho3D::StringHash XML_TYPE_TEXTURE("texture");
		const Urho3D::StringHash XML_TYPE_ELEMENT("element");
		const Urho3D::StringHash XML_TYPE_ELEMENTS("elements");
		const Urho3D::StringHash XML_TYPE_ANIMATION_SETTINGS("animation");
		const Urho3D::StringHash XML_TYPE_RENDERPATH("renderpath");
		const Urho3D::StringHash XML_TYPE_TEXTURE_ATLAS("TextureAtlas");
		const Urho3D::StringHash XML_TYPE_2D_PARTICLE_EFFECT("particleEmitterConfig");
		const Urho3D::StringHash XML_TYPE_TEXTURE_3D("texture3d");
		const Urho3D::StringHash XML_TYPE_CUBEMAP("cubemap");
		const Urho3D::StringHash XML_TYPE_SPRITER_DATA("spriter_data");

		const Urho3D::StringHash BINARY_TYPE_SCENE("USCN");
		const Urho3D::StringHash BINARY_TYPE_PACKAGE("UPAK");
		const Urho3D::StringHash BINARY_TYPE_COMPRESSED_PACKAGE("ULZ4");
		const Urho3D::StringHash BINARY_TYPE_ANGLESCRIPT("ASBC");
		const Urho3D::StringHash BINARY_TYPE_MODEL("UMDL");
		const Urho3D::StringHash BINARY_TYPE_SHADER("USHD");
		const Urho3D::StringHash BINARY_TYPE_ANIMATION("UANI");

		const Urho3D::StringHash EXTENSION_TYPE_TTF(".ttf");
		const Urho3D::StringHash EXTENSION_TYPE_OTF(".otf");
		const Urho3D::StringHash EXTENSION_TYPE_OGG(".ogg");
		const Urho3D::StringHash EXTENSION_TYPE_WAV(".wav");
		const Urho3D::StringHash EXTENSION_TYPE_DDS(".dds");
		const Urho3D::StringHash EXTENSION_TYPE_PNG(".png");
		const Urho3D::StringHash EXTENSION_TYPE_JPG(".jpg");
		const Urho3D::StringHash EXTENSION_TYPE_JPEG(".jpeg");
		const Urho3D::StringHash EXTENSION_TYPE_TGA(".tga");
		const Urho3D::StringHash EXTENSION_TYPE_OBJ(".obj");
		const Urho3D::StringHash EXTENSION_TYPE_FBX(".fbx");
		const Urho3D::StringHash EXTENSION_TYPE_COLLADA(".dae");
		const Urho3D::StringHash EXTENSION_TYPE_BLEND(".blend");
		const Urho3D::StringHash EXTENSION_TYPE_ANGELSCRIPT(".as");
		const Urho3D::StringHash EXTENSION_TYPE_LUASCRIPT(".lua");
		const Urho3D::StringHash EXTENSION_TYPE_HLSL(".hlsl");
		const Urho3D::StringHash EXTENSION_TYPE_GLSL(".glsl");
		const Urho3D::StringHash EXTENSION_TYPE_FRAGMENTSHADER(".frag");
		const Urho3D::StringHash EXTENSION_TYPE_VERTEXSHADER(".vert");
		const Urho3D::StringHash EXTENSION_TYPE_HTML(".html");

		const Urho3D::StringHash TEXT_VAR_FILE_ID("browser_file_id");
		const Urho3D::StringHash TEXT_VAR_DIR_ID("browser_dir_id");
		const Urho3D::StringHash TEXT_VAR_RESOURCE_TYPE("resource_type");
		const Urho3D::StringHash TEXT_VAR_RESOURCE_DIR_ID("resource_dir_id");

		const int BROWSER_FILE_SOURCE_RESOURCE_DIR = 1;

		unsigned int BrowserFile::browserFileIndex = 1;
		unsigned int BrowserDir::browserDirIndex = 1;

		bool BrowserFileopCmp(BrowserFile* a, BrowserFile* b)
		{
			return a->fullname == (b->fullname);
		}
		bool BrowserFileopCmpScore(BrowserFile* a, BrowserFile* b)
		{
			return a->sortScore > b->sortScore;
		}
		bool ResourceTypeopCmp(ResourceType& a, ResourceType& b)
		{
			return a.name == b.name;
		}
		bool BrowserDiropCmp(BrowserDir* a, BrowserDir* b)
		{
			return a->name == b->name;
		}
		Urho3D::String Join(const Urho3D::Vector<Urho3D::String>& strings, Urho3D::String seperator)
		{
			Urho3D::String ret;
			if (strings.Empty())
				return ret;
			ret.Append(strings[0]);

			for (unsigned i = 1; i < strings.Size(); i++)
			{
				ret.Append(seperator);
				ret.Append(strings[i]);
			}
			return ret;
		}

		//////////////////////////////////////////////////////////////////////////
		/// get ResourceType functions
		namespace Res
		{
			int GetResourceType(Urho3D::Context* context, Urho3D::String path, Urho3D::StringHash& fileType, Urho3D::ResourceCache* cache = NULL);

			int GetResourceType(Urho3D::StringHash fileType)
			{
				// binary fileTypes
				if (fileType == BINARY_TYPE_SCENE)
					return RESOURCE_TYPE_SCENE;
				else if (fileType == BINARY_TYPE_PACKAGE)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == BINARY_TYPE_COMPRESSED_PACKAGE)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == BINARY_TYPE_ANGLESCRIPT)
					return RESOURCE_TYPE_SCRIPTFILE;
				else if (fileType == BINARY_TYPE_MODEL)
					return RESOURCE_TYPE_MODEL;
				else if (fileType == BINARY_TYPE_SHADER)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == BINARY_TYPE_ANIMATION)
					return RESOURCE_TYPE_ANIMATION;

				// xml fileTypes
				else if (fileType == XML_TYPE_SCENE)
					return RESOURCE_TYPE_SCENE;
				else if (fileType == XML_TYPE_NODE)
					return RESOURCE_TYPE_PREFAB;
				else if (fileType == XML_TYPE_MATERIAL)
					return RESOURCE_TYPE_MATERIAL;
				else if (fileType == XML_TYPE_TECHNIQUE)
					return RESOURCE_TYPE_TECHNIQUE;
				else if (fileType == XML_TYPE_PARTICLEEFFECT)
					return RESOURCE_TYPE_PARTICLEEFFECT;
				else if (fileType == XML_TYPE_PARTICLEEMITTER)
					return RESOURCE_TYPE_PARTICLEEMITTER;
				else if (fileType == XML_TYPE_TEXTURE)
					return RESOURCE_TYPE_TEXTURE;
				else if (fileType == XML_TYPE_ELEMENT)
					return RESOURCE_TYPE_UIELEMENT;
				else if (fileType == XML_TYPE_ELEMENTS)
					return RESOURCE_TYPE_UIELEMENTS;
				else if (fileType == XML_TYPE_ANIMATION_SETTINGS)
					return RESOURCE_TYPE_ANIMATION_SETTINGS;
				else if (fileType == XML_TYPE_RENDERPATH)
					return RESOURCE_TYPE_RENDERPATH;
				else if (fileType == XML_TYPE_TEXTURE_ATLAS)
					return RESOURCE_TYPE_TEXTURE_ATLAS;
				else if (fileType == XML_TYPE_2D_PARTICLE_EFFECT)
					return RESOURCE_TYPE_2D_PARTICLE_EFFECT;
				else if (fileType == XML_TYPE_TEXTURE_3D)
					return RESOURCE_TYPE_TEXTURE_3D;
				else if (fileType == XML_TYPE_CUBEMAP)
					return RESOURCE_TYPE_CUBEMAP;
				else if (fileType == XML_TYPE_SPRITER_DATA)
					return RESOURCE_TYPE_2D_ANIMATION_SET;

				// extension fileTypes
				else if (fileType == EXTENSION_TYPE_TTF)
					return RESOURCE_TYPE_FONT;
				else if (fileType == EXTENSION_TYPE_OTF)
					return RESOURCE_TYPE_FONT;
				else if (fileType == EXTENSION_TYPE_OGG)
					return RESOURCE_TYPE_SOUND;
				else if (fileType == EXTENSION_TYPE_WAV)
					return RESOURCE_TYPE_SOUND;
				else if (fileType == EXTENSION_TYPE_DDS)
					return RESOURCE_TYPE_IMAGE;
				else if (fileType == EXTENSION_TYPE_PNG)
					return RESOURCE_TYPE_IMAGE;
				else if (fileType == EXTENSION_TYPE_JPG)
					return RESOURCE_TYPE_IMAGE;
				else if (fileType == EXTENSION_TYPE_JPEG)
					return RESOURCE_TYPE_IMAGE;
				else if (fileType == EXTENSION_TYPE_TGA)
					return RESOURCE_TYPE_IMAGE;
				else if (fileType == EXTENSION_TYPE_OBJ)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_FBX)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_COLLADA)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_BLEND)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_ANGELSCRIPT)
					return RESOURCE_TYPE_SCRIPTFILE;
				else if (fileType == EXTENSION_TYPE_LUASCRIPT)
					return RESOURCE_TYPE_SCRIPTFILE;
				else if (fileType == EXTENSION_TYPE_HLSL)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_GLSL)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_FRAGMENTSHADER)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_VERTEXSHADER)
					return RESOURCE_TYPE_UNUSABLE;
				else if (fileType == EXTENSION_TYPE_HTML)
					return RESOURCE_TYPE_UNUSABLE;

				return RESOURCE_TYPE_UNKNOWN;
			}

			bool GetExtensionType(Urho3D::String path, Urho3D::StringHash& fileType)
			{
				Urho3D::StringHash type = Urho3D::StringHash(GetExtension(path));
				if (type == EXTENSION_TYPE_TTF)
					fileType = EXTENSION_TYPE_TTF;
				else if (type == EXTENSION_TYPE_OGG)
					fileType = EXTENSION_TYPE_OGG;
				else if (type == EXTENSION_TYPE_WAV)
					fileType = EXTENSION_TYPE_WAV;
				else if (type == EXTENSION_TYPE_DDS)
					fileType = EXTENSION_TYPE_DDS;
				else if (type == EXTENSION_TYPE_PNG)
					fileType = EXTENSION_TYPE_PNG;
				else if (type == EXTENSION_TYPE_JPG)
					fileType = EXTENSION_TYPE_JPG;
				else if (type == EXTENSION_TYPE_JPEG)
					fileType = EXTENSION_TYPE_JPEG;
				else if (type == EXTENSION_TYPE_TGA)
					fileType = EXTENSION_TYPE_TGA;
				else if (type == EXTENSION_TYPE_OBJ)
					fileType = EXTENSION_TYPE_OBJ;
				else if (type == EXTENSION_TYPE_FBX)
					fileType = EXTENSION_TYPE_FBX;
				else if (type == EXTENSION_TYPE_COLLADA)
					fileType = EXTENSION_TYPE_COLLADA;
				else if (type == EXTENSION_TYPE_BLEND)
					fileType = EXTENSION_TYPE_BLEND;
				else if (type == EXTENSION_TYPE_ANGELSCRIPT)
					fileType = EXTENSION_TYPE_ANGELSCRIPT;
				else if (type == EXTENSION_TYPE_LUASCRIPT)
					fileType = EXTENSION_TYPE_LUASCRIPT;
				else if (type == EXTENSION_TYPE_HLSL)
					fileType = EXTENSION_TYPE_HLSL;
				else if (type == EXTENSION_TYPE_GLSL)
					fileType = EXTENSION_TYPE_GLSL;
				else if (type == EXTENSION_TYPE_FRAGMENTSHADER)
					fileType = EXTENSION_TYPE_FRAGMENTSHADER;
				else if (type == EXTENSION_TYPE_VERTEXSHADER)
					fileType = EXTENSION_TYPE_VERTEXSHADER;
				else if (type == EXTENSION_TYPE_HTML)
					fileType = EXTENSION_TYPE_HTML;
				else
					return false;

				return true;
			}

			bool GetBinaryType(Urho3D::Context* context, Urho3D::String path, Urho3D::StringHash & fileType, Urho3D::ResourceCache* cache = NULL)
			{
				Urho3D::StringHash type;
				if (cache)
				{
					Urho3D::SharedPtr<Urho3D::File> file = cache->GetFile(path);
					if (file.Null())
						return false;

					if (file->GetSize() == 0)
						return false;

					type = Urho3D::StringHash(file->ReadFileID());
				}
				else
				{
					Urho3D::File file(context);
					if (!file.Open(path))
						return false;

					if (file.GetSize() == 0)
						return false;

					type = Urho3D::StringHash(file.ReadFileID());
				}

				if (type == BINARY_TYPE_SCENE)
					fileType = BINARY_TYPE_SCENE;
				else if (type == BINARY_TYPE_PACKAGE)
					fileType = BINARY_TYPE_PACKAGE;
				else if (type == BINARY_TYPE_COMPRESSED_PACKAGE)
					fileType = BINARY_TYPE_COMPRESSED_PACKAGE;
				else if (type == BINARY_TYPE_ANGLESCRIPT)
					fileType = BINARY_TYPE_ANGLESCRIPT;
				else if (type == BINARY_TYPE_MODEL)
					fileType = BINARY_TYPE_MODEL;
				else if (type == BINARY_TYPE_SHADER)
					fileType = BINARY_TYPE_SHADER;
				else if (type == BINARY_TYPE_ANIMATION)
					fileType = BINARY_TYPE_ANIMATION;
				else
					return false;

				return true;
			}

			bool GetXmlType(Urho3D::Context* context, Urho3D::String path, Urho3D::StringHash & fileType, Urho3D::ResourceCache* cache = NULL)
			{
				Urho3D::String name;
				if (cache)
				{
					Urho3D::XMLFile* xml = cache->GetResource<Urho3D::XMLFile>(path);
					if (xml == NULL)
						return false;

					name = xml->GetRoot().GetName();
				}
				else
				{
					Urho3D::File file(context);
					if (!file.Open(path))
						return false;

					if (file.GetSize() == 0)
						return false;

					Urho3D::XMLFile xml(context);
					if (xml.Load(file))
						name = xml.GetRoot().GetName();
					else
						return false;
				}

				bool found = false;
				if (!name.Empty())
				{
					found = true;
					Urho3D::StringHash type = Urho3D::StringHash(name);
					if (type == XML_TYPE_SCENE)
						fileType = XML_TYPE_SCENE;
					else if (type == XML_TYPE_NODE)
						fileType = XML_TYPE_NODE;
					else if (type == XML_TYPE_MATERIAL)
						fileType = XML_TYPE_MATERIAL;
					else if (type == XML_TYPE_TECHNIQUE)
						fileType = XML_TYPE_TECHNIQUE;
					else if (type == XML_TYPE_PARTICLEEFFECT)
						fileType = XML_TYPE_PARTICLEEFFECT;
					else if (type == XML_TYPE_PARTICLEEMITTER)
						fileType = XML_TYPE_PARTICLEEMITTER;
					else if (type == XML_TYPE_TEXTURE)
						fileType = XML_TYPE_TEXTURE;
					else if (type == XML_TYPE_ELEMENT)
						fileType = XML_TYPE_ELEMENT;
					else if (type == XML_TYPE_ELEMENTS)
						fileType = XML_TYPE_ELEMENTS;
					else if (type == XML_TYPE_ANIMATION_SETTINGS)
						fileType = XML_TYPE_ANIMATION_SETTINGS;
					else if (type == XML_TYPE_RENDERPATH)
						fileType = XML_TYPE_RENDERPATH;
					else if (type == XML_TYPE_TEXTURE_ATLAS)
						fileType = XML_TYPE_TEXTURE_ATLAS;
					else if (type == XML_TYPE_2D_PARTICLE_EFFECT)
						fileType = XML_TYPE_2D_PARTICLE_EFFECT;
					else if (type == XML_TYPE_TEXTURE_3D)
						fileType = XML_TYPE_TEXTURE_3D;
					else if (type == XML_TYPE_CUBEMAP)
						fileType = XML_TYPE_CUBEMAP;
					else if (type == XML_TYPE_SPRITER_DATA)
						fileType = XML_TYPE_SPRITER_DATA;
					else
						found = false;
				}
				return found;
			}

			Urho3D::String ResourceTypeName(int resourceType)
			{
				if (resourceType == RESOURCE_TYPE_UNUSABLE)
					return "Unusable";
				else if (resourceType == RESOURCE_TYPE_UNKNOWN)
					return "Unknown";
				else if (resourceType == RESOURCE_TYPE_NOTSET)
					return "Uninitialized";
				else if (resourceType == RESOURCE_TYPE_SCENE)
					return "Scene";
				else if (resourceType == RESOURCE_TYPE_SCRIPTFILE)
					return "Script File";
				else if (resourceType == RESOURCE_TYPE_MODEL)
					return "Model";
				else if (resourceType == RESOURCE_TYPE_MATERIAL)
					return "Material";
				else if (resourceType == RESOURCE_TYPE_ANIMATION)
					return "Animation";
				else if (resourceType == RESOURCE_TYPE_IMAGE)
					return "Image";
				else if (resourceType == RESOURCE_TYPE_SOUND)
					return "Sound";
				else if (resourceType == RESOURCE_TYPE_TEXTURE)
					return "Texture";
				else if (resourceType == RESOURCE_TYPE_FONT)
					return "Font";
				else if (resourceType == RESOURCE_TYPE_PREFAB)
					return "Prefab";
				else if (resourceType == RESOURCE_TYPE_TECHNIQUE)
					return "Render Technique";
				else if (resourceType == RESOURCE_TYPE_PARTICLEEFFECT)
					return "Particle Effect";
				else if (resourceType == RESOURCE_TYPE_PARTICLEEMITTER)
					return "Particle Emitter";
				else if (resourceType == RESOURCE_TYPE_UIELEMENT)
					return "UI Element";
				else if (resourceType == RESOURCE_TYPE_UIELEMENTS)
					return "UI Elements";
				else if (resourceType == RESOURCE_TYPE_ANIMATION_SETTINGS)
					return "Animation Settings";
				else if (resourceType == RESOURCE_TYPE_RENDERPATH)
					return "Render Path";
				else if (resourceType == RESOURCE_TYPE_TEXTURE_ATLAS)
					return "Texture Atlas";
				else if (resourceType == RESOURCE_TYPE_2D_PARTICLE_EFFECT)
					return "2D Particle Effect";
				else if (resourceType == RESOURCE_TYPE_TEXTURE_3D)
					return "Texture 3D";
				else if (resourceType == RESOURCE_TYPE_CUBEMAP)
					return "Cubemap";
				else if (resourceType == RESOURCE_TYPE_2D_ANIMATION_SET)
					return "2D Animation Set";
				else
					return "";
			}

			int GetResourceType(Urho3D::Context* context, Urho3D::String path)
			{
				Urho3D::StringHash fileType;
				return GetResourceType(context, path, fileType);
			}

			int GetResourceType(Urho3D::Context* context, Urho3D::String path, Urho3D::StringHash & fileType, Urho3D::ResourceCache* cache)
			{
				if (GetExtensionType(path, fileType) || GetBinaryType(context, path, fileType, cache) || GetXmlType(context, path, fileType, cache))
					return GetResourceType(fileType);

				return RESOURCE_TYPE_UNKNOWN;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// class : BrowserDir
		BrowserDir::BrowserDir(Urho3D::String path_, Urho3D::ResourceCache* cache, ResourceBrowser* resBrowser)
		{
			resourceKey = path_;
			Urho3D::String parent = GetParentPath(path_);
			name = path_;
			name.Replace(parent, "");
			id = browserDirIndex++;
			cache_ = cache;
			resBrowser_ = resBrowser;
		}

		BrowserDir::~BrowserDir()
		{
			for (unsigned i = 0; i < files.Size(); i++)
			{
				if (files[i])
				{
					delete files[i];
					files[i] = NULL;
				}
			}

			files.Clear();
		}

		int BrowserDir::opCmp(BrowserDir& b)
		{
			return name == b.name;
		}

		//////////////////////////////////////////////////////////////////////////
		/// class : BrowserFile
		BrowserFile* BrowserDir::AddFile(Urho3D::String name, unsigned int resourceSourceIndex, unsigned int sourceType)
		{
			Urho3D::String path = resourceKey + "/" + name;
			BrowserFile* file = new BrowserFile(path, resourceSourceIndex, sourceType, cache_, resBrowser_);
			files.Push(file);
			return file;
		}

		BrowserFile::BrowserFile(Urho3D::String path_, unsigned int resourceSourceIndex_, int sourceType_, Urho3D::ResourceCache* cache, ResourceBrowser* resBrowser)
		{
			sourceType = sourceType_;
			resourceSourceIndex = resourceSourceIndex_;
			resourceKey = path_;
			name = GetFileName(path_);
			extension = GetExtension(path_);
			fullname = GetFileNameAndExtension(path_);
			id = browserFileIndex++;
			cache_ = cache;
			resBrowser_ = resBrowser;
		}

		int BrowserFile::opCmp(BrowserFile& b)
		{
			//if (browserSearchSortMode == 1)
			return fullname== (b.fullname);
		}

		int BrowserFile::opCmpScore(BrowserFile& b)
		{
			return sortScore - b.sortScore;
		}

		Urho3D::String BrowserFile::GetResourceSource()
		{
			if (sourceType == BROWSER_FILE_SOURCE_RESOURCE_DIR)
				return cache_->GetResourceDirs()[resourceSourceIndex];
			else
				return "Unknown";
		}

		Urho3D::String BrowserFile::GetFullPath()
		{
			return Urho3D::String(cache_->GetResourceDirs()[resourceSourceIndex] + resourceKey);
		}

		Urho3D::String BrowserFile::GetPath()
		{
			return resourceKey;
		}

		void BrowserFile::DetermainResourceType()
		{
			resourceType = Res::GetResourceType(cache_->GetContext(), GetFullPath(), fileType, NULL);
			Urho3D::Text* browserFileListRow_ = browserFileListRow.Get();
			if (browserFileListRow_ != NULL)
			{
				resBrowser_->InitializeBrowserFileListRow(browserFileListRow_, this);
			}
		}

		Urho3D::String BrowserFile::ResourceTypeName()
		{
			return Res::ResourceTypeName(resourceType);
		}

		void BrowserFile::FileChanged()
		{
			// 		if (!fileSystem.FileExists(GetFullPath()))
			// 		{
			// 		}
			// 		else
			// 		{
			// 		}
		}

		//////////////////////////////////////////////////////////////////////////
		/// class : ResourceBrowser
		ResourceBrowser::ResourceBrowser(Urho3D::Context* context) : Object(context)
		{
			selectedBrowserDirectory = NULL;
			selectedBrowserFile = NULL;
			browserDragFile = NULL;
			rootDir = NULL;
			cache_ = GetSubsystem<Urho3D::ResourceCache>();
			ui_ = GetSubsystem<Urho3D::UI>();
			fileSystem_ = GetSubsystem<Urho3D::FileSystem>();
		}

		ResourceBrowser::~ResourceBrowser()
		{
			Urho3D::HashMap<Urho3D::String, BrowserDir*>::Iterator it;

			for (it = browserDirs.Begin(); it != browserDirs.End(); it++)
			{
				if (it->second_)
				{
					delete it->second_;
					it->second_ = NULL;
				}
			}
			browserDirs.Clear();
		}

		void ResourceBrowser::CreateResourceBrowser()
		{
			if (browserWindow != NULL) return;

			CreateResourceBrowserUI();
			InitResourceBrowserPreview();
			RebuildResourceDatabase();

			/// add an Window Menu Item and add it 
			EditorView* editorView = GetSubsystem<EditorView>();
			Urho3D::Menu* sceneMenu_ = editorView->GetGetMenuBar()->CreateMenu("Window");
			editorView->GetGetMenuBar()->CreateMenuItem("Window", "Resource Browser", UI::A_SHOWRESOURCE_VAR);
			SubscribeToEvent(editorView->GetGetMenuBar(), UI::E_MENUBAR_ACTION, HANDLER(ResourceBrowser, HandleMenuBarAction));
		}

		void ResourceBrowser::Update()
		{
			if (browserFilesToScan.Size() == 0)
				return;

			int counter = 0;
			bool updateBrowserUI = false;
			BrowserFile* scanItem = browserFilesToScan[0];
			while (counter < BROWSER_WORKER_ITEMS_PER_TICK)
			{
				scanItem->DetermainResourceType();

				// next
				browserFilesToScan.Erase(0);
				if (browserFilesToScan.Size() > 0)
					scanItem = browserFilesToScan[0];
				else
					break;
				counter++;
			}

			if (browserFilesToScan.Size() > 0)
				browserStatusMessage->SetText("Files left to scan: " + Urho3D::String(browserFilesToScan.Size()));
			else
				browserStatusMessage->SetText("Scan complete");
		}

		bool ResourceBrowser::IsVisible()
		{
			return browserWindow->IsVisible();
		}

		void ResourceBrowser::HideResourceBrowserWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			browserWindow->SetVisible(false);
			EditorView* editorView = GetSubsystem<EditorView>();
			editorView->GetLeftFrame()->RemoveTab("Resources");
		}

		bool ResourceBrowser::ShowResourceBrowserWindow()
		{
			browserWindow->SetVisible(true);
			browserWindow->BringToFront();
			ui_->SetFocusElement(browserSearch);

			EditorView* editorView = GetSubsystem<EditorView>();
			unsigned index = editorView->GetLeftFrame()->AddTab("Resources", browserWindow);
			editorView->GetLeftFrame()->SetActiveTab(index);

			return true;
		}

		void ResourceBrowser::ToggleResourceFilterWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (browserFilterWindow->IsVisible())
				HideResourceFilterWindow();
			else
				ShowResourceFilterWindow();
		}

		void ResourceBrowser::HideResourceFilterWindow()
		{
			browserFilterWindow->SetVisible(false);
		}

		void ResourceBrowser::ShowResourceFilterWindow()
		{
			int x = browserWindow->GetPosition().x_ + browserWindow->GetWidth();
			int y = browserWindow->GetPosition().y_;
			browserFilterWindow->SetPosition(Urho3D::IntVector2(x, y));
			browserFilterWindow->SetVisible(true);
			browserFilterWindow->BringToFront();
		}

		void ResourceBrowser::RefreshBrowserPreview()
		{
			resourceBrowserPreview->QueueUpdate();
		}

		void ResourceBrowser::ScanResourceDirectories()
		{
			Urho3D::HashMap<Urho3D::String, BrowserDir*>::Iterator it;

			for (it = browserDirs.Begin(); it != browserDirs.End(); it++)
			{
				if (it->second_)
				{
					delete it->second_;
					it->second_ = NULL;
				}
			}
			browserDirs.Clear();
			browserFiles.Clear();
			browserFilesToScan.Clear();

			rootDir = new BrowserDir("", cache_, this);
			browserDirs[""] = rootDir;

			// collect all of the items and sort them afterwards
			for (unsigned int i = 0; i < cache_->GetResourceDirs().Size(); ++i)
			{
				if (activeResourceDirFilters.Find(i) != activeResourceDirFilters.End())
					continue;

				ScanResourceDir(i);
			}
		}

		void ResourceBrowser::ScanResourceDir(unsigned int resourceDirIndex)
		{
			Urho3D::String resourceDir = cache_->GetResourceDirs()[resourceDirIndex];

			ScanResourceDirFiles("", resourceDirIndex);

			Urho3D::Vector<Urho3D::String> dirs;
			fileSystem_->ScanDir(dirs, resourceDir, "*", Urho3D::SCAN_DIRS, true);

			for (unsigned int i = 0; i < dirs.Size(); ++i)
			{
				Urho3D::String path = dirs[i];
				if (path.EndsWith("."))
					continue;

				InitBrowserDir(path);
				ScanResourceDirFiles(path, resourceDirIndex);
			}
		}

		void ResourceBrowser::ScanResourceDirFiles(Urho3D::String path, unsigned int resourceDirIndex)
		{
			Urho3D::String fullPath = cache_->GetResourceDirs()[resourceDirIndex] + path;
			if (!fileSystem_->DirExists(fullPath))
				return;

			BrowserDir* dir = GetBrowserDir(path);

			if (dir == NULL)
				return;

			// get files in directory
			Urho3D::Vector<Urho3D::String> dirFiles;
			fileSystem_->ScanDir(dirFiles, fullPath, "*.*", Urho3D::SCAN_FILES, false);

			// add new files
			for (unsigned int x = 0; x < dirFiles.Size(); x++)
			{
				Urho3D::String filename = dirFiles[x];
				BrowserFile* browserFile = dir->AddFile(filename, resourceDirIndex, BROWSER_FILE_SOURCE_RESOURCE_DIR);
				browserFiles.Push(browserFile);
				browserFilesToScan.Push(browserFile);
			}
		}

		void ResourceBrowser::HandleMenuBarAction(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace UI::MenuBarAction;

			Urho3D::StringHash action = eventData[P_ACTION].GetStringHash();
			if (action == UI::A_SHOWRESOURCE_VAR)
			{
				ui_->SetFocusElement(NULL);
				ShowResourceBrowserWindow();
			}
		}

		void ResourceBrowser::HandleRescanResourceBrowserClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			RebuildResourceDatabase();
		}

		void ResourceBrowser::HandleResourceBrowserDirListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (browserDirList->GetSelection() == Urho3D::M_MAX_UNSIGNED)
				return;

			Urho3D::UIElement* uiElement = browserDirList->GetItems()[browserDirList->GetSelection()];
			BrowserDir* dir = GetBrowserDir(uiElement->GetVar(TEXT_VAR_DIR_ID).GetString());
			if (dir == NULL)
				return;

			PopulateResourceBrowserFilesByDirectory(dir);
		}

		void ResourceBrowser::HandleResourceBrowserFileListSelectionChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (browserFileList->GetSelection() == Urho3D::M_MAX_UNSIGNED)
				return;

			Urho3D::UIElement* uiElement = browserFileList->GetItems()[browserFileList->GetSelection()];
			BrowserFile* file = GetBrowserFileFromUIElement(uiElement);
			if (file == NULL)
				return;

			if (resourcePreviewNode != NULL)
				resourcePreviewNode->Remove();

			resourcePreviewNode = resourcePreviewScene->CreateChild("PreviewNodeContainer");
			CreateResourcePreview(file->GetFullPath(), resourcePreviewNode);

			if (resourcePreviewNode != NULL)
			{
				Urho3D::Vector<Urho3D::BoundingBox> boxes;

				Urho3D::PODVector<Urho3D::StaticModel*> staticModels;
				resourcePreviewNode->GetComponents<Urho3D::StaticModel>(staticModels, true);
				Urho3D::PODVector<Urho3D::AnimatedModel*> animatedModels;
				resourcePreviewNode->GetComponents<Urho3D::AnimatedModel>(animatedModels, true);

				for (unsigned i = 0; i < staticModels.Size(); ++i)
					boxes.Push(staticModels[i]->GetWorldBoundingBox());

				for (unsigned i = 0; i < animatedModels.Size(); ++i)
					boxes.Push(animatedModels[i]->GetWorldBoundingBox());

				if (boxes.Size() > 0)
				{
					Urho3D::Vector3 camPosition = Urho3D::Vector3(0.0f, 0.0f, -1.2f);
					Urho3D::BoundingBox biggestBox = boxes[0];
					for (unsigned i = 1; i < boxes.Size(); ++i)
					{
						if (boxes[i].Size().Length() > biggestBox.Size().Length())
							biggestBox = boxes[i];
					}
					resourcePreviewCameraNode->SetPosition(biggestBox.Center() + camPosition * biggestBox.Size().Length());
				}

				resourcePreviewScene->AddChild(resourcePreviewNode);
				RefreshBrowserPreview();
			}
		}

		void ResourceBrowser::HandleResourceDirFilterToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* checkbox = dynamic_cast<Urho3D::CheckBox*>(eventData[P_ELEMENT].GetPtr());
			if (!checkbox || !checkbox->GetVars().Contains(TEXT_VAR_RESOURCE_DIR_ID))
				return;

			int resourceDir = checkbox->GetVar(TEXT_VAR_RESOURCE_DIR_ID).GetInt();
			Urho3D::Vector<int>::Iterator find = activeResourceDirFilters.Find(resourceDir);

			if (checkbox->IsChecked() && find != activeResourceDirFilters.End())
				activeResourceDirFilters.Erase(find);
			else if (!checkbox->IsChecked() && find == activeResourceDirFilters.End())
				activeResourceDirFilters.Push(resourceDir);

			if (ignoreRefreshBrowserResults == false)
				RebuildResourceDatabase();
		}

		void ResourceBrowser::HandleResourceBrowserSearchTextChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			RefreshBrowserResults();
		}

		void ResourceBrowser::HandleResourceTypeFilterToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* checkbox = dynamic_cast<Urho3D::CheckBox*>(eventData[P_ELEMENT].GetPtr());

			if (!checkbox || !checkbox->GetVars().Contains(TEXT_VAR_RESOURCE_TYPE))
				return;

			int resourceType = checkbox->GetVar(TEXT_VAR_RESOURCE_TYPE).GetInt();
			Urho3D::Vector<int>::Iterator find = activeResourceTypeFilters.Find(resourceType);

			if (checkbox->IsChecked() && find != activeResourceTypeFilters.End())
				activeResourceTypeFilters.Erase(find);
			else if (!checkbox->IsChecked() && find == activeResourceTypeFilters.End())
				activeResourceTypeFilters.Push(resourceType);

			if (ignoreRefreshBrowserResults == false)
				RefreshBrowserResults();
		}

		void ResourceBrowser::HandleHideResourceFilterWindow(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			HideResourceFilterWindow();
		}

		void ResourceBrowser::HandleResourceTypeFilterToggleAllTypesToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* checkbox = dynamic_cast<Urho3D::CheckBox*>(eventData[P_ELEMENT].GetPtr());

			Urho3D::UIElement* filterHolder = browserFilterWindow->GetChild("TypeFilters", true);
			Urho3D::PODVector<Urho3D::UIElement*>  children;
			filterHolder->GetChildren(children, true);

			ignoreRefreshBrowserResults = true;
			for (unsigned i = 0; i < children.Size(); ++i)
			{
				Urho3D::CheckBox* filter = dynamic_cast<Urho3D::CheckBox*>(children[i]);
				if (filter != NULL)
					filter->SetChecked(checkbox->IsChecked());
			}
			ignoreRefreshBrowserResults = false;
			RefreshBrowserResults();
		}

		void ResourceBrowser::HandleResourceDirFilterToggleAllTypesToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Toggled;
			Urho3D::CheckBox* checkbox = dynamic_cast<Urho3D::CheckBox*>(eventData[P_ELEMENT].GetPtr());

			Urho3D::UIElement* filterHolder = browserFilterWindow->GetChild("DirFilters", true);
			Urho3D::PODVector<Urho3D::UIElement*>  children;
			filterHolder->GetChildren(children, true);

			ignoreRefreshBrowserResults = true;
			for (unsigned i = 0; i < children.Size(); ++i)
			{
				Urho3D::CheckBox* filter = dynamic_cast<Urho3D::CheckBox*>(children[i]);
				if (filter != NULL)
					filter->SetChecked(checkbox->IsChecked());
			}
			ignoreRefreshBrowserResults = false;
			RebuildResourceDatabase();
		}

		void ResourceBrowser::HandleBrowserFileDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::DragBegin;
			Urho3D::UIElement* uiElement = (Urho3D::UIElement*)eventData[P_ELEMENT].GetPtr();
			browserDragFile = GetBrowserFileFromUIElement(uiElement);
		}

		void ResourceBrowser::HandleBrowserFileDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (browserDragFile == NULL)
				return;

			Urho3D::UIElement* element = ui_->GetElementAt(ui_->GetCursor()->GetScreenPosition());
			if (element != NULL)
				return;

			if (browserDragFile->resourceType == RESOURCE_TYPE_MATERIAL)
			{
				/// \todo
				// 			StaticModel* model = dynamic_cast<StaticModel*>(GetDrawableAtMousePostion());
				// 			if (model !is null)
				// 			{
				// 				AssignMaterial(model, browserDragFile.resourceKey);
				// 			}
			}

			browserDragFile = NULL;
			browserDragComponent = NULL;
			browserDragNode = NULL;
		}

		void ResourceBrowser::HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::FileChanged;

			Urho3D::String filename = eventData[P_FILENAME].GetString();
			BrowserFile* file = GetBrowserFileFromPath(filename);

			if (file == NULL)
			{
				// TODO: new file logic when watchers are supported
				return;
			}
			else
			{
				file->FileChanged();
			}
		}

		void ResourceBrowser::RotateResourceBrowserPreview(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::DragMove;

			int elemX = eventData[P_ELEMENTX].GetInt();
			int elemY = eventData[P_ELEMENTY].GetInt();

			if (resourceBrowserPreview->GetHeight() > 0 && resourceBrowserPreview->GetWidth() > 0)
			{
				float yaw = ((resourceBrowserPreview->GetHeight() / 2) - elemY) * (90.0f / resourceBrowserPreview->GetHeight());
				float pitch = ((resourceBrowserPreview->GetWidth() / 2) - elemX) * (90.0f / resourceBrowserPreview->GetWidth());

				resourcePreviewNode->SetRotation(resourcePreviewNode->GetRotation().Slerp(Urho3D::Quaternion(yaw, pitch, 0.0f), 0.1f));
				RefreshBrowserPreview();
			}
		}

		void ResourceBrowser::HandleBrowserFileClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
	// 		using namespace ItemClicked;
	// 
	// 		if (eventData[P_BUTTON].GetInt() != MOUSEB_RIGHT)
	// 			return;
	// 
	// 		UIElement* uiElement = dynamic_cast<UIElement*>(eventData[P_ITEM].GetPtr());
	// 		BrowserFile* file = GetBrowserFileFromUIElement(uiElement);
	// 
	// 		if (file == NULL)
	// 			return;
	// 
	// 		Vector<UIElement*> actions;
	// 		if (file->resourceType == RESOURCE_TYPE_MATERIAL)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Edit", "HandleBrowserEditResource", file));
	// 		}
	// 		else if (file.resourceType == RESOURCE_TYPE_MODEL)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Instance Animated Model", "HandleBrowserInstantiateAnimatedModel", file));
	// 			actions.Push(CreateBrowserFileActionMenu("Instance Static Model", "HandleBrowserInstantiateStaticModel", file));
	// 		}
	// 		else if (file.resourceType == RESOURCE_TYPE_PREFAB)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Instance Prefab", "HandleBrowserInstantiatePrefab", file));
	// 			actions.Push(CreateBrowserFileActionMenu("Instance in Spawner", "HandleBrowserInstantiateInSpawnEditor", file));
	// 		}
	// 		else if (file.fileType == EXTENSION_TYPE_OBJ ||
	// 			file.fileType == EXTENSION_TYPE_COLLADA ||
	// 			file.fileType == EXTENSION_TYPE_FBX ||
	// 			file.fileType == EXTENSION_TYPE_BLEND)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Import Model", "HandleBrowserImportModel", file));
	// 			actions.Push(CreateBrowserFileActionMenu("Import Scene", "HandleBrowserImportScene", file));
	// 		}
	// 		else if (file.resourceType == RESOURCE_TYPE_UIELEMENT)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Open UI Layout", "HandleBrowserOpenUILayout", file));
	// 		}
	// 		else if (file.resourceType == RESOURCE_TYPE_SCENE)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Load Scene", "HandleBrowserLoadScene", file));
	// 		}
	// 		else if (file.resourceType == RESOURCE_TYPE_SCRIPTFILE)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Execute Script", "HandleBrowserRunScript", file));
	// 		}
	// 		else if (file.resourceType == RESOURCE_TYPE_PARTICLEEFFECT)
	// 		{
	// 			actions.Push(CreateBrowserFileActionMenu("Edit", "HandleBrowserEditResource", file));
	// 		}
	// 
	// 		actions.Push(CreateBrowserFileActionMenu("Open", "HandleBrowserOpenResource", file));
	// 
	// 		ActivateContextMenu(actions);
		}

		BrowserDir* ResourceBrowser::GetBrowserDir(Urho3D::String path)
		{
			Urho3D::HashMap<Urho3D::String, BrowserDir*>::Iterator it = browserDirs.Find(path);
			if (it == browserDirs.End())
				return NULL;
			return it->second_;
		}

		BrowserDir* ResourceBrowser::InitBrowserDir(Urho3D::String path)
		{
			BrowserDir* browserDir = NULL;

			Urho3D::HashMap<Urho3D::String, BrowserDir*>::Iterator it = browserDirs.Find(path);
			if (it != browserDirs.End())
				return it->second_;

			Urho3D::Vector<Urho3D::String> parts = path.Split('/');
			Urho3D::Vector<Urho3D::String> finishedParts;
			if (parts.Size() > 0)
			{
				BrowserDir* parent = rootDir;
				for (unsigned int i = 0; i < parts.Size(); ++i)
				{
					finishedParts.Push(parts[i]);
					Urho3D::String currentPath = Join(finishedParts, "/");
					browserDir = GetBrowserDir(currentPath);
					if (browserDir == NULL)
					{
						browserDir = new BrowserDir(currentPath, cache_, this);
						browserDirs[currentPath] = browserDir;
						parent->children.Push(browserDir);
					}
					parent = browserDir;
	
				}
				return browserDir;
			}
			return NULL;
		}

		BrowserFile* ResourceBrowser::GetBrowserFileFromId(unsigned id)
		{
			if (id == 0)
				return NULL;

			BrowserFile* file;
			for (unsigned i = 0; i < browserFiles.Size(); ++i)
			{
				file = browserFiles[i];
				if (file->id == id) return file;
			}
			return NULL;
		}

		BrowserFile* ResourceBrowser::GetBrowserFileFromUIElement(Urho3D::UIElement* element)
		{
			if (element == NULL || !element->GetVars().Contains(TEXT_VAR_FILE_ID))
				return NULL;
			return GetBrowserFileFromId(element->GetVar(TEXT_VAR_FILE_ID).GetUInt());
		}

		BrowserFile* ResourceBrowser::GetBrowserFileFromPath(Urho3D::String path)
		{
			for (unsigned i = 0; i < browserFiles.Size(); ++i)
			{
				BrowserFile* file = browserFiles[i];
				if (path == file->GetFullPath())
					return file;
			}
			return NULL;
		}

		void ResourceBrowser::CreateResourceBrowserUI()
		{
			browserWindow = ui_->LoadLayout(cache_->GetResource<Urho3D::XMLFile>("UI/IDEResourceBrowser.xml"));
			browserDirList = (Urho3D::ListView*)browserWindow->GetChild("DirectoryList", true);
			browserFileList = (Urho3D::ListView*)browserWindow->GetChild("FileList", true);
			browserSearch = (Urho3D::LineEdit*)browserWindow->GetChild("Search", true);
			browserStatusMessage = (Urho3D::Text*)browserWindow->GetChild("StatusMessage", true);
			browserResultsMessage = (Urho3D::Text*)browserWindow->GetChild("ResultsMessage", true);
			// browserWindow.visible = false;
			//browserWindow.opacity = uiMaxOpacity;

			browserFilterWindow = Urho3D::DynamicCast<Urho3D::Window>(ui_->LoadLayout(cache_->GetResource<Urho3D::XMLFile>("UI/EditorResourceFilterWindow.xml")));
			CreateResourceFilterUI();
			HideResourceFilterWindow();

			int height = Urho3D::Min(ui_->GetRoot()->GetHeight() / 4, 300);
			//browserWindow->SetSize(900, height);
			//browserWindow->SetPosition(35, ui_->GetRoot()->GetHeight() - height - 25);

			/// \todo
			//CloseContextMenu();
		//	ui_->GetRoot()->AddChild(browserWindow);
			ui_->GetRoot()->AddChild(browserFilterWindow);

			EditorView* editorView = GetSubsystem<EditorView>();
			if (editorView)
				editorView->GetLeftFrame()->AddTab("Resources", browserWindow);
		
			
		
		
			SubscribeToEvent(browserWindow->GetChild("CloseButton", true), Urho3D::E_RELEASED, HANDLER(ResourceBrowser, HideResourceBrowserWindow));
			SubscribeToEvent(browserWindow->GetChild("RescanButton", true), Urho3D::E_RELEASED, HANDLER(ResourceBrowser, HandleRescanResourceBrowserClick));
			SubscribeToEvent(browserWindow->GetChild("FilterButton", true), Urho3D::E_RELEASED, HANDLER(ResourceBrowser, ToggleResourceFilterWindow));
			SubscribeToEvent(browserDirList, Urho3D::E_SELECTIONCHANGED, HANDLER(ResourceBrowser, HandleResourceBrowserDirListSelectionChange));
			SubscribeToEvent(browserSearch, Urho3D::E_TEXTCHANGED, HANDLER(ResourceBrowser, HandleResourceBrowserSearchTextChange));
			SubscribeToEvent(browserFileList, Urho3D::E_ITEMCLICKED, HANDLER(ResourceBrowser, HandleBrowserFileClick));
			SubscribeToEvent(browserFileList, Urho3D::E_SELECTIONCHANGED, HANDLER(ResourceBrowser, HandleResourceBrowserFileListSelectionChange));
			SubscribeToEvent(cache_, Urho3D::E_FILECHANGED, HANDLER(ResourceBrowser, HandleFileChanged));
		}

		void ResourceBrowser::InitResourceBrowserPreview()
		{
			resourcePreviewScene = new  Urho3D::Scene(context_);
			resourcePreviewScene->SetName("PreviewScene");

			resourcePreviewScene->CreateComponent<Urho3D::Octree>();
			Urho3D::PhysicsWorld* physicsWorld = resourcePreviewScene->CreateComponent<Urho3D::PhysicsWorld>();
			physicsWorld->SetEnabled(false);
			physicsWorld->SetGravity(Urho3D::Vector3(0.0f, 0.0f, 0.0f));

			Urho3D::Node* zoneNode = resourcePreviewScene->CreateChild("Zone");
			Urho3D::Zone* zone = zoneNode->CreateComponent<Urho3D::Zone>();
			zone->SetBoundingBox(Urho3D::BoundingBox(-1000, 1000));
			zone->SetAmbientColor(Urho3D::Color(0.15f, 0.15f, 0.15f));
			zone->SetFogColor(Urho3D::Color(0.0f, 0.0f, 0.0f));
			zone->SetFogStart(10.0f);
			zone->SetFogEnd(100.0f);

			resourcePreviewCameraNode = resourcePreviewScene->CreateChild("PreviewCamera");
			resourcePreviewCameraNode->SetPosition(Urho3D::Vector3(0.0f, 0.0f, -1.5f));
			Urho3D::Camera* camera = resourcePreviewCameraNode->CreateComponent<Urho3D::Camera>();
			camera->SetNearClip(0.1f);
			camera->SetFarClip(100.0f);

			resourcePreviewLightNode = resourcePreviewScene->CreateChild("PreviewLight");
			resourcePreviewLightNode->SetDirection(Urho3D::Vector3(0.5f, -0.5f, 0.5f));
			resourcePreviewLight = resourcePreviewLightNode->CreateComponent<Urho3D::Light>();
			resourcePreviewLight->SetLightType(Urho3D::LIGHT_DIRECTIONAL);
			resourcePreviewLight->SetSpecularIntensity(0.5f);

			resourceBrowserPreview = (Urho3D::View3D*)browserWindow->GetChild("ResourceBrowserPreview", true);
			resourceBrowserPreview->SetFixedHeight(200);
			resourceBrowserPreview->SetFixedWidth(266);
			resourceBrowserPreview->SetView(resourcePreviewScene, camera);
			resourceBrowserPreview->SetAutoUpdate(false);

			resourcePreviewNode = resourcePreviewScene->CreateChild("PreviewNodeContainer");

			SubscribeToEvent(resourceBrowserPreview, Urho3D::E_DRAGMOVE, HANDLER(ResourceBrowser, RotateResourceBrowserPreview));

			RefreshBrowserPreview();
		}

		void ResourceBrowser::InitializeBrowserFileListRow(Urho3D::Text* fileText, BrowserFile* file)
		{
			fileText->RemoveAllChildren();
			Urho3D::VariantMap params = Urho3D::VariantMap();
			fileText->SetVar(TEXT_VAR_FILE_ID, file->id);
			fileText->SetVar(TEXT_VAR_RESOURCE_TYPE, file->resourceType);
			if (file->resourceType > 0)
				fileText->SetDragDropMode(Urho3D::DD_SOURCE);

			{
				Urho3D::Text* text = new Urho3D::Text(context_);
				fileText->AddChild(text);
				text->SetStyle("FileSelectorListText");
				text->SetText(file->fullname);
				text->SetName(Urho3D::String(file->resourceKey));
			}

		{
			Urho3D::Text* text = new Urho3D::Text(context_);
			fileText->AddChild(text);
			text->SetStyle("FileSelectorListText");
			text->SetText(file->ResourceTypeName());
		}

		if (file->resourceType == RESOURCE_TYPE_MATERIAL ||
			file->resourceType == RESOURCE_TYPE_MODEL ||
			file->resourceType == RESOURCE_TYPE_PARTICLEEFFECT ||
			file->resourceType == RESOURCE_TYPE_PREFAB
			)
		{
			SubscribeToEvent(fileText, Urho3D::E_DRAGBEGIN, HANDLER(ResourceBrowser, HandleBrowserFileDragBegin));
			SubscribeToEvent(fileText, Urho3D::E_DRAGEND, HANDLER(ResourceBrowser, HandleBrowserFileDragEnd));
		}
		}

		void ResourceBrowser::RebuildResourceDatabase()
		{
			if (browserWindow.Null())
				return;

			Urho3D::String newResourceDirsCache = Join(cache_->GetResourceDirs(), ";");
			ScanResourceDirectories();
			if (newResourceDirsCache != resourceDirsCache)
			{
				resourceDirsCache = newResourceDirsCache;
				PopulateResourceDirFilters();
			}
			PopulateBrowserDirectories();
			PopulateResourceBrowserFilesByDirectory(rootDir);
		}

		void ResourceBrowser::RefreshBrowserResults()
		{
			if (browserSearch->GetText().Empty())
			{
				browserDirList->SetVisible(true);
				PopulateResourceBrowserFilesByDirectory(selectedBrowserDirectory);
			}
			else
			{
				browserDirList->SetVisible(false);
				PopulateResourceBrowserBySearch();
			}
		}

		void ResourceBrowser::CreateResourceFilterUI()
		{
			Urho3D::UIElement* options = browserFilterWindow->GetChild("TypeOptions", true);
			Urho3D::CheckBox* toggleAllTypes = (Urho3D::CheckBox*)browserFilterWindow->GetChild("ToggleAllTypes", true);
			Urho3D::CheckBox* toggleAllResourceDirs = (Urho3D::CheckBox*)browserFilterWindow->GetChild("ToggleAllResourceDirs", true);

			SubscribeToEvent(toggleAllTypes, Urho3D::E_TOGGLED, HANDLER(ResourceBrowser, HandleResourceTypeFilterToggleAllTypesToggled));
			SubscribeToEvent(toggleAllResourceDirs, Urho3D::E_TOGGLED, HANDLER(ResourceBrowser, HandleResourceDirFilterToggleAllTypesToggled));
			SubscribeToEvent(browserFilterWindow->GetChild("CloseButton", true), Urho3D::E_RELEASED, HANDLER(ResourceBrowser, HandleHideResourceFilterWindow));

			int columns = 2;
			Urho3D::UIElement* col1 = browserFilterWindow->GetChild("TypeFilterColumn1", true);
			Urho3D::UIElement* col2 = browserFilterWindow->GetChild("TypeFilterColumn2", true);

			// use array to get sort of items
			Urho3D::Vector<ResourceType> sorted;
			for (int i = 1; i <= NUMBER_OF_VALID_RESOURCE_TYPES; ++i)
				sorted.Push(ResourceType(i, Res::ResourceTypeName(i)));
			/// \todo
			// 2 unknown types are reserved for the top, the rest are alphabetized
			//sorted.Sort();
			Urho3D::Sort(sorted.Begin(), sorted.End(), ResourceTypeopCmp);
			sorted.Insert(0, ResourceType(RESOURCE_TYPE_UNKNOWN, Res::ResourceTypeName(RESOURCE_TYPE_UNKNOWN)));
			sorted.Insert(0, ResourceType(RESOURCE_TYPE_UNUSABLE, Res::ResourceTypeName(RESOURCE_TYPE_UNUSABLE)));
			int halfColumns = (int)ceil(float(sorted.Size()) / float(columns));

			for (unsigned int i = 0; i < sorted.Size(); ++i)
			{
				ResourceType& type = sorted[i];
				Urho3D::UIElement* resourceTypeHolder = new Urho3D::UIElement(context_);
				if (i < (unsigned)halfColumns)
					col1->AddChild(resourceTypeHolder);
				else
					col2->AddChild(resourceTypeHolder);

				resourceTypeHolder->SetLayoutMode(Urho3D::LM_HORIZONTAL);
				resourceTypeHolder->SetLayoutSpacing(4);
				Urho3D::Text* label = new Urho3D::Text(context_);
				label->SetStyle("EditorAttributeText");
				label->SetText(type.name);
				Urho3D::CheckBox* checkbox = new Urho3D::CheckBox(context_);
				checkbox->SetName(Urho3D::String(type.id));
				checkbox->SetStyleAuto();
				checkbox->SetVar(TEXT_VAR_RESOURCE_TYPE, i);
				checkbox->SetChecked(true);
				SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(ResourceBrowser, HandleResourceTypeFilterToggled));

				resourceTypeHolder->AddChild(checkbox);
				resourceTypeHolder->AddChild(label);
			}
		}

		void ResourceBrowser::CreateDirList(BrowserDir* dir, Urho3D::UIElement* parentUI /*= null*/)
		{
			Urho3D::Text* dirText = new Urho3D::Text(context_);
			browserDirList->InsertItem(browserDirList->GetNumItems(), dirText, parentUI);
			dirText->SetStyle("FileSelectorListText");
			dirText->SetText(dir->resourceKey.Empty() ? "Root" : dir->name);
			dirText->SetName(dir->resourceKey);
			dirText->SetVar(TEXT_VAR_DIR_ID, dir->resourceKey);

			// Sort directories alphetically
			browserSearchSortMode = BROWSER_SORT_MODE_ALPHA;
			/// \todo
			//dir->children.Sort();
			Sort(dir->children.Begin(), dir->children.End(), BrowserDiropCmp);

			for (unsigned i = 0; i < dir->children.Size(); ++i)
				CreateDirList(dir->children[i], dirText);
		}

		void ResourceBrowser::CreateFileList(BrowserFile* file)
		{
			Urho3D::Text* fileText = new Urho3D::Text(context_);
			fileText->SetStyle("FileSelectorListText");
			fileText->SetLayoutMode(Urho3D::LM_HORIZONTAL);
			browserFileList->InsertItem(browserFileList->GetNumItems(), fileText);
			file->browserFileListRow = fileText;
			InitializeBrowserFileListRow(fileText, file);
		}

		void ResourceBrowser::CreateResourcePreview(Urho3D::String path, Urho3D::Node* previewNode)
		{
			resourceBrowserPreview->SetAutoUpdate(false);
			int resourceType = Res::GetResourceType(context_,path);
			if (resourceType > 0)
			{
				Urho3D::File file(context_);
				file.Open(path);

				if (resourceType == RESOURCE_TYPE_MODEL)
				{
					Urho3D::Model* model = new Urho3D::Model(context_);
					if (model->Load(file))
					{
						Urho3D::StaticModel* staticModel = previewNode->CreateComponent<Urho3D::StaticModel>();
						staticModel->SetModel(model);
						return;
					}
				}
				else if (resourceType == RESOURCE_TYPE_MATERIAL)
				{
					Urho3D::Material* material = new Urho3D::Material(context_);
					/// \todo i get an error if i dont cast it to Resource ... :/
					if (((Urho3D::Resource*)material)->Load(file))
					{
						Urho3D::StaticModel* staticModel = previewNode->CreateComponent<Urho3D::StaticModel>();
						staticModel->SetModel(cache_->GetResource<Urho3D::Model>("Models/Sphere.mdl"));
						staticModel->SetMaterial(material);
						return;
					}
				}
				else if (resourceType == RESOURCE_TYPE_IMAGE)
				{
					Urho3D::SharedPtr<Urho3D::Image> image(new Urho3D::Image(context_));
					if (image->Load(file))
					{
						Urho3D::StaticModel* staticModel = previewNode->CreateComponent<Urho3D::StaticModel>();
						staticModel->SetModel(cache_->GetResource<Urho3D::Model>("Models/Editor/ImagePlane.mdl"));
						Urho3D::Material* material = cache_->GetResource<Urho3D::Material>("Materials/Editor/TexturedUnlit.xml");
						Urho3D::SharedPtr<Urho3D::Texture2D> texture(new Urho3D::Texture2D(context_));
						texture->SetData(image, true);
						material->SetTexture(Urho3D::TU_DIFFUSE, texture);
						staticModel->SetMaterial(material);
						return;
					}
				}
				else if (resourceType == RESOURCE_TYPE_PREFAB)
				{
					bool loaded = false;
					if (GetExtension(path) == ".xml")
					{
						Urho3D::XMLFile xmlFile(context_);
						if (xmlFile.Load(file))
							loaded = previewNode->LoadXML(xmlFile.GetRoot(), true);
					}
					else
					{
						loaded = previewNode->Load(file, true);
					}

					Urho3D::PODVector<Urho3D::StaticModel*> statDest;
					previewNode->GetComponents<Urho3D::StaticModel>(statDest, true);
					Urho3D::PODVector<Urho3D::AnimatedModel*> animDest;
					previewNode->GetComponents<Urho3D::AnimatedModel>(animDest, true);

					if (loaded && (statDest.Size() > 0 || animDest.Size() > 0))
						return;

					previewNode->RemoveAllChildren();
					previewNode->RemoveAllComponents();
				}
				else if (resourceType == RESOURCE_TYPE_PARTICLEEFFECT)
				{
					Urho3D::SharedPtr<Urho3D::ParticleEffect> particleEffect(new Urho3D::ParticleEffect(context_));
					/// \todo i get an error if i dont cast it to Resource ... :/
					if (((Urho3D::Resource*)particleEffect)->Load(file))
					{
						Urho3D::ParticleEmitter* particleEmitter = previewNode->CreateComponent<Urho3D::ParticleEmitter>();
						particleEmitter->SetEffect(particleEffect);
						particleEffect->SetActiveTime(0.0f);
						particleEmitter->Reset();
						resourceBrowserPreview->SetAutoUpdate(true);
						return;
					}
				}
			}

			Urho3D::StaticModel* staticModel = previewNode->CreateComponent<Urho3D::StaticModel>();
			staticModel->SetModel(cache_->GetResource<Urho3D::Model>("Models/Editor/ImagePlane.mdl"));
			Urho3D::Material* material = cache_->GetResource<Urho3D::Material>("Materials/Editor/TexturedUnlit.xml");
			Urho3D::SharedPtr<Urho3D::Texture2D> texture(new Urho3D::Texture2D(context_));
			Urho3D::SharedPtr<Urho3D::Image> noPreviewImage(cache_->GetResource<Urho3D::Image>("Textures/Editor/NoPreviewAvailable.png"));
			texture->SetData(noPreviewImage, false);
			material->SetTexture(Urho3D::TU_DIFFUSE, texture);
			staticModel->SetMaterial(material);

			return;
		}

		void ResourceBrowser::PopulateResourceDirFilters()
		{
			Urho3D::UIElement* resourceDirs = browserFilterWindow->GetChild("DirFilters", true);
			resourceDirs->RemoveAllChildren();
			activeResourceDirFilters.Clear();
			for (unsigned i = 0; i < cache_->GetResourceDirs().Size(); ++i)
			{
				Urho3D::UIElement* resourceDirHolder = new Urho3D::UIElement(context_);
				resourceDirs->AddChild(resourceDirHolder);
				resourceDirHolder->SetLayoutMode(Urho3D::LM_HORIZONTAL);
				resourceDirHolder->SetLayoutSpacing(4);
				resourceDirHolder->SetFixedHeight(16);

				Urho3D::Text* label = new Urho3D::Text(context_);
				label->SetStyle("EditorAttributeText");
				label->SetText(cache_->GetResourceDirs()[i].Replaced(fileSystem_->GetProgramDir(), ""));
				Urho3D::CheckBox* checkbox = new Urho3D::CheckBox(context_);
				checkbox->SetName(Urho3D::String(i));
				checkbox->SetStyleAuto();
				checkbox->SetVar(TEXT_VAR_RESOURCE_DIR_ID, i);
				checkbox->SetChecked(true);
				SubscribeToEvent(checkbox, Urho3D::E_TOGGLED, HANDLER(ResourceBrowser, HandleResourceDirFilterToggled));

				resourceDirHolder->AddChild(checkbox);
				resourceDirHolder->AddChild(label);
			}
		}

		void ResourceBrowser::PopulateBrowserDirectories()
		{
			browserDirList->RemoveAllItems();
			CreateDirList(rootDir);
			browserDirList->SetSelection(0);
		}

		void ResourceBrowser::PopulateResourceBrowserFilesByDirectory(BrowserDir* dir)
		{
			selectedBrowserDirectory = dir;
			browserFileList->RemoveAllItems();
			if (dir == NULL)
				return;

			Urho3D::Vector<BrowserFile*> files;
			for (unsigned x = 0; x < dir->files.Size(); x++)
			{
				BrowserFile* file = dir->files[x];

				if (activeResourceTypeFilters.Find(file->resourceType) == activeResourceTypeFilters.End())
					files.Push(file);
			}

			// Sort alphetically
			browserSearchSortMode = BROWSER_SORT_MODE_ALPHA;
			/// \todo
			Urho3D::Sort(files.Begin(), files.End(), BrowserFileopCmp);
	
			PopulateResourceBrowserResults(files);
			browserResultsMessage->SetText("Showing " + Urho3D::String(files.Size()) + " files");
		}

		void ResourceBrowser::PopulateResourceBrowserResults(Urho3D::Vector<BrowserFile*>& files)
		{
			browserFileList->RemoveAllItems();
			for (unsigned i = 0; i < files.Size(); ++i)
				CreateFileList(files[i]);
		}

		void ResourceBrowser::PopulateResourceBrowserBySearch()
		{
			Urho3D::String query = browserSearch->GetText();

			Urho3D::Vector<int> scores;
			Urho3D::Vector<BrowserFile*> scored;
			Urho3D::Vector<BrowserFile*> filtered;
			{
				BrowserFile* file = NULL;
				for (unsigned x = 0; x < browserFiles.Size(); x++)
				{
					file = browserFiles[x];
					file->sortScore = -1;
					if (activeResourceTypeFilters.Find(file->resourceType) == activeResourceTypeFilters.End())
						continue;

					if (activeResourceDirFilters.Find(file->resourceSourceIndex) == activeResourceDirFilters.End())
						continue;

					int find = file->fullname.Find(query, 0, false);
					if (find > -1)
					{
						int fudge = query.Length() - file->fullname.Length();
						int score = find * int(Urho3D::Abs(fudge * 2)) + int(Urho3D::Abs(fudge));
						file->sortScore = score;
						scored.Push(file);
						scores.Push(score);
					}
				}
			}

			// cut this down for a faster sort
			if (scored.Size() > BROWSER_SEARCH_LIMIT)
			{
				/// \todo
				//scores.Sort();
				Sort(scores.Begin(), scores.End());
				int scoreThreshold = scores[BROWSER_SEARCH_LIMIT];
				BrowserFile* file;
				for (unsigned x = 0; x < scored.Size(); x++)
				{
					file = scored[x];
					if (file->sortScore <= scoreThreshold)
						filtered.Push(file);
				}
			}
			else
				filtered = scored;

			browserSearchSortMode = BROWSER_SORT_MODE_ALPHA;
			/// \todo
			//filtered.Sort();
			if (browserSearchSortMode == 1)	
				Sort(filtered.Begin(), filtered.End(), BrowserFileopCmp);
			else
				Sort(filtered.Begin(), filtered.End(), BrowserFileopCmpScore);

			PopulateResourceBrowserResults(filtered);
			browserResultsMessage->SetText("Showing top " + Urho3D::String(filtered.Size()) + " of " + Urho3D::String(scored.Size()) + " results");
		}
	}
}