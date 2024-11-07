#ifndef TOMS_LANE_FONT_HPP
#define TOMS_LANE_FONT_HPP

#include "../tl-application.hpp"
#include "./geometry.hpp"

namespace tl
{

struct sprite_font
{
	char* content;
	int char_count;
};

int font_interface_initialize_from_sprite(const sprite_font& sprite);

int font_interface_initialize_from_file(
	char* file_name,
	MemorySpace& target,
	MemorySpace& remainder
);

float font_interface_render_chars(
	const RenderBuffer& buffer,
	char* text,
	const Rect<float>& firstCharFootprint,
	uint32_t color
);

void font_interface_render_int(
	const RenderBuffer& buffer,
	int number,
	const Rect<float>& firstCharFootprint,
	uint32_t color
);

}

#endif
