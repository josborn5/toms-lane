#ifndef TOMS_LANE_WIN32_WINDOW_HPP
#define TOMS_LANE_WIN32_WINDOW_HPP

#include "../tl-window.hpp"

namespace tl
{
	HWND window_handle_get();

	const RenderBuffer& render_buffer_get();
}

#endif
