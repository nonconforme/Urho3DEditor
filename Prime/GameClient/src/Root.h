#ifndef _ROOT_H
#define _ROOT_H

#include <string>
#include <memory>

#include <Urho3D/Urho3D.h>

namespace Urho3D
{
	class Engine;
	class Context;
}

namespace Prime
{

	class Root
	{
	public:
		static Root* instance();

		Urho3D::Engine* GetEngine() { return _engine; }
		Urho3D::Context* GetContext() { return _context; }

		void Run();
		void Stop();

	private:
		Root();
		~Root();

		Urho3D::Context* _context;
		Urho3D::Engine* _engine;
		bool m_bStop;

		static bool g_initialized;
		static Root g_instance;
	};
}

#endif