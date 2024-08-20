#ifndef TOMS_LANE_FONT_HPP
#define TOMS_LANE_FONT_HPP

#include "../tl-application.hpp"
#include "./geometry.hpp"

namespace tl
{

void font_interface_initialize();

void font_interface_render_alphabet(
	const RenderBuffer& buffer,
	char* text,
	const Rect<float>& firstCharFootprint,
	uint32_t color
);

void font_interface_render_integer(
	const RenderBuffer& buffer,
	int number,
	const Rect<float>& firstCharFootprint,
	uint32_t color
);

}

#endif
