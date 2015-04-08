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
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Math/StringHash.h>


namespace Prime
{
	namespace UI
	{
		const Urho3D::StringHash ACTION_VAR("Action");
		const Urho3D::StringHash A_UNDO_VAR("UndoAction");
		const Urho3D::StringHash A_REDO_VAR("RedoAction");
		const Urho3D::StringHash A_QUITEDITOR_VAR("QuitEditorAction");
		const Urho3D::StringHash A_SHOWATTRIBUTE_VAR("ShowAttributeAction");
		const Urho3D::StringHash A_SHOWHIERARCHY_VAR("ShowHierarchyAction");
		const Urho3D::StringHash A_SHOWRESOURCE_VAR("ShowResourceBrowser");

		const Urho3D::StringHash A_NEWSCENE_VAR("NewScene");
		const Urho3D::StringHash A_OPENSCENE_VAR("OpenScene");
		const Urho3D::StringHash A_SAVESCENE_VAR("SaveScene");
		const Urho3D::StringHash A_SAVESCENEAS_VAR("SaveSceneAs");

		const Urho3D::StringHash A_LOADNODEASREP_VAR("LoadNodeAsRep");
		const Urho3D::StringHash A_LOADNODEASLOCAL_VAR("LoadNodeAsLocal");
		const Urho3D::StringHash A_SAVENODEAS_VAR("SaveNodeAs");

		const Urho3D::StringHash A_CREATELOCALNODE_VAR("CreateLocalNode");
		const Urho3D::StringHash A_CREATEREPNODE_VAR("CreateRepNode");

		const Urho3D::StringHash A_CREATECOMPONENT_VAR("CreateComponent");
		const Urho3D::StringHash A_CREATEBUILTINOBJ_VAR("CreateBuiltinObject");

		const int PICK_GEOMETRIES = 0;
		const int PICK_LIGHTS = 1;
		const int PICK_ZONES = 2;
		const int PICK_RIGIDBODIES = 3;
		const int PICK_UI_ELEMENTS = 4;
		const int MAX_PICK_MODES = 5;
		const int MAX_UNDOSTACK_SIZE = 256;

		const Urho3D::StringHash FILENAME_VAR("FileName");
		const Urho3D::StringHash MODIFIED_VAR("Modified");
		const Urho3D::StringHash CHILD_ELEMENT_FILENAME_VAR("ChildElemFileName");

		const Urho3D::StringHash UI_ELEMENT_TYPE("UIElement");
		const Urho3D::StringHash WINDOW_TYPE("Window");
		const Urho3D::StringHash MENU_TYPE("Menu");
		const Urho3D::StringHash TEXT_TYPE("Text");
		const Urho3D::StringHash CURSOR_TYPE("Cursor");
		const Urho3D::StringHash VIEW_MODE("VIEW_MODE");

		const Urho3D::StringHash EDITOR_TABID("Editor_TabID");

		const Urho3D::String AUTO_STYLE("");    // Empty string means auto style, i.e. applying style according to UI-element's type automatically
		const Urho3D::String TEMP_SCENE_NAME("_tempscene_.xml");
		const Urho3D::StringHash CALLBACK_VAR("Callback");
		const Urho3D::StringHash INDENT_MODIFIED_BY_ICON_VAR("IconIndented");

		const int SHOW_POPUP_INDICATOR = -1;
		const unsigned int MAX_QUICK_MENU_ITEMS = 10;
		const unsigned int maxRecentSceneCount = 5;

		//////////////////////////////////////////////////////////////////////////
		/// EditorHierarchyWindow  const
		const int ITEM_NONE = 0;
		const int ITEM_NODE = 1;
		const int ITEM_COMPONENT = 2;
		const int ITEM_UI_ELEMENT = 3;
		const unsigned int NO_ITEM = Urho3D::M_MAX_UNSIGNED;

		const Urho3D::StringHash SCENE_TYPE("Scene");
		const Urho3D::StringHash NODE_TYPE("Node");
		const Urho3D::StringHash STATICMODELGROUP_TYPE("StaticModelGroup");
		const Urho3D::StringHash SPLINEPATH_TYPE("SplinePath");
		const Urho3D::StringHash CONSTRAINT_TYPE("Constraint");

		const Urho3D::String NO_CHANGE(char(0));
		const Urho3D::StringHash TYPE_VAR("Type");

		const Urho3D::StringHash NODE_ID_VAR("NodeID");
		const Urho3D::StringHash COMPONENT_ID_VAR("ComponentID");
		const Urho3D::StringHash UI_ELEMENT_ID_VAR("UIElementID");
		const Urho3D::StringHash DRAGDROPCONTENT_VAR("DragDropContent");
		const Urho3D::StringHash ID_VARS[] = { Urho3D::StringHash(""), NODE_ID_VAR, COMPONENT_ID_VAR, UI_ELEMENT_ID_VAR };

		//////////////////////////////////////////////////////////////////////////
		/// EditorAttributeInspectorWindow const
		const Urho3D::String STRIKED_OUT = "--";
		const Urho3D::StringHash NODE_IDS_VAR("NodeIDs");
		const Urho3D::StringHash COMPONENT_IDS_VAR("ComponentIDs");
		const Urho3D::StringHash UI_ELEMENT_IDS_VAR("UIElementIDs");
		const int LABEL_WIDTH = 30;

		const unsigned int ATTRIBUTE_RES = 0;
		const unsigned int VARIABLE_RES = 1;
		const unsigned int STYLE_RES = 2;

		const unsigned int MIN_NODE_ATTRIBUTES = 4;
		const unsigned int MAX_NODE_ATTRIBUTES = 8;
		const int ATTRNAME_WIDTH = 150;
		const int ATTR_HEIGHT = 19;
		const Urho3D::StringHash ATTR_COORDINATE("Coordinate");
		const Urho3D::StringHash TEXT_CHANGED_EVENT_TYPE("TextChanged");

		const unsigned int UI_ELEMENT_BASE_ID = 1;

		//////////////////////////////////////////////////////////////////////////
		/// Editor View
		const unsigned int VIEWPORT_BORDER_H = 0x00000001;
		const unsigned int VIEWPORT_BORDER_H1 = 0x00000002;
		const unsigned int VIEWPORT_BORDER_H2 = 0x00000004;
		const unsigned int VIEWPORT_BORDER_V = 0x00000010;
		const unsigned int VIEWPORT_BORDER_V1 = 0x00000020;
		const unsigned int VIEWPORT_BORDER_V2 = 0x00000040;

		const unsigned int VIEWPORT_SINGLE = 0x00000000;
		const unsigned int VIEWPORT_TOP = 0x00000100;
		const unsigned int VIEWPORT_BOTTOM = 0x00000200;
		const unsigned int VIEWPORT_LEFT = 0x00000400;
		const unsigned int VIEWPORT_RIGHT = 0x00000800;
		const unsigned int VIEWPORT_TOP_LEFT = 0x00001000;
		const unsigned int VIEWPORT_TOP_RIGHT = 0x00002000;
		const unsigned int VIEWPORT_BOTTOM_LEFT = 0x00004000;
		const unsigned int VIEWPORT_BOTTOM_RIGHT = 0x00008000;

		// Combinations for easier testing
		const unsigned int VIEWPORT_BORDER_H_ANY = 0x00000007;
		const unsigned int VIEWPORT_BORDER_V_ANY = 0x00000070;
		const unsigned int VIEWPORT_SPLIT_H = 0x0000f300;
		const unsigned int VIEWPORT_SPLIT_V = 0x0000fc00;
		const unsigned int VIEWPORT_SPLIT_HV = 0x0000f000;
		const unsigned int VIEWPORT_TOP_ANY = 0x00003300;
		const unsigned int VIEWPORT_BOTTOM_ANY = 0x0000c200;
		const unsigned int VIEWPORT_LEFT_ANY = 0x00005400;
		const unsigned int VIEWPORT_RIGHT_ANY = 0x0000c800;
		const unsigned int VIEWPORT_QUAD = 0x0000f000;


	}
}