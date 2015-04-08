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

#include "TabWindow.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/BorderImage.h>


namespace Prime
{
	namespace UI
	{
		const Urho3D::StringHash TAB_INDEX("Tab_Index");
		extern const char* UI_CATEGORY("UICategory");

		TabWindow::TabWindow(Urho3D::Context* context) : Window(context)
		{
			SetEnabled(true);
			bringToFront_ = true;
			clipChildren_ = true;

			SetLayout(Urho3D::LM_VERTICAL);

			buttonContainer_ = CreateChild<UIElement>("TW_ButtonContainer");
			buttonContainer_->SetInternal(true);
			buttonContainer_->SetLayout(Urho3D::LM_HORIZONTAL);
			buttonContainer_->SetFixedHeight(25);

			// 		BorderImage* img = CreateChild<BorderImage>();
			// 		img->SetInternal(true);
			// 		img->SetFixedHeight(11);

			contentContainer_ = CreateChild<UIElement>("TW_ContentContainer");
			contentContainer_->SetInternal(true);
		}

		TabWindow::~TabWindow()
		{

		}

		void TabWindow::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<TabWindow>(UI_CATEGORY);
			COPY_BASE_ATTRIBUTES(Window);

		}

		unsigned TabWindow::AddTab(const Urho3D::String& name, Urho3D::UIElement* content)
		{
			Urho3D::StringHash id(name);
			Urho3D::HashMap<Urho3D::StringHash, unsigned>::Iterator it = nameTabMapping_.Find(id);
			if (it == nameTabMapping_.End())
			{

				/// create new tab
				unsigned index = tabsContent_.Size();
				nameTabMapping_[id] = index;

				tabsContent_.Push(Urho3D::SharedPtr<Urho3D::UIElement>(content));
				content->SetFixedHeight(GetHeight() - buttonContainer_->GetHeight());
				int layoutwidth = GetLayoutBorder().left_ + GetLayoutBorder().right_;
				content->SetFixedWidth(GetWidth() - layoutwidth);

				Urho3D::SharedPtr<Urho3D::Button> tabbutton(buttonContainer_->CreateChild<Urho3D::Button>(name));
				tabbutton->SetStyle("TabButton");
				tabbutton->SetVar(TAB_INDEX, index);

				SubscribeToEvent(tabbutton.Get(), Urho3D::E_RELEASED, HANDLER(TabWindow, HandleButton));

				Urho3D::Text* tabButtonLabel = tabbutton->CreateChild<Urho3D::Text>(name + "_Text");
				tabButtonLabel->SetText(name);
				tabButtonLabel->SetStyle("Text");
				tabButtonLabel->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
				tabsButtons_.Push(tabbutton);
				tabbutton->SetFixedWidth(tabButtonLabel->GetRowWidth(0) + tabbutton->GetLayoutBorder().left_ + tabbutton->GetLayoutBorder().right_);

				if (activeContent_.Null())
					SetActiveTab(index);


				return index;
			}
			else
			{
				if (activeContent_.Null())
					SetActiveTab(it->second_);

				/// return found tab
				return it->second_;
			}
		}

		void TabWindow::RemoveTab(const Urho3D::String& name)
		{
			Urho3D::StringHash id(name);
			Urho3D::HashMap<Urho3D::StringHash, unsigned>::Iterator it = nameTabMapping_.Find(id);

			if (it != nameTabMapping_.End())
			{
				RemoveTab(it->second_);
			}

		}

		void TabWindow::RemoveTab(unsigned index)
		{
			if (index < tabsContent_.Size())
			{
				UIElement* temp = tabsContent_[index];
				Urho3D::Button* tempB = tabsButtons_[index];
				tempB->Remove();
				tabsButtons_.Erase(index);

				if (temp == activeContent_.Get())
				{
					activeContent_->Remove();
					activeContent_ = NULL;
				}
				tabsContent_.Erase(index);

				if (tabsContent_.Size() > 0)
				{
					SetActiveTab(0);
				}
				Urho3D::HashMap<Urho3D::StringHash, unsigned>::Iterator it;
				for (it = nameTabMapping_.Begin(); it != nameTabMapping_.End(); it++)
				{
					if (it->second_ == index)
					{
						nameTabMapping_.Erase(it);
						break;
					}
				}
			}
		}

		bool TabWindow::SetActiveTab(const Urho3D::String& name)
		{
			Urho3D::StringHash id(name);
			Urho3D::HashMap<Urho3D::StringHash, unsigned>::Iterator it = nameTabMapping_.Find(id);

			if (it != nameTabMapping_.End())
			{

				return SetActiveTab(it->second_);
			}
			return false;
		}

		bool TabWindow::SetActiveTab(unsigned index)
		{
			if (index < tabsContent_.Size())
			{
				UIElement* temp = tabsContent_[index];
				if (temp != activeContent_.Get())
				{
					if (activeContent_.NotNull())
						activeContent_->Remove();

					activeContent_ = temp;
					contentContainer_->AddChild(temp);

					activeContent_->SetFixedHeight(GetHeight() - buttonContainer_->GetHeight());
					int layoutwidth = GetLayoutBorder().left_ + GetLayoutBorder().right_;
					activeContent_->SetFixedWidth(GetWidth() - layoutwidth);

					using namespace ActiveTabChanged;
					Urho3D::VariantMap& eventData_ = GetEventDataMap();
					eventData_[P_TABINDEX] = index;
					SendEvent(E_ACTIVETABCHANGED, eventData_);

				}
				return true;
			}
			return false;
		}

		void TabWindow::HandleButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::Released;

			UIElement* element = (UIElement*)eventData[P_ELEMENT].GetPtr();
			if (element)
			{
				const Urho3D::Variant& indexVar = element->GetVar(TAB_INDEX);
				if (indexVar != Urho3D::Variant::EMPTY)
				{
					SetActiveTab(indexVar.GetUInt());
				}
			}
		}

		void TabWindow::OnResize()
		{
			if (activeContent_.NotNull())
			{
				int layoutwidth = GetLayoutBorder().left_ + GetLayoutBorder().right_;
				activeContent_->SetFixedWidth(GetWidth() - layoutwidth);
			}
		}

	}
}