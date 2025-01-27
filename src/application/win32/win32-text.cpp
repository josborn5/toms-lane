#include <windows.h>
#include "./win32-window.hpp"

namespace tl
{

int text_interface_render(
	char* text,
	unsigned int half_width,
	unsigned int half_height,
	unsigned int center_x,
	unsigned int center_y)
{
	HWND window_handle = window_handle_get();

	if (nullptr == window_handle)
	{
		return -1;
	}

	HDC device_context = GetDC(window_handle);

	RECT footprint = {0};
	footprint.left = center_x - half_width;
	footprint.right = center_x + half_width;

	// windows has origin in top left. TL has origin in bottom left
	footprint.top = center_y - half_height;
	footprint.bottom = center_y + half_height;

	SetTextColor(device_context, 0xFFFFFF);
	SetBkMode(device_context, TRANSPARENT);

	int draw_result = DrawText(
		device_context,
		(LPCTSTR)text,
		-1,
		&footprint,
		DT_LEFT
	);

	DeleteDC(device_context);

	return draw_result;
}

}
