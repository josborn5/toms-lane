#include <windows.h>
#include "./win32-window.hpp"

namespace tl
{

static const unsigned int text_buffer_size = 256;
static char text_to_render[text_buffer_size] = {0};
static RECT footprint = {0};
static bool need_to_render = false;

void win32_text_render(HDC device_context)
{
	if (!need_to_render)
	{
		return;
	}

	need_to_render = false;

	SetTextColor(device_context, 0xFFFFFF);
	SetBkMode(device_context, TRANSPARENT);

	DrawText(
		device_context,
		(LPCTSTR)text_to_render,
		-1,
		&footprint,
		DT_LEFT
	);
}

int text_interface_render(
	char* text,
	unsigned int half_width,
	unsigned int half_height,
	unsigned int center_x,
	unsigned int center_y)
{
	if (need_to_render)
	{
		return 1;
	}

	need_to_render = true;

	footprint.left = center_x - half_width;
	footprint.right = center_x + half_width;

	// windows has origin in top left. TL has origin in bottom left
	footprint.top = center_y - half_height;
	footprint.bottom = center_y + half_height;

	for (int i = 0; (i < text_buffer_size - 1) && *text; i += 1)
	{
		text_to_render[i] = *text;
		text++;
	}

	return 0;
}

}
