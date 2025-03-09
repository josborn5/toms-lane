#include <windows.h>
#include "./win32-window.hpp"

namespace tl
{

struct text_to_render
{
	char* text = nullptr;
	RECT footprint = {0};
};

static unsigned int texts_count = 0;

static const unsigned int text_buffer_size = 64;
static text_to_render texts_to_render[text_buffer_size] = {0};
static HFONT default_font = NULL;
static LOGFONT font_info = {0};

static float char_height_over_width = 0.0f;

static void default_font_read(HDC device_context)
{
	// Use the current font as a base to create a new font with its height set to
	// height of the rect
	GetObject(device_context, sizeof(HFONT), &default_font);
	GetObject(default_font, sizeof(LOGFONT), &font_info);
}

static void default_font_initialize()
{
	HWND window_handle = window_handle_get();
	HDC device_context = GetDC(window_handle);
	default_font_read(device_context);

	font_info.lfItalic = FALSE;
	font_info.lfUnderline = FALSE;
	font_info.lfStrikeOut = FALSE;
	font_info.lfFaceName[0] = 'C';
	font_info.lfFaceName[1] = 'o';
	font_info.lfFaceName[2] = 'n';
	font_info.lfFaceName[3] = 's';
	font_info.lfFaceName[4] = 'o';
	font_info.lfFaceName[5] = 'l';
	font_info.lfFaceName[6] = 'a';
	font_info.lfFaceName[7] = 's';
	font_info.lfFaceName[8] = '\0';

	HFONT font_to_set = CreateFontIndirectA(&font_info);

	SelectObject(device_context, font_to_set);

	// Read the font again after setting it to calculate the aspect ratio of each character
	default_font_read(device_context);
	ReleaseDC(window_handle, device_context);

	char_height_over_width = (float)font_info.lfHeight / (float)font_info.lfWidth;
}

bool win32_text_will_render()
{
	return (texts_count > 0);
}

void win32_text_render(HDC device_context)
{
	if (texts_count == 0)
	{
		return;
	}

	SetTextColor(device_context, 0xFFFFFF);
	SetBkMode(device_context, TRANSPARENT);


	for (unsigned int i = 0; i < texts_count; i += 1)
	{
		text_to_render& render_text = texts_to_render[i];

		font_info.lfHeight = render_text.footprint.bottom - render_text.footprint.top;
		HFONT font_to_set = CreateFontIndirectA(&font_info);

		SelectObject(device_context, font_to_set);

		DrawText(
			device_context,
			(LPCTSTR)(render_text.text),
			-1,
			&render_text.footprint,
			DT_LEFT
		);
	}

	texts_count = 0;
}

int text_interface_render(
	char* text,
	unsigned int half_width,
	unsigned int half_height,
	unsigned int center_x,
	unsigned int center_y)
{
	if (texts_count >= text_buffer_size)
	{
		return -1;
	}

	if (default_font == NULL)
	{
		default_font_initialize();
	}

	text_to_render* render_text = &texts_to_render[texts_count];

	render_text->text = text;
	render_text->footprint.left = center_x - half_width;
	render_text->footprint.right = center_x + half_width;

	// windows has origin in top left. TL has origin in bottom left
	const RenderBuffer& render_buffer = render_buffer_get();
	render_text->footprint.top = render_buffer.height - center_y - half_height;
	render_text->footprint.bottom = render_buffer.height - center_y + half_height;

	texts_count += 1;

	int char_width = (int)((float)(half_height + half_height) / char_height_over_width);

	return char_width;
}

}
