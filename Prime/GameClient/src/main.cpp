// Uses WinMain as our Entry Point
// NOTE: Our game DLL must be compiled with the same define

#define _USE_DLL
//#define _USE_WINDOWS_ENTRY_POINT

#include "Platform.h"
#include <Windows.h>
#include "Root.h"

#ifndef _USE_DLL
int main(int argc, char* argv[])
#else
	#ifndef _USE_WINDOWS_ENTRY_POINT
		#define _CONSOLE
		#pragma comment(linker,"/SUBSYSTEM:CONSOLE")
		DLL_EXPORT int GameMain(int argc, char* argv[])
	#else
		#pragma comment(linker,"/SUBSYSTEM:WINDOWS")
		DLL_EXPORT int GameMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
	#endif
#endif
{
	//Urho3D::SharedPtr<Urho3D::Context> content(new Urho3D::Context());
	//Urho3D::SharedPtr<Game> game(new Game(content));

	Prime::Root::instance()->Run();

	return 0;
}