#include "EditorPlugin.h"

#include <Urho3D/Urho3D.h>

namespace Prime
{
	namespace Editor
	{
		EditorPlugin::EditorPlugin(Urho3D::Context* context, EditorFramePosition framePosition) : Object(context),
			visible_(false),
			_framePosition(framePosition)
		{

		}

		EditorPlugin::~EditorPlugin()
		{

		}
	}

}