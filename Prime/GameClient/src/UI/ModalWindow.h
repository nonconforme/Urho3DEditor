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

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>

namespace Urho3D
{
	class Button;
	class Text;
	class UIElement;
	class XMLFile;
}

namespace Prime
{
	namespace UI
	{
		/// Message box dialog.
		class ModalWindow : public Urho3D::Object
		{
			OBJECT(ModalWindow);

		public:
			/// Construct. If layout file is not given, use the default message box layout. If style file is not given, use the default style file from root UI element.
			ModalWindow(Urho3D::Context* context, Urho3D::UIElement* content = NULL, const Urho3D::String& titleString = Urho3D::String::EMPTY, Urho3D::XMLFile* layoutFile = 0, Urho3D::XMLFile* styleFile = 0);
			/// Destruct.
			virtual ~ModalWindow();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			/// Set title text. No-ops if there is no title text element.
			void SetTitle(const Urho3D::String& text);
			/// Set Content Container.
			void SetContent(Urho3D::UIElement* content);

			/// Return title text. Return empty string if there is no title text element.
			const Urho3D::String& GetTitle() const;
			/// Return dialog window.
			Urho3D::UIElement* GetWindow() const { return window_; }
			/// Return Content Container.
			Urho3D::UIElement* GetContentContainer() const;
			/// Return Content.
			Urho3D::UIElement* GetContent() const { return content_; }
		private:
			/// Handle events that dismiss the message box.
			void HandleMessageAcknowledged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// UI element containing the whole UI layout. Typically it is a Window element type.
			Urho3D::SharedPtr<Urho3D::UIElement> window_;
			/// Title text element.
			Urho3D::Text* titleText_;
			/// Content Container element.
			Urho3D::SharedPtr<Urho3D::UIElement> contentContainer_;
			/// Content element.
			Urho3D::SharedPtr<Urho3D::UIElement> content_;
			/// OK button element.
			Urho3D::Button* okButton_;
		};

	}
}