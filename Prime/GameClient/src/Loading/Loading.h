#ifndef _LOADING_LOADING_H
#define _LOADING_LOADING_H

#include <StateObject.h>

namespace Urho3D
{
	class Camera;
	class CustomGeometry;
	class FileSelector;
	class Menu;
	class Node;
	class Scene;
	class Slider;
	class Text;
	class UIElement;
	class Window;
}

namespace Prime
{
	class LoadingState : public StateObject
	{
		OBJECT(LoadingState);
	public:
		LoadingState(Urho3D::Context* context);
		~LoadingState();

	protected: // Internal functions

		void CreateScene();
		void CreateUI();

	protected: // Event handlers

		void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::SharedPtr<Urho3D::Scene> _scene;
		Urho3D::SharedPtr<Urho3D::Node> _cameraNode;

		Urho3D::SharedPtr<Urho3D::UIElement> _uiContainer;
		Urho3D::SharedPtr<Urho3D::Text> _loadingMessage;
		Urho3D::SharedPtr<Urho3D::UIElement> _dialog;
	};
}
#endif