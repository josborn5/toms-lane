#ifndef TOMS_LANE_FONT_HPP
#define TOMS_LANE_FONT_HPP

#include "./software-rendering.hpp"
#include "./geometry.hpp"

namespace tl
{

int font_interface_initialize();

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
