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

		// Use the current font as a base to create a new font with its height set to
		// height of the rect
		HFONT default_font;
		GetObject(device_context, sizeof(HFONT), &default_font);

		LOGFONT font_info;
		GetObject(default_font, sizeof(LOGFONT), &font_info);
		font_info.lfHeight = render_text.footprint.bottom - render_text.footprint.top;
		font_info.lfWidth = 0;
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
		return 1;
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

	return 0;
}

}
