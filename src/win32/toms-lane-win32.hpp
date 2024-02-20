#ifndef TOMS_LANE_WIN32_H
#define TOMS_LANE_WIN32_H

/* referenced */
#include <windows.h>
#include "../tl-window.hpp"

/* offered */
#include "./win32-file.hpp"

namespace tl
{
	int OpenWindow(HINSTANCE instance, const WindowSettings &settings);
}

#endif
