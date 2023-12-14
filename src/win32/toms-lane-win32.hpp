#ifndef TOMS_LANE_WIN32_H
#define TOMS_LANE_WIN32_H

/* referenced */
#include <windows.h>
#include "../platform/toms-lane-application.hpp"

/* offered */
#include "./win32-file.hpp"

namespace tl
{
	int Win32Main(HINSTANCE instance);
	int Win32Main(HINSTANCE instance, const WindowSettings &settings);
}

#endif
