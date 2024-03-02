#ifndef TOMS_LANE_WIN32_APPLICATION_HPP
#define TOMS_LANE_WIN32_APPLICATION_HPP

#include "../tl-application.hpp"

namespace tl
{
	void window_handle_set(HWND windowHandle);
	HINSTANCE instance_handle_get();
}

#endif
