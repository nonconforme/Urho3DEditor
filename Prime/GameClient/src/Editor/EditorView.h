#ifndef _EDITOR_EDITORVIEW_H
#define _EDITOR_EDITORVIEW_H

#pragma once

#include "../UI/Macros.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>

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
}

namespace Prime
{
	namespace UI
	{
		class MenuBarUI;
		class ToolBarUI;
		class MiniToolBarUI;
		class TabWindow;
	}

	namespace Editor
	{
		class EditorData;

		class EditorView : public Urho3D::Object
		{
			OBJECT(EditorView);
		public:
			/// Construct.
			EditorView(Urho3D::Context* context, EditorData* data = NULL);
			/// Destruct.
			virtual ~EditorView();
			static void RegisterObject(Urho3D::Context* context);

			void HandlePost(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void UpdateLayout();

			void SetToolBarVisible(bool enable);
			bool IsToolBarVisible() const;

			void SetLeftFrameVisible(bool enable);
			bool IsLeftFrameVisible() const;

			void SetRightFrameVisible(bool enable);
			bool IsRightFrameVisible() const;

			// 	
			// 		void SetMiniToolBarVisible(bool enable);
			// 		bool IsMiniToolBarVisible() const;

			UI::TabWindow* GetLeftFrame();
			UI::TabWindow* GetRightFrame();
			UI::TabWindow* GetMiddleFrame();
			UI::ToolBarUI* GetToolBar();
			UI::MiniToolBarUI* GetMiniToolBar();
			UI::MenuBarUI* GetGetMenuBar();

			U_PROPERTY_IMP(int, minLeftFrameWidth_, MinLeftFrameWidth);
			U_PROPERTY_IMP(int, minMiddleFrameWidth_, MinMiddleFrameWidth);
			U_PROPERTY_IMP(int, minRightFrameWidth_, MinRightFrameWidth);
		protected:
			/// Cache Subsystems
			Urho3D::ResourceCache* cache_;
			Urho3D::Graphics* graphics_;
			Urho3D::UI* ui_;

			Urho3D::WeakPtr<Urho3D::UIElement> uiRoot_;
			Urho3D::SharedPtr<UI::MenuBarUI> menubar_;
			Urho3D::SharedPtr<UI::ToolBarUI> toolbar_;
			Urho3D::SharedPtr<UI::MiniToolBarUI> miniToolbar_;
			Urho3D::SharedPtr<UI::TabWindow> leftFrame_;
			Urho3D::SharedPtr<UI::TabWindow> middleFrame_;
			Urho3D::SharedPtr<UI::TabWindow> rightFrame_;


		};
	}
}

#endif