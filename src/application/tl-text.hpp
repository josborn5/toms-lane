#ifndef TOMS_LANE_WIN32_TEXT_HPP
#define TOMS_LANE_WIN32_TEXT_HPP

#include <stdint.h>

namespace tl
{

int text_interface_render(
	char* text,
	unsigned int half_width,
	unsigned int half_height,
	unsigned int center_x,
	unsigned int center_y);

int text_interface_render(
	char* text,
	uint32_t color,
	unsigned int half_width,
	unsigned int half_height,
	unsigned int center_x,
	unsigned int center_y);

};

#endif
