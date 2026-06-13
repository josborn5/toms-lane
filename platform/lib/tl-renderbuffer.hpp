#ifndef TOMS_LANE_RENDER_BUFFER
#define TOMS_LANE_RENDER_BUFFER

#include <stdint.h>

namespace tl {

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
		unsigned int max_width = 0;

		void init(uint32_t* pixels, unsigned int width, unsigned int height, frame_buffer_origin origin);
		uint32_t get_pixel(unsigned int pixel_index) const;
		unsigned int get_pixel_index(unsigned int x, unsigned int y) const;
		void plot_pixel(uint32_t color, unsigned int x, unsigned int y) const;
		void draw_horizontal_line(uint32_t color, unsigned int x0, unsigned int x1, unsigned int y) const;
		void fill(uint32_t color) const;
		void fill_rect(uint32_t color, unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) const;
		private:
			unsigned int _max_pixel_index = 0;
			unsigned int _max_height = 0;

			unsigned int resolve_y_for_bottom_left_origin(unsigned int y) const;
			unsigned int resolve_y_for_top_left_origin(unsigned int y) const;
			unsigned int (RenderBuffer::*_y_resolver)(unsigned int) const;
	};
}

#endif
