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

#include "UIGlobals.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Core/Attribute.h>

namespace Urho3D
{
	class FileSystem;
	class Menu;
	class XMLFile;
	class Text;
	class Window;
	class UIElement;
	class CheckBox;
	class Component;
	class Node;
	class Button;
	class ListView;
	class Serializable;
	class LineEdit;
}

namespace Prime
{
	namespace UI
	{
		namespace UIUtils
		{
			//////////////////////////////////////////////////////////////////////////
			///  Editor UI Creation Function
			//////////////////////////////////////////////////////////////////////////
			void		CreateDir(const Urho3D::Object* obj, const Urho3D::String& pathName, Urho3D::String baseDir = Urho3D::String::EMPTY);
			Urho3D::Menu*		CreateMenu(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, const Urho3D::String& title);
			Urho3D::Menu*		CreateMenuItem(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, const Urho3D::String& title, Urho3D::EventHandler* handler = NULL, int accelKey = 0, int accelQual = 0, bool addToQuickMenu = true, Urho3D::String quickMenuText = "");
			Urho3D::Text*		CreateAccelKeyText(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, int accelKey, int accelQual);
			Urho3D::Window*		CreatePopup(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, Urho3D::Menu* baseMenu);
			void		FinalizedPopupMenu(Urho3D::Window* popup);
			void		CreateChildDivider(Urho3D::UIElement* parent);
			Urho3D::UIElement*	CreateGroup(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, const Urho3D::String& title, Urho3D::LayoutMode layoutMode);
			Urho3D::CheckBox*	CreateToolBarToggle(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_, const Urho3D::String& title);
			void		CreateToolBarIcon(Urho3D::Context* context, Urho3D::XMLFile* iconStyle_, Urho3D::UIElement* element);
			Urho3D::UIElement*	CreateToolTip(Urho3D::UIElement* parent, const Urho3D::String& title, const Urho3D::IntVector2& offset);
			void		FinalizeGroupHorizontal(Urho3D::UIElement* group, const Urho3D::String& baseStyle);
			Urho3D::Button*		CreateSmallToolBarButton(Urho3D::Context* context, Urho3D::XMLFile* uiStyle, Urho3D::XMLFile* iconStyle_, const Urho3D::String& title, Urho3D::String toolTipTitle = "");
			void		CreateSmallToolBarIcon(Urho3D::Context* context, Urho3D::XMLFile* iconStyle_, Urho3D::UIElement* element);
			Urho3D::UIElement*	CreateSmallToolBarSpacer(unsigned int width);

			//////////////////////////////////////////////////////////////////////////
			///  Hierarchy Window Function
			//////////////////////////////////////////////////////////////////////////
			Urho3D::String	GetUIElementTitle(Urho3D::UIElement* element);
			Urho3D::String	GetComponentTitle(Urho3D::Component* component);
			Urho3D::String	GetNodeTitle(Urho3D::Node* node);
			Urho3D::Variant		GetUIElementID(Urho3D::UIElement* element);
			unsigned int	GetID(Urho3D::Serializable* serializable, int itemType = ITEM_NONE);
			int		GetType(Urho3D::Serializable* serializable);
			bool	MatchID(Urho3D::UIElement* element, const Urho3D::Variant& id, int itemType);
			void	SetIconEnabledColor(Urho3D::UIElement* element, bool enabled, bool partial = false);

			void	IconizeUIElement(Urho3D::Context* context, Urho3D::XMLFile* iconStyle_, Urho3D::UIElement* element, const Urho3D::String& iconType);
			Urho3D::Vector<Urho3D::Serializable*> ToSerializableArray(Urho3D::Vector<Urho3D::Node*> nodes);

			//////////////////////////////////////////////////////////////////////////
			///  Attribute Window Function
			//////////////////////////////////////////////////////////////////////////
			Urho3D::UIElement*	CreateAttributeEditorParent(Urho3D::ListView* list, const Urho3D::String& name, unsigned int index, unsigned int subIndex);
			Urho3D::UIElement*	CreateAttributeEditorParentAsListChild(Urho3D::ListView* list, const Urho3D::String& name, unsigned int index, unsigned int subIndex);
			Urho3D::LineEdit*	CreateAttributeLineEdit(Urho3D::UIElement* parent, Urho3D::Vector<Urho3D::Serializable*>& serializables, unsigned int index, unsigned int subIndex);
			void		SetAttributeEditorID(Urho3D::UIElement* attrEdit, Urho3D::Vector<Urho3D::Serializable*>& serializables);
			void		SetAttributeEditorID(Urho3D::UIElement* attrEdit, Urho3D::Serializable* serializables);
			Urho3D::UIElement*	CreateBoolAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);
			Urho3D::UIElement*	CreateStringAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);
			Urho3D::UIElement*	CreateNumAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);
			Urho3D::UIElement*	CreateIntAttributeEditor(Urho3D::ListView* list, Urho3D::Vector<Urho3D::Serializable*>& serializables, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);

			/// scene Editor, menu bar actions
			Urho3D::String ExtractFileName(Urho3D::VariantMap& eventData, bool forSave = false);

		}
	}
}