#include "EPScene2D.h"
#include "EditorData.h"
#include "EditorView.h"
#include "EditorSelection.h"
#include "EditorState.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Urho2D/Drawable2D.h>

namespace Prime
{
	namespace Editor
	{
		extern const char* URHO2D_CATEGORY("Urho2D");


		EPScene2D::EPScene2D(Urho3D::Context* context) : EditorPlugin(context)
		{

		}

		EPScene2D::~EPScene2D()
		{

		}

		bool EPScene2D::HasMainScreen()
		{
			return true;
		}

		Urho3D::String EPScene2D::GetName() const
		{
			return Urho3D::String("2DView");
		}

		void EPScene2D::Edit(Object *object)
		{

		}

		bool EPScene2D::Handles(Object *object) const
		{
			/// can handle 2d Drawables 
			Urho3D::Drawable2D* drawable = dynamic_cast<Urho3D::Drawable2D*>(object);
			if (drawable)
				return true;
			/// also check if the component is in the Urho2d category
			const Urho3D::HashMap<Urho3D::String, Urho3D::Vector<Urho3D::StringHash> >& categories = context_->GetObjectCategories();
			Urho3D::HashMap<Urho3D::String, Urho3D::Vector<Urho3D::StringHash> >::ConstIterator it = categories.Find(URHO2D_CATEGORY);
			if (it != categories.End())
			{
				Urho3D::Vector<Urho3D::StringHash>::ConstIterator it2 = it->second_.Find(object->GetType());
				if (it2 != it->second_.End())
				{
					return true;
				}
			}

			return false;
		}

		Urho3D::UIElement* EPScene2D::GetMainScreen()
		{
			if (!window_)
			{
				EditorData* editorData_ = GetSubsystem<EditorData>();
				window_ = new Urho3D::Window(context_);
				window_->SetFixedSize(200, 200);
				window_->SetStyleAuto(editorData_->GetDefaultStyle());
				Urho3D::Text* hello = window_->CreateChild<Urho3D::Text>();
				hello->SetText("hello 2d view ... ");
				hello->SetStyleAuto(editorData_->GetDefaultStyle());
			}
			return window_;
		}

		void EPScene2D::SetVisible(bool visible)
		{
			visible_ = visible;
		}

		EPScene2DEditor::EPScene2DEditor(Urho3D::Context* context) : Object(context)
		{
			ui_ = GetSubsystem<Urho3D::UI>();
			input_ = GetSubsystem<Urho3D::Input>();
			cache_ = GetSubsystem<Urho3D::ResourceCache>();
			renderer = GetSubsystem<Urho3D::Renderer>();
			fileSystem_ = GetSubsystem<Urho3D::FileSystem>();

			editorView_ = GetSubsystem<EditorView>();
			editorData_ = GetSubsystem<EditorData>();
			editorSelection_ = GetSubsystem<EditorSelection>();
			if (editorData_)
				editor_ = editorData_->GetEditor();
		}

		EPScene2DEditor::~EPScene2DEditor()
		{

		}

		void EPScene2DEditor::Edit(Object *object)
		{
			/// can handle 2d Drawables 
			Urho3D::Drawable2D* drawable = dynamic_cast<Urho3D::Drawable2D*>(object);
			if (drawable)
			{

			}

			/// also check if the component is in the Urho2d category
			const Urho3D::HashMap<Urho3D::String, Urho3D::Vector<Urho3D::StringHash> >& categories = context_->GetObjectCategories();
			Urho3D::HashMap<Urho3D::String, Urho3D::Vector<Urho3D::StringHash> >::ConstIterator it = categories.Find(URHO2D_CATEGORY);
			if (it != categories.End())
			{
				Urho3D::Vector<Urho3D::StringHash>::ConstIterator it2 = it->second_.Find(object->GetType());
				if (it2 != it->second_.End())
				{

				}
			}
		}

		void EPScene2DEditor::Show()
		{

		}

		void EPScene2DEditor::Hide()
		{

		}

	}
}