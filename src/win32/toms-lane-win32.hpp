#ifndef TOMS_LANE_WIN32_H
#define TOMS_LANE_WIN32_H

#include <windows.h>

namespace tl
{
	struct WindowSettings
	{
		int width;
		int height;
		char* title;
		int targetFPS;
	};

	int Win32Main(HINSTANCE instance);

	int Win32Main(HINSTANCE instance, const WindowSettings &settings);
}

#endif
