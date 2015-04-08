#ifndef _EDITOR_GIZMOSCENE3D_H
#define _EDITOR_GIZMOSCENE3D_H

#pragma once

#include "EPScene3D.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Math/Ray.h>

namespace Urho3D
{
	class Node;
	class StaticModel;
	class Scene;
}

namespace Prime
{
	namespace Editor
	{
		
		class EditorData;
		class EditorSelection;
		class EPScene3D;

		class GizmoAxis : public Urho3D::Object
		{
			OBJECT(GizmoAxis);
		public:
			/// Construct.
			GizmoAxis(Urho3D::Context* context);
			/// Destruct.
			virtual ~GizmoAxis();

			void Update(Urho3D::Ray cameraRay, float scale, bool drag, const Urho3D::Vector3& camPos);

			void Moved();

			Urho3D::Ray axisRay;
			bool selected;
			bool lastSelected;
			float t;
			float d;
			float lastT;
			float lastD;
		};


		class GizmoScene3D : public Urho3D::Object
		{
			OBJECT(GizmoScene3D);
		public:
			/// Construct.
			GizmoScene3D(Urho3D::Context* context, EPScene3D* epScene3D);
			/// Destruct.
			virtual ~GizmoScene3D();
			void CreateGizmo();
			void HideGizmo();
			void ShowGizmo();

			void UpdateGizmo();

			void PositionGizmo();
			void ResizeGizmo();

			void CalculateGizmoAxes();

			void GizmoMoved();
			void UseGizmo();
			bool IsGizmoSelected();
		protected:
			EditorData*			editorData_;
			EditorSelection*	editorSelection_;

			Urho3D::SharedPtr<Urho3D::Node> gizmoNode;
			Urho3D::SharedPtr<Urho3D::StaticModel> gizmo;
			EditMode lastGizmoMode;

			Urho3D::SharedPtr<GizmoAxis> gizmoAxisX;
			Urho3D::SharedPtr<GizmoAxis> gizmoAxisY;
			Urho3D::SharedPtr<GizmoAxis> gizmoAxisZ;
			EPScene3D* epScene3D_;
			// For undo
			// 		bool previousGizmoDrag;
			// 		bool needGizmoUndo;
			// 		Array<Transform> oldGizmoTransforms;
		};
	}
}

#endif