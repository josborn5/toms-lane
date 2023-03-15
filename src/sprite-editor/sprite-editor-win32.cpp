#include <windows.h>
#include "../win32/toms-lane-win32.hpp"

char* filePath;

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	tl::WindowSettings settings;
	settings.title = "Sprite Editor";
	settings.width = 800;
	settings.height = 600;
	settings.targetFPS = 60;
	settings.permanentSpaceInMegabytes = 1;

	if (!*commandLine)
	{
		return 1;
	}

	filePath = commandLine;

	tl::Win32Main(instance, settings);

	return 0;
}
