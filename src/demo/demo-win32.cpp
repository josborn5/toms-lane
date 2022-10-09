#include <windows.h>
#include "../win32/toms-lane-win32.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	tl::WindowSettings settings;
	settings.title = "Demo";
	settings.width = 1280;
	settings.height = 720;
	settings.targetFPS = 30;

	tl::Win32Main(instance, settings);
}
