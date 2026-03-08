#ifndef TOMS_LANE_SOFTWARE_RENDERING_SPRITE_H
#define TOMS_LANE_SOFTWARE_RENDERING_SPRITE_H

#include "./math.hpp"
#include "./geometry.hpp"
#include "./memory.hpp"

namespace tl
{
	enum frame_buffer_origin {
		frame_buffer_origin_top_left,
		frame_buffer_origin_bottom_left
	};

	struct RenderBuffer
	{
		unsigned int* pixels = nullptr;
		int width = 0;
		int height = 0;
		frame_buffer_origin origin = frame_buffer_origin_bottom_left;

		void init(uint32_t* pixels, unsigned int width, unsigned int height, frame_buffer_origin origin);
		void plot_pixel(uint32_t color, unsigned int x, unsigned int y) const;
		private:
			unsigned int _max_pixel_index;
			unsigned int frame_buffer_get_row_start_pixel(unsigned int y) const;
	};

	// monochrome sprite
	struct Sprite
	{
		char* content;
		int height;
		int width;
	};

	Sprite LoadSprite(char* content);
	Vec2<int> GetContentDimensions(char* content);

	// Draw a sprite of a size determined by the given sprite string and blockHalfSize value
	void DrawSprite(
		const RenderBuffer &renderBuffer,
		const Sprite &sprite,
		const Rect<float> &footprint,
		uint32_t color
	);

}

#endif
