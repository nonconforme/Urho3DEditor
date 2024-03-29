#include "GizmoScene3D.h"
#include "EditorData.h"
#include "EditorSelection.h"
#include "EPScene3D.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Graphics.h>

namespace Prime
{
	namespace Editor
	{
		const float axisMaxD = 0.1f;
		const float axisMaxT = 1.0f;
		const float rotSensitivity = 50.0f;

		GizmoScene3D::GizmoScene3D(Urho3D::Context* context, EPScene3D* epScene3D) : Object(context)
		{
			gizmoAxisX = new GizmoAxis(context);
			gizmoAxisY = new GizmoAxis(context);
			gizmoAxisZ = new GizmoAxis(context);
			epScene3D_ = epScene3D;

			editorData_ = GetSubsystem<EditorData>();
			editorSelection_ = GetSubsystem<EditorSelection>();
		}

		GizmoScene3D::~GizmoScene3D()
		{
		}

		void GizmoScene3D::CreateGizmo()
		{
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

			gizmoNode = new Urho3D::Node(context_);
			gizmo = gizmoNode->CreateComponent<Urho3D::StaticModel>();
			gizmo->SetModel(cache->GetResource<Urho3D::Model>("Models/Editor/Axes.mdl"));
			gizmo->SetMaterial(0, cache->GetResource<Urho3D::Material>("Materials/Editor/RedUnlit.xml"));
			gizmo->SetMaterial(1, cache->GetResource<Urho3D::Material>("Materials/Editor/GreenUnlit.xml"));
			gizmo->SetMaterial(2, cache->GetResource<Urho3D::Material>("Materials/Editor/BlueUnlit.xml"));
			gizmo->SetEnabled(false);
			gizmo->SetViewMask(0x80000000); // Editor raycasts use viewmask 0x7fffffff
			gizmo->SetOccludee(false);

			gizmoAxisX->lastSelected = false;
			gizmoAxisY->lastSelected = false;
			gizmoAxisZ->lastSelected = false;
			lastGizmoMode = EDIT_MOVE;
		}

		void GizmoScene3D::HideGizmo()
		{
			if (gizmo != NULL)
				gizmo->SetEnabled(false);
		}

		void GizmoScene3D::ShowGizmo()
		{
			if (gizmo != NULL)
			{
				gizmo->SetEnabled(true);

				// Because setting enabled = false detaches the gizmo from octree,
				// and it is a manually added drawable, must readd to octree when showing
				if (editorData_->GetEditorScene()->GetComponent<Urho3D::Octree>() != NULL)
					editorData_->GetEditorScene()->GetComponent<Urho3D::Octree>()->AddManualDrawable(gizmo);
			}
		}

		void GizmoScene3D::UpdateGizmo()
		{
			UseGizmo();
			PositionGizmo();
			ResizeGizmo();
		}

		void GizmoScene3D::PositionGizmo()
		{
			if (gizmo == NULL)
				return;
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			Urho3D::Vector3 center(0.0f, 0.0f, 0.0f);
			bool containsScene = false;

			for (unsigned int i = 0; i < editorSelection_->GetNumEditNodes(); ++i)
			{
				// Scene's transform should not be edited, so hide gizmo if it is included
				if (editorSelection_->GetEditNodes()[i] == editorData_->GetEditorScene())
				{
					containsScene = true;
					break;
				}
				center += editorSelection_->GetEditNodes()[i]->GetWorldPosition();
			}

			if (editorSelection_->GetEditNodes().Empty() || containsScene)
			{
				HideGizmo();
				return;
			}

			center /= (float)editorSelection_->GetNumEditNodes();
			gizmoNode->SetPosition(center);

			if (epScene3D_->axisMode == AXIS_WORLD || editorSelection_->GetNumEditNodes() > 1)
				gizmoNode->SetRotation(Urho3D::Quaternion());
			else
				gizmoNode->SetRotation(editorSelection_->GetEditNodes()[0]->GetWorldRotation());

			if (epScene3D_->editMode != lastGizmoMode)
			{
				switch (epScene3D_->editMode)
				{
				case EDIT_MOVE:
					gizmo->SetModel(cache->GetResource<Urho3D::Model>("Models/Editor/Axes.mdl"));
					break;

				case EDIT_ROTATE:
					gizmo->SetModel(cache->GetResource<Urho3D::Model>("Models/Editor/RotateAxes.mdl"));
					break;

				case EDIT_SCALE:
					gizmo->SetModel(cache->GetResource<Urho3D::Model>("Models/Editor/ScaleAxes.mdl"));

					break;
				}

				lastGizmoMode = epScene3D_->editMode;
			}

			if ((epScene3D_->editMode != EDIT_SELECT && !epScene3D_->orbiting) && !gizmo->IsEnabled())
				ShowGizmo();
			else if ((epScene3D_->editMode == EDIT_SELECT || epScene3D_->orbiting) && gizmo->IsEnabled())
				HideGizmo();
		}

		void GizmoScene3D::ResizeGizmo()
		{
			if (gizmo == NULL || !gizmo->IsEnabled())
				return;

			float scale = 0.1f / epScene3D_->camera_->GetZoom();

			if (epScene3D_->camera_->IsOrthographic())
				scale *= epScene3D_->camera_->GetOrthoSize();
			else
				scale *= (epScene3D_->camera_->GetView() * gizmoNode->GetPosition()).z_;

			gizmoNode->SetScale(Urho3D::Vector3(scale, scale, scale));
		}

		void GizmoScene3D::CalculateGizmoAxes()
		{
			gizmoAxisX->axisRay = Urho3D::Ray(gizmoNode->GetPosition(), gizmoNode->GetRotation() * Urho3D::Vector3(1, 0, 0));
			gizmoAxisY->axisRay = Urho3D::Ray(gizmoNode->GetPosition(), gizmoNode->GetRotation() * Urho3D::Vector3(0, 1, 0));
			gizmoAxisZ->axisRay = Urho3D::Ray(gizmoNode->GetPosition(), gizmoNode->GetRotation() * Urho3D::Vector3(0, 0, 1));
		}

		void GizmoScene3D::GizmoMoved()
		{
			gizmoAxisX->Moved();
			gizmoAxisY->Moved();
			gizmoAxisZ->Moved();
		}

		void GizmoScene3D::UseGizmo()
		{
			if (gizmo == NULL || !gizmo->IsEnabled() || epScene3D_->editMode == EDIT_SELECT)
			{
				// 			StoreGizmoEditActions();
				// 			previousGizmoDrag = false;
				return;
			}
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

			Urho3D::IntVector2 pos = ui->GetCursorPosition();
			Urho3D::UIElement* e = ui->GetElementAt(pos);

			if (e != epScene3D_->activeView)
				return;

			const Urho3D::IntVector2& screenpos = epScene3D_->activeView->GetScreenPosition();
			float	posx = float(pos.x_ - screenpos.x_) / float(epScene3D_->activeView->GetWidth());
			float	posy = float(pos.y_ - screenpos.y_) / float(epScene3D_->activeView->GetHeight());

			Urho3D::Ray cameraRay = epScene3D_->camera_->GetScreenRay(posx, posy);
			float scale = gizmoNode->GetScale().x_;

			Urho3D::Input* input = GetSubsystem<Urho3D::Input>();
			// Recalculate axes only when not left-dragging
			bool drag = input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT);
			if (!drag)
				CalculateGizmoAxes();

			gizmoAxisX->Update(cameraRay, scale, drag, epScene3D_->cameraNode_->GetPosition());
			gizmoAxisY->Update(cameraRay, scale, drag, epScene3D_->cameraNode_->GetPosition());
			gizmoAxisZ->Update(cameraRay, scale, drag, epScene3D_->cameraNode_->GetPosition());
			Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
			if (gizmoAxisX->selected != gizmoAxisX->lastSelected)
			{
				gizmo->SetMaterial(0, cache->GetResource<Urho3D::Material>(gizmoAxisX->selected ? "Materials/Editor/BrightRedUnlit.xml" : "Materials/Editor/RedUnlit.xml"));
				gizmoAxisX->lastSelected = gizmoAxisX->selected;
			}
			if (gizmoAxisY->selected != gizmoAxisY->lastSelected)
			{
				gizmo->SetMaterial(1, cache->GetResource<Urho3D::Material>(gizmoAxisY->selected ? "Materials/Editor/BrightGreenUnlit.xml" : "Materials/Editor/GreenUnlit.xml"));
				gizmoAxisY->lastSelected = gizmoAxisY->selected;
			}
			if (gizmoAxisZ->selected != gizmoAxisZ->lastSelected)
			{
				gizmo->SetMaterial(2, cache->GetResource<Urho3D::Material>(gizmoAxisZ->selected ? "Materials/Editor/BrightBlueUnlit.xml" : "Materials/Editor/BlueUnlit.xml"));
				gizmoAxisZ->lastSelected = gizmoAxisZ->selected;
			};

			if (drag)
			{
				// Store initial transforms for undo when gizmo drag started
				// 			if (!previousGizmoDrag)
				// 			{
				// 				oldGizmoTransforms.Resize(editNodes.length);
				// 				for (uint i = 0; i < editNodes.length; ++i)
				// 					oldGizmoTransforms[i].Define(editNodes[i]);
				// 			}

				bool moved = false;

				if (epScene3D_->editMode == EDIT_MOVE)
				{
					Urho3D::Vector3 adjust(0, 0, 0);
					if (gizmoAxisX->selected)
						adjust += Urho3D::Vector3(1, 0, 0) * (gizmoAxisX->t - gizmoAxisX->lastT);
					if (gizmoAxisY->selected)
						adjust += Urho3D::Vector3(0, 1, 0) * (gizmoAxisY->t - gizmoAxisY->lastT);
					if (gizmoAxisZ->selected)
						adjust += Urho3D::Vector3(0, 0, 1) * (gizmoAxisZ->t - gizmoAxisZ->lastT);

					moved = epScene3D_->MoveNodes(adjust);
				}
				else if (epScene3D_->editMode == EDIT_ROTATE)
				{
					Urho3D::Vector3 adjust(0, 0, 0);
					if (gizmoAxisX->selected)
						adjust.x_ = (gizmoAxisX->d - gizmoAxisX->lastD) * rotSensitivity / scale;
					if (gizmoAxisY->selected)
						adjust.y_ = -(gizmoAxisY->d - gizmoAxisY->lastD) * rotSensitivity / scale;
					if (gizmoAxisZ->selected)
						adjust.z_ = (gizmoAxisZ->d - gizmoAxisZ->lastD) * rotSensitivity / scale;

					moved = epScene3D_->RotateNodes(adjust);
				}
				else if (epScene3D_->editMode == EDIT_SCALE)
				{
					Urho3D::Vector3 adjust(0, 0, 0);
					if (gizmoAxisX->selected)
						adjust += Urho3D::Vector3(1, 0, 0) * (gizmoAxisX->t - gizmoAxisX->lastT);
					if (gizmoAxisY->selected)
						adjust += Urho3D::Vector3(0, 1, 0) * (gizmoAxisY->t - gizmoAxisY->lastT);
					if (gizmoAxisZ->selected)
						adjust += Urho3D::Vector3(0, 0, 1) * (gizmoAxisZ->t - gizmoAxisZ->lastT);

					// Special handling for uniform scale: use the unmodified X-axis movement only
					if (epScene3D_->editMode == EDIT_SCALE && gizmoAxisX->selected && gizmoAxisY->selected && gizmoAxisZ->selected)
					{
						float x = gizmoAxisX->t - gizmoAxisX->lastT;
						adjust = Urho3D::Vector3(x, x, x);
					}

					moved = epScene3D_->ScaleNodes(adjust);
				}

				if (moved)
				{
					GizmoMoved();
					// 				UpdateNodeAttributes();
					// 				needGizmoUndo = true;
				}
			}
			else
			{
				// 			if (previousGizmoDrag)
				// 				StoreGizmoEditActions();
			}

			/*		previousGizmoDrag = drag;*/
		}

		bool GizmoScene3D::IsGizmoSelected()
		{
			return gizmo != NULL && gizmo->IsEnabled() && (gizmoAxisX->selected || gizmoAxisY->selected || gizmoAxisZ->selected);
		}

		GizmoAxis::GizmoAxis(Urho3D::Context* context) : Object(context)
		{
			selected = false;
			lastSelected = false;
			t = 0.0;
			d = 0.0;
			lastT = 0.0;
			lastD = 0.0;
		}

		GizmoAxis::~GizmoAxis()
		{
		}

		void GizmoAxis::Update(Urho3D::Ray cameraRay, float scale, bool drag, const Urho3D::Vector3& camPos)
		{
			Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

			// Do not select when UI has modal element
			if (ui->HasModalElement())
			{
				selected = false;
				return;
			}

			Urho3D::Vector3 closest = cameraRay.ClosestPoint(axisRay);
			Urho3D::Vector3 projected = axisRay.Project(closest);
			d = axisRay.Distance(closest);
			t = (projected - axisRay.origin_).DotProduct(axisRay.direction_);

			// Determine the sign of d from a plane that goes through the camera position to the axis
			Urho3D::Plane axisPlane(camPos, axisRay.origin_, axisRay.origin_ + axisRay.direction_);
			if (axisPlane.Distance(closest) < 0.0)
				d = -d;

			// Update selected status only when not dragging
			if (!drag)
			{
				selected = (Urho3D::Abs(d) < axisMaxD * scale) && (t >= -axisMaxD * scale) && (t <= axisMaxT * scale);
				lastT = t;
				lastD = d;
			}
		}

		void GizmoAxis::Moved()
		{
			lastT = t;
			lastD = d;
		}
	}
}