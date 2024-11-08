#ifndef TOMS_LANE_FONT_FILE_HPP
#define TOMS_LANE_FONT_FILE_HPP

#include <stdint.h>

namespace tl
{

struct sprite_font
{
	uint8_t* content;
	int char_count;
};

const sprite_font& font_file_interface_get_sprite_font();

}

#endif
