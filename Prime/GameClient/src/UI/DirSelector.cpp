

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

#include "DirSelector.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Container/Sort.h>
#include <Urho3D/DebugNew.h>
#include <Urho3D/UI/FileSelector.h>

namespace Prime
{
	namespace UI
	{
		static bool CompareEntries(const Urho3D::FileSelectorEntry& lhs, const Urho3D::FileSelectorEntry& rhs)
		{
			if (lhs.directory_ && !rhs.directory_)
				return true;
			if (!lhs.directory_ && rhs.directory_)
				return false;
			return lhs.name_.Compare(rhs.name_, false) < 0;
		}

		DirSelector::DirSelector(Urho3D::Context* context) :
			Object(context),
			directoryMode_(false),
			ignoreEvents_(false)
		{
			window_ = new Urho3D::Window(context_);
			window_->SetLayout(Urho3D::LM_VERTICAL);

			titleLayout = new Urho3D::UIElement(context_);
			titleLayout->SetLayout(Urho3D::LM_HORIZONTAL);
			window_->AddChild(titleLayout);

			titleText_ = new Urho3D::Text(context_);
			titleLayout->AddChild(titleText_);

			closeButton_ = new Urho3D::Button(context_);
			titleLayout->AddChild(closeButton_);

			pathEdit_ = new Urho3D::LineEdit(context_);
			window_->AddChild(pathEdit_);

			fileList_ = new Urho3D::ListView(context_);
			window_->AddChild(fileList_);

			fileNameLayout_ = new Urho3D::UIElement(context_);
			fileNameLayout_->SetLayout(Urho3D::LM_HORIZONTAL);

			fileNameEdit_ = new Urho3D::LineEdit(context_);
			fileNameLayout_->AddChild(fileNameEdit_);

			filterList_ = new Urho3D::DropDownList(context_);
			fileNameLayout_->AddChild(filterList_);

			window_->AddChild(fileNameLayout_);

			separatorLayout_ = new Urho3D::UIElement(context_);
			window_->AddChild(separatorLayout_);

			buttonLayout_ = new Urho3D::UIElement(context_);
			buttonLayout_->SetLayout(Urho3D::LM_HORIZONTAL);

			buttonLayout_->AddChild(new Urho3D::UIElement(context_)); // Add spacer

			cancelButton_ = new Urho3D::Button(context_);
			cancelButtonText_ = new Urho3D::Text(context_);
			cancelButtonText_->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
			cancelButton_->AddChild(cancelButtonText_);
			buttonLayout_->AddChild(cancelButton_);

			okButton_ = new Urho3D::Button(context_);
			okButtonText_ = new Urho3D::Text(context_);
			okButtonText_->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
			okButton_->AddChild(okButtonText_);
			buttonLayout_->AddChild(okButton_);

			window_->AddChild(buttonLayout_);

			Urho3D::Vector<Urho3D::String> defaultFilters;
			defaultFilters.Push("*.*");
			SetFilters(defaultFilters, 0);
			Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();
			SetPath(fileSystem->GetCurrentDir());

			// Focus the fileselector's filelist initially when created, and bring to front
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();
			ui->GetRoot()->AddChild(window_);
			ui->SetFocusElement(fileList_);
			window_->SetModal(true);

			SubscribeToEvent(filterList_, Urho3D::E_ITEMSELECTED, HANDLER(DirSelector, HandleFilterChanged));
			SubscribeToEvent(pathEdit_, Urho3D::E_TEXTFINISHED, HANDLER(DirSelector, HandlePathChanged));
			SubscribeToEvent(fileNameEdit_, Urho3D::E_TEXTFINISHED, HANDLER(DirSelector, HandleOKPressed));
			SubscribeToEvent(fileList_, Urho3D::E_ITEMSELECTED, HANDLER(DirSelector, HandleFileSelected));
			SubscribeToEvent(fileList_, Urho3D::E_ITEMDOUBLECLICKED, HANDLER(DirSelector, HandleFileDoubleClicked));
			SubscribeToEvent(fileList_, Urho3D::E_UNHANDLEDKEY, HANDLER(DirSelector, HandleFileListKey));
			SubscribeToEvent(okButton_, Urho3D::E_RELEASED, HANDLER(DirSelector, HandleOKPressed));
			SubscribeToEvent(cancelButton_, Urho3D::E_RELEASED, HANDLER(DirSelector, HandleCancelPressed));
			SubscribeToEvent(closeButton_, Urho3D::E_RELEASED, HANDLER(DirSelector, HandleCancelPressed));
			SubscribeToEvent(window_, Urho3D::E_MODALCHANGED, HANDLER(DirSelector, HandleCancelPressed));
		}

		DirSelector::~DirSelector()
		{
			window_->Remove();
		}

		void DirSelector::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<DirSelector>();
		}

		void DirSelector::SetDefaultStyle(Urho3D::XMLFile* style)
		{
			if (!style)
				return;

			window_->SetDefaultStyle(style);
			window_->SetStyle("FileSelector");

			titleText_->SetStyle("FileSelectorTitleText");
			closeButton_->SetStyle("CloseButton");

			okButtonText_->SetStyle("FileSelectorButtonText");
			cancelButtonText_->SetStyle("FileSelectorButtonText");

			titleLayout->SetStyle("FileSelectorLayout");
			fileNameLayout_->SetStyle("FileSelectorLayout");
			buttonLayout_->SetStyle("FileSelectorLayout");
			separatorLayout_->SetStyle("EditorSeparator");

			fileList_->SetStyle("FileSelectorListView");
			fileNameEdit_->SetStyle("FileSelectorLineEdit");
			pathEdit_->SetStyle("FileSelectorLineEdit");

			filterList_->SetStyle("FileSelectorFilterList");

			okButton_->SetStyle("FileSelectorButton");
			cancelButton_->SetStyle("FileSelectorButton");

			const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::UIElement> >& filterTexts = filterList_->GetListView()->GetContentElement()->GetChildren();
			for (unsigned i = 0; i < filterTexts.Size(); ++i)
				filterTexts[i]->SetStyle("FileSelectorFilterText");

			const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::UIElement> >& listTexts = fileList_->GetContentElement()->GetChildren();
			for (unsigned i = 0; i < listTexts.Size(); ++i)
				listTexts[i]->SetStyle("FileSelectorListText");

			UpdateElements();
		}

		void DirSelector::SetTitle(const Urho3D::String& text)
		{
			titleText_->SetText(text);
		}

		void DirSelector::SetButtonTexts(const Urho3D::String& okText, const Urho3D::String& cancelText)
		{
			okButtonText_->SetText(okText);
			cancelButtonText_->SetText(cancelText);
		}

		void DirSelector::SetPath(const Urho3D::String& path)
		{
			Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();
			if (fileSystem->DirExists(path))
			{
				path_ = AddTrailingSlash(path);
				SetLineEditText(pathEdit_, path_);
				RefreshFiles();
			}
			else
			{
				// If path was invalid, restore the old path to the line edit
				if (pathEdit_->GetText() != path_)
					SetLineEditText(pathEdit_, path_);
			}
		}

		void DirSelector::SetFileName(const Urho3D::String& fileName)
		{
			SetLineEditText(fileNameEdit_, fileName);
		}

		void DirSelector::SetFilters(const Urho3D::Vector<Urho3D::String>& filters, unsigned defaultIndex)
		{
			if (filters.Empty())
				return;

			ignoreEvents_ = true;

			filters_ = filters;
			filterList_->RemoveAllItems();
			for (unsigned i = 0; i < filters_.Size(); ++i)
			{
				Urho3D::Text* filterText = new Urho3D::Text(context_);
				filterList_->AddItem(filterText);
				filterText->SetText(filters_[i]);
				filterText->SetStyle("FileSelectorFilterText");
			}
			if (defaultIndex > filters.Size())
				defaultIndex = 0;
			filterList_->SetSelection(defaultIndex);

			ignoreEvents_ = false;

			if (GetFilter() != lastUsedFilter_)
				RefreshFiles();
		}

		void DirSelector::SetDirectoryMode(bool enable)
		{
			directoryMode_ = enable;
			RefreshFiles();
		}

		void DirSelector::UpdateElements()
		{
			buttonLayout_->SetFixedHeight(Urho3D::Max(okButton_->GetHeight(), cancelButton_->GetHeight()));
		}

		Urho3D::XMLFile* DirSelector::GetDefaultStyle() const
		{
			return window_->GetDefaultStyle(false);
		}

		const Urho3D::String& DirSelector::GetTitle() const
		{
			return titleText_->GetText();
		}

		const Urho3D::String& DirSelector::GetFileName() const
		{
			return fileNameEdit_->GetText();
		}

		const Urho3D::String& DirSelector::GetFilter() const
		{
			Urho3D::Text* selectedFilter = static_cast<Urho3D::Text*>(filterList_->GetSelectedItem());
			if (selectedFilter)
				return selectedFilter->GetText();
			else
				return Urho3D::String::EMPTY;
		}

		unsigned DirSelector::GetFilterIndex() const
		{
			return filterList_->GetSelection();
		}

		void DirSelector::SetLineEditText(Urho3D::LineEdit* edit, const Urho3D::String& text)
		{
			ignoreEvents_ = true;
			edit->SetText(text);
			ignoreEvents_ = false;
		}

		void DirSelector::RefreshFiles()
		{
			Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();

			ignoreEvents_ = true;

			fileList_->RemoveAllItems();
			fileEntries_.Clear();

			Urho3D::Vector<Urho3D::String> directories;
			Urho3D::Vector<Urho3D::String> files;
			fileSystem->ScanDir(directories, path_, "*", Urho3D::SCAN_DIRS, false);
			if (!directoryMode_)
				fileEntries_.Reserve(directories.Size() + files.Size());

			fileSystem->ScanDir(files, path_, GetFilter(), Urho3D::SCAN_FILES, false);



			for (unsigned i = 0; i < directories.Size(); ++i)
			{
				Urho3D::FileSelectorEntry newEntry;
				newEntry.name_ = directories[i];
				newEntry.directory_ = true;
				fileEntries_.Push(newEntry);
			}

			if (!directoryMode_)
				for (unsigned i = 0; i < files.Size(); ++i)
				{
					Urho3D::FileSelectorEntry newEntry;
					newEntry.name_ = files[i];
					newEntry.directory_ = false;
					fileEntries_.Push(newEntry);
				}

			// Sort and add to the list view
			// While items are being added, disable layout update for performance optimization
			Sort(fileEntries_.Begin(), fileEntries_.End(), CompareEntries);
			Urho3D::UIElement* listContent = fileList_->GetContentElement();
			listContent->DisableLayoutUpdate();
			for (unsigned i = 0; i < fileEntries_.Size(); ++i)
			{
				Urho3D::String displayName;
				if (fileEntries_[i].directory_)
					displayName = "<DIR> " + fileEntries_[i].name_;
				else
					displayName = fileEntries_[i].name_;

				Urho3D::Text* entryText = new Urho3D::Text(context_);
				fileList_->AddItem(entryText);
				entryText->SetText(displayName);
				entryText->SetStyle("FileSelectorListText");
			}
			listContent->EnableLayoutUpdate();
			listContent->UpdateLayout();

			ignoreEvents_ = false;

			// Clear filename from the previous dir so that there is no confusion
			SetFileName(Urho3D::String::EMPTY);
			lastUsedFilter_ = GetFilter();
		}

		bool DirSelector::EnterFile()
		{
			unsigned index = fileList_->GetSelection();
			if (index >= fileEntries_.Size())
				return false;

			if (fileEntries_[index].directory_)
			{
				// If a directory double clicked, enter it. Recognize . and .. as a special case
				const Urho3D::String& newPath = fileEntries_[index].name_;
				if ((newPath != ".") && (newPath != ".."))
					SetPath(path_ + newPath);
				else if (newPath == "..")
				{
					Urho3D::String parentPath = GetParentPath(path_);
					SetPath(parentPath);
				}

				return true;
			}
			else
			{
				// Double clicking a file is the same as pressing OK
				if (!directoryMode_)
				{
					using namespace Urho3D::FileSelected;

					Urho3D::VariantMap& eventData = GetEventDataMap();
					eventData[P_FILENAME] = path_ + fileEntries_[index].name_;
					eventData[P_FILTER] = GetFilter();
					eventData[P_OK] = true;
					SendEvent(Urho3D::E_FILESELECTED, eventData);
				}
			}

			return false;
		}

		void DirSelector::HandleFilterChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			if (GetFilter() != lastUsedFilter_)
				RefreshFiles();
		}

		void DirSelector::HandlePathChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			// Attempt to set path. Restores old if does not exist
			SetPath(pathEdit_->GetText());
		}

		void DirSelector::HandleFileSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			unsigned index = fileList_->GetSelection();
			if (index >= fileEntries_.Size())
				return;
			// If a file selected, update the filename edit field
			if (!fileEntries_[index].directory_)
				SetFileName(fileEntries_[index].name_);
		}

		void DirSelector::HandleFileDoubleClicked(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			if (eventData[Urho3D::ItemDoubleClicked::P_BUTTON] == Urho3D::MOUSEB_LEFT)
				EnterFile();
		}

		void DirSelector::HandleFileListKey(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			using namespace Urho3D::UnhandledKey;

			int key = eventData[P_KEY].GetInt();
			if (key == Urho3D::KEY_RETURN || key == Urho3D::KEY_RETURN2 || key == Urho3D::KEY_KP_ENTER)
			{
				bool entered = EnterFile();
				// When a key is used to enter a directory, select the first file if no selection
				if (entered && !fileList_->GetSelectedItem())
					fileList_->SetSelection(0);
			}
		}

		void DirSelector::HandleOKPressed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			const Urho3D::String& fileName = GetFileName();

			if (!directoryMode_)
			{
				if (!fileName.Empty())
				{
					using namespace Urho3D::FileSelected;

					Urho3D::VariantMap& newEventData = GetEventDataMap();
					newEventData[P_FILENAME] = path_ + GetFileName();
					newEventData[P_FILTER] = GetFilter();
					newEventData[P_OK] = true;
					SendEvent(Urho3D::E_FILESELECTED, newEventData);
				}
			}
			else if (eventType == Urho3D::E_RELEASED && !path_.Empty())
			{
				using namespace Urho3D::FileSelected;

				Urho3D::VariantMap& newEventData = GetEventDataMap();
				newEventData[P_FILENAME] = path_;
				newEventData[P_FILTER] = GetFilter();
				newEventData[P_OK] = true;
				SendEvent(Urho3D::E_FILESELECTED, newEventData);
			}
		}

		void DirSelector::HandleCancelPressed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			if (ignoreEvents_)
				return;

			if (eventType == Urho3D::E_MODALCHANGED && eventData[Urho3D::ModalChanged::P_MODAL].GetBool())
				return;

			using namespace Urho3D::FileSelected;

			Urho3D::VariantMap& newEventData = GetEventDataMap();
			newEventData[P_FILENAME] = Urho3D::String::EMPTY;
			newEventData[P_FILTER] = GetFilter();
			newEventData[P_OK] = false;
			SendEvent(Urho3D::E_FILESELECTED, newEventData);
		}

	}
}