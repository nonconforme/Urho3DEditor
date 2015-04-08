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

#include "ModalWindow.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UIElement.h>

namespace Prime
{
	namespace UI
	{

		ModalWindow::ModalWindow(Urho3D::Context* context, Urho3D::UIElement* content /*= NULL*/, const Urho3D::String& titleString /*= String::EMPTY*/, Urho3D::XMLFile* layoutFile /*= 0*/, Urho3D::XMLFile* styleFile /*= 0*/)
			: Object(context),
			titleText_(0),
			okButton_(0)
		{
			// If layout file is not given, use the default message box layout
			if (!layoutFile)
			{
				Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
				layoutFile = cache->GetResource<Urho3D::XMLFile>("UI/ModalWindow.xml");
				if (!layoutFile)    // Error is already logged
					return;         // Note: windowless MessageBox should not be used!
			}

			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();
			window_ = ui->LoadLayout(layoutFile, styleFile);
			if (!window_)   // Error is already logged
				return;
			ui->GetRoot()->AddChild(window_);

			content_ = content;

			// Set the title and message strings if they are given
			titleText_ = dynamic_cast<Urho3D::Text*>(window_->GetChild("TitleText", true));
			if (titleText_ && !titleString.Empty())
				titleText_->SetText(titleString);

			contentContainer_ = dynamic_cast<Urho3D::UIElement*>(window_->GetChild("ContentContainer", true));
			if (contentContainer_ && content)
				contentContainer_->AddChild(content);

			// Center window after the message is set
			Urho3D::Window* window = dynamic_cast<Urho3D::Window*>(window_.Get());
			if (window)
			{
				Urho3D::Graphics* graphics = GetSubsystem<Urho3D::Graphics>();  // May be null if headless
				if (graphics)
				{
					const Urho3D::IntVector2& size = window->GetSize();
					window->SetPosition((graphics->GetWidth() - size.x_) / 2, (graphics->GetHeight() - size.y_) / 2);
				}
				else
					LOGWARNING("Instantiating a modal window in headless mode!");

				window->SetModal(true);
				SubscribeToEvent(window, Urho3D::E_MODALCHANGED, HANDLER(ModalWindow, HandleMessageAcknowledged));
			}

			// Bind the buttons (if any in the loaded UI layout) to event handlers
			okButton_ = dynamic_cast<Urho3D::Button*>(window_->GetChild("OkButton", true));
			if (okButton_)
			{
				ui->SetFocusElement(okButton_);
				SubscribeToEvent(okButton_, Urho3D::E_RELEASED, HANDLER(ModalWindow, HandleMessageAcknowledged));
			}

			Urho3D::Button* cancelButton = dynamic_cast<Urho3D::Button*>(window_->GetChild("CancelButton", true));
			if (cancelButton)
				SubscribeToEvent(cancelButton, Urho3D::E_RELEASED, HANDLER(ModalWindow, HandleMessageAcknowledged));

			Urho3D::Button* closeButton = dynamic_cast<Urho3D::Button*>(window_->GetChild("CloseButton", true));
			if (closeButton)
				SubscribeToEvent(closeButton, Urho3D::E_RELEASED, HANDLER(ModalWindow, HandleMessageAcknowledged));
		}


		ModalWindow::~ModalWindow()
		{
			if (window_)
				window_->Remove();
		}


		void ModalWindow::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<ModalWindow>();
		}


		void ModalWindow::SetTitle(const Urho3D::String& text)
		{
			if (titleText_)
				titleText_->SetText(text);
		}


		void ModalWindow::SetContent(Urho3D::UIElement* content)
		{
			content_ = Urho3D::SharedPtr<Urho3D::UIElement>(content);
		}

		const Urho3D::String& ModalWindow::GetTitle() const
		{
			return titleText_ ? titleText_->GetText() : Urho3D::String::EMPTY;
		}


		Urho3D::UIElement* ModalWindow::GetContentContainer() const
		{
			return contentContainer_;
		}

		void ModalWindow::HandleMessageAcknowledged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MessageACK;

			Urho3D::VariantMap& newEventData = GetEventDataMap();
			newEventData[P_OK] = eventData[Urho3D::Released::P_ELEMENT] == okButton_;
			SendEvent(Urho3D::E_MESSAGEACK, newEventData);

			this->ReleaseRef();
		}

	}

}