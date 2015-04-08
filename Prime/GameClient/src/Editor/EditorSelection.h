#ifndef _EDITOR_EDITORSELECTION_H
#define _EDITOR_EDITORSELECTION_H

#pragma once

#include "../UI/Macros.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>

namespace Urho3D
{
	class UIElement;
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
	class Camera;
}

namespace Prime
{
	namespace UI
	{
		class MenuBarUI;
		class ToolBarUI;
		class MiniToolBarUI;
		class AttributeWindowUI;
		class AttributeInspector;
		class HierarchyWindow;
		class ResourceCache;
	}

	namespace Editor
	{
		class EditorState;
		class ProjectManager;

		class EditorSelection : public Urho3D::Object
		{
			OBJECT(EditorSelection);
		public:
			EditorSelection(Urho3D::Context* context, EditorState* editor = NULL);
			virtual ~EditorSelection();
			static void RegisterObject(Urho3D::Context* context);
			///  Selection 
			void ClearSelection();


			void AddSelectedComponent(Urho3D::Component* comp);
			void AddSelectedNode(Urho3D::Node* node);
			void AddEditComponent(Urho3D::Component* comp);
			void AddEditNode(Urho3D::Node* node);
			void AddSelectedUIElement(Urho3D::UIElement* element);
			void AddEditUIElement(Urho3D::UIElement* element);

			unsigned GetNumSelectedUIElements();
			unsigned GetNumSelectedComponents();
			unsigned GetNumSelectedNodes();
			unsigned GetNumEditComponents();
			unsigned GetNumEditNodes();

			Urho3D::Vector<Urho3D::Node*>&		GetSelectedNodes();
			Urho3D::Vector<Urho3D::Component*>&	GetSelectedComponents();
			Urho3D::Vector<Urho3D::UIElement*>&	GetSelectedUIElements();
			Urho3D::Vector<Urho3D::Node*>&		GetEditNodes();
			Urho3D::Vector<Urho3D::Component*>&	GetEditComponents();
			Urho3D::UIElement*			GetEditUIElement();
			Urho3D::Vector<Urho3D::UIElement*>	GetEditUIElements();
			Urho3D::Node*				GetEditNode();
			unsigned int		GetNumEditableComponentsPerNode();

			void	SetEditNode(Urho3D::Node* node);
			void	SetEditUIElement(Urho3D::UIElement* element);
			void	SetSelectedNodes(Urho3D::Vector<Urho3D::Node*>& nodes);
			void	SetSelectedComponents(Urho3D::Vector<Urho3D::Component*>& comps);
			void	SetSelectedUIElements(Urho3D::Vector<Urho3D::UIElement*>& elemets);
			void	SetEditNodes(Urho3D::Vector<Urho3D::Node*>& nodes);
			void	SetEditComponents(Urho3D::Vector<Urho3D::Component*>& comps);
			void	SetEditUIElements(Urho3D::Vector<Urho3D::UIElement*>& elements);
			void	SetNumEditableComponentsPerNode(unsigned int num);

			void			SetGlobalVarNames(const Urho3D::String& name);
			const Urho3D::Variant&	GetGlobalVarNames(Urho3D::StringHash& name);

			void OnHierarchyListSelectionChange(const Urho3D::PODVector<Urho3D::UIElement*>& items, const Urho3D::PODVector<unsigned>& indices);
		protected:
			/// Selection
			Urho3D::Vector<Urho3D::Node*>		selectedNodes_;
			Urho3D::Vector<Urho3D::Component*>	selectedComponents_;
			Urho3D::Vector<Urho3D::UIElement*>	selectedUIElements_;

			Urho3D::UIElement*	editUIElement_;
			Urho3D::Node*		editNode_;
			EditorState* editor_;
			Urho3D::Vector<Urho3D::Node*>		editNodes_;
			Urho3D::Vector<Urho3D::Component*>	editComponents_;
			Urho3D::Vector<Urho3D::UIElement*>	editUIElements_;

			unsigned int numEditableComponentsPerNode_;

			// Node or UIElement hash-to-varname reverse mapping
			Urho3D::VariantMap globalVarNames_;



		};

	}
}

#endif