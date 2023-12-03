#ifndef TOMS_LANE_WIN32_H
#define TOMS_LANE_WIN32_H

#include <windows.h>
#include "./win32-file.hpp"
#include "../platform/toms-lane-platform.hpp"

namespace tl
{
	int Win32Main(HINSTANCE instance);
	int Win32Main(HINSTANCE instance, const WindowSettings &settings);
}

#endif
