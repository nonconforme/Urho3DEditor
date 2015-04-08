// Uses WinMain as our Entry Point
// NOTE: Our game DLL must be compiled with the same define
//#define _USE_WINDOWS_ENTRY_POINT

#include <Windows.h>
#include <iostream>
#include <stdio.h>

#ifndef _USE_WINDOWS_ENTRY_POINT
typedef int (*GameMain)(int argc, char* argv[]);
#else
typedef int (*GameMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );
#endif

#ifndef _USE_WINDOWS_ENTRY_POINT
#define _CONSOLE
#pragma comment(linker,"/SUBSYSTEM:CONSOLE")
int main(int argc, char* argv[])
#else
#pragma comment(linker,"/SUBSYSTEM:WINDOWS")
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
#endif
{
	TCHAR currentDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDirectory);
	 
	char proxyBuffer[4096];
	_snprintf(proxyBuffer, sizeof(proxyBuffer), "%s//bin//GameClient.dll", currentDirectory); // Build the full path to our game proxy
	proxyBuffer[sizeof(proxyBuffer)-1] = '/0'; // Null terminate the string as this is not done for us

	HINSTANCE gameInstance = LoadLibraryEx(proxyBuffer, NULL, LOAD_WITH_ALTERED_SEARCH_PATH); // Load the DLL, note that we MUST load with an altered search path for this to work

	if(!gameInstance)
	{
		std::cout << "Could not Load DLL at: " << '/n' << proxyBuffer << '/n';
		DWORD errorCode = GetLastError();
		std::cout << "Error code: " << errorCode << '/n';
	}
	// Now we find our entry point in the DLL, based on how we structured our current entry point
	// This allows us the same information we would normally expect in an entry point
	#ifndef _USE_WINDOWS_ENTRY_POINT
	GameMain gMain = (GameMain)GetProcAddress(gameInstance, "GameMain");
	if(gMain)
	{
		return gMain(argc, argv);
	}
	else
	{
		std::cout << "Could not find entry point" << '/n';
		return 1;
	}
	#else
	GameMain gMain = (GameMain)GetProcAddress(gameInstance, "GameMain");
	if(gMain)
		return gMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	else
		return 1;
	#endif
} 