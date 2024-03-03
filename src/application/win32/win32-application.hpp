#ifndef TOMS_LANE_WIN32_APPLICATION_HPP
#define TOMS_LANE_WIN32_APPLICATION_HPP

namespace tl
{
	HWND window_handle_get();
	void window_handle_set(HWND windowHandle);

	HINSTANCE instance_handle_get();
}

#endif
