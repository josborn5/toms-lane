#include <windows.h>
#include "../win32/toms-lane-win32.hpp"

char* filePath;
const int windowWidth = 800;
const int windowHeight = 600;

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	tl::WindowSettings settings;
	settings.title = "Sprite Editor";
	settings.width = windowWidth;
	settings.height = windowHeight;
	settings.targetFPS = 60;
	settings.permanentSpaceInMegabytes = 1;
	settings.transientSpaceInMegabytes = 1;

	if (!*commandLine)
	{
		return 1;
	}

	filePath = commandLine;

	tl::Win32Main(instance, settings);

	return 0;
}
