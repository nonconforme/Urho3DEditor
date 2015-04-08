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
	class DropDownList;
	class Font;
	class LineEdit;
	class ListView;
	class ResourceCache;
	class Text;
	class UIElement;
	class Window;
	class XMLFile;

	/// %File selector's list entry (file or directory.)
	struct FileSelectorEntry;
}

namespace Prime
{
	namespace UI
	{
		/// %File selector dialog.
		class DirSelector : public Urho3D::Object
		{
			OBJECT(DirSelector);

		public:
			/// Construct.
			DirSelector(Urho3D::Context* context);
			/// Destruct.
			virtual ~DirSelector();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			/// Set fileselector UI style.
			void SetDefaultStyle(Urho3D::XMLFile* style);
			/// Set title text.
			void SetTitle(const Urho3D::String& text);
			/// Set button texts.
			void SetButtonTexts(const Urho3D::String& okText, const Urho3D::String& cancelText);
			/// Set current path.
			void SetPath(const Urho3D::String& path);
			/// Set current filename.
			void SetFileName(const Urho3D::String& fileName);
			/// Set filters.
			void SetFilters(const Urho3D::Vector<Urho3D::String>& filters, unsigned defaultIndex);
			/// Set directory selection mode. Default false.
			void SetDirectoryMode(bool enable);
			/// Update elements to layout properly. Call this after manually adjusting the sub-elements.
			void UpdateElements();

			/// Return the UI style file.
			Urho3D::XMLFile* GetDefaultStyle() const;
			/// Return fileselector window.
			Urho3D::Window* GetWindow() const { return window_; }
			/// Return window title text element.
			Urho3D::Text* GetTitleText() const { return titleText_; }
			/// Return file list.
			Urho3D::ListView* GetFileList() const { return fileList_; }
			/// Return path editor.
			Urho3D::LineEdit* GetPathEdit() const { return pathEdit_; }
			/// Return filename editor.
			Urho3D::LineEdit* GetFileNameEdit() const { return fileNameEdit_; }
			/// Return filter dropdown.
			Urho3D::DropDownList* GetFilterList() const { return filterList_; }
			/// Return OK button.
			Urho3D::Button* GetOKButton() const { return okButton_; }
			/// Return cancel button.
			Urho3D::Button* GetCancelButton() const { return cancelButton_; }
			/// Return close button.
			Urho3D::Button* GetCloseButton() const { return closeButton_; }
			/// Return window title.
			const Urho3D::String& GetTitle() const;
			/// Return current path.
			const Urho3D::String& GetPath() const { return path_; }
			/// Return current filename.
			const Urho3D::String& GetFileName() const;
			/// Return current filter.
			const Urho3D::String& GetFilter() const;
			/// Return current filter index.
			unsigned GetFilterIndex() const;
			/// Return directory mode flag.
			bool GetDirectoryMode() const { return directoryMode_; }

		private:
			/// Set the text of an edit field and ignore the resulting event.
			void SetLineEditText(Urho3D::LineEdit* edit, const Urho3D::String& text);
			/// Refresh the directory listing.
			void RefreshFiles();
			/// Enter a directory or confirm a file. Return true if a directory entered.
			bool EnterFile();
			/// Handle filter changed.
			void HandleFilterChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle path edited.
			void HandlePathChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle file selected from the list.
			void HandleFileSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle file doubleclicked from the list (enter directory / OK the file selection.)
			void HandleFileDoubleClicked(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle file list key pressed.
			void HandleFileListKey(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle OK button pressed.
			void HandleOKPressed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			/// Handle cancel button pressed.
			void HandleCancelPressed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			/// Fileselector window.
			Urho3D::SharedPtr<Urho3D::Window> window_;
			/// Title layout.
			Urho3D::UIElement* titleLayout;
			/// Window title text.
			Urho3D::Text* titleText_;
			/// File list.
			Urho3D::ListView* fileList_;
			/// Path editor.
			Urho3D::LineEdit* pathEdit_;
			/// Filename editor.
			Urho3D::LineEdit* fileNameEdit_;
			/// Filter dropdown.
			Urho3D::DropDownList* filterList_;
			/// OK button.
			Urho3D::Button* okButton_;
			/// OK button text.
			Urho3D::Text* okButtonText_;
			/// Cancel button.
			Urho3D::Button* cancelButton_;
			/// Cancel button text.
			Urho3D::Text* cancelButtonText_;
			/// Close button.
			Urho3D::Button* closeButton_;
			/// Filename and filter layout.
			Urho3D::UIElement* fileNameLayout_;
			/// Separator layout.
			Urho3D::UIElement* separatorLayout_;
			/// Button layout.
			Urho3D::UIElement* buttonLayout_;
			/// Current directory.
			Urho3D::String path_;
			/// Filters.
			Urho3D::Vector<Urho3D::String> filters_;
			/// File entries.
			Urho3D::Vector<Urho3D::FileSelectorEntry> fileEntries_;
			/// Filter used to get the file list.
			Urho3D::String lastUsedFilter_;
			/// Directory mode flag.
			bool directoryMode_;
			/// Ignore events flag, used when changing line edits manually.
			bool ignoreEvents_;
		};

	}
}