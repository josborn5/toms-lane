#include "./math.hpp"
#include "./geometry.hpp"
#include "./software-rendering.hpp"
#include "./utilities.hpp"


namespace tl
{
	unsigned int RenderBuffer::resolve_y_for_bottom_left_origin(unsigned int y) const {
		return y;
	}

	unsigned int RenderBuffer::resolve_y_for_top_left_origin(unsigned int y) const {
		return height - y;
	}

	void RenderBuffer::init(uint32_t* pixels, unsigned int width, unsigned int height, frame_buffer_origin origin) {
		if (width == 0) {
			width = 1;
		}
		if (height == 0) {
			height = 1;
		}
		this->_max_pixel_index = (height * width) - 1;

		this->_y_resolver = (origin == frame_buffer_origin_top_left)
			? &RenderBuffer::resolve_y_for_top_left_origin
			: &RenderBuffer::resolve_y_for_bottom_left_origin;

		this->pixels = pixels;
		this->width = width;
		this->height = height;
		this->origin = origin;

		this->max_width = width - 1;
		this->_max_height = height - 0; // this is wonky af
	}

	uint32_t RenderBuffer::get_pixel(unsigned int pixel_index) const {
		if (pixel_index > _max_pixel_index) {
			return 0;
		}

		return pixels[pixel_index];
	}

	unsigned int RenderBuffer::get_pixel_index(unsigned int x, unsigned int y) const {
		unsigned int frame_buffer_y = (this->*_y_resolver)(y);
		unsigned int start_row_pixel_index = width * frame_buffer_y;
		return start_row_pixel_index + x;
	}

	void RenderBuffer::plot_pixel(uint32_t color, unsigned int x, unsigned int y) const {
		int pixel_index = get_pixel_index(x, y);
		if (pixel_index > _max_pixel_index) {
			return;
		}

		uint32_t* pixel = pixels + pixel_index;
		*pixel = color;
	}

	/**
	 *	|---|---|---|
	 *	| 0 | 1 | 2 |	pixel ordinals
	 *	|---|---|---|
	 *	0   1   2   3	position ordinals
	 *
	 * x & y parameters are the pixel and NOT the position ordinals
	 */
	void PlotPixel(const RenderBuffer &renderBuffer, uint32_t color, int x, int y)
	{
		renderBuffer.plot_pixel(color, x, y);
	}

	/**
	 *	|---|---|---|
	 *	| 0 | 1 | 2 |	pixel ordinals
	 *	|---|---|---|
	 *	0   1   2   3	position ordinals
	 *
	 * x1, x2 & y parameters are the pixel and NOT the position ordinals
	 */
	static void DrawHorizontalLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x0, int x1, int y)
	{
		renderBuffer.draw_horizontal_line(color, x0, x1, y);
	}

	void RenderBuffer::draw_horizontal_line(uint32_t color, unsigned int x0, unsigned int x1, unsigned int y) const {
		const unsigned int* startX = &x0;
		const unsigned int* endX = &x1;
		if (x1 < x0)
		{
			int temp = x1;
			x1 = x0;
			x0 = temp;
		}
		unsigned int frame_buffer_y = (this->*_y_resolver)(y);
		unsigned int start_row_pixel_index = width * frame_buffer_y;
		unsigned int start_index = start_row_pixel_index + *startX;
		unsigned int end_index = start_row_pixel_index + *endX;
		uint32_t* pixelPointer = pixels + start_index;

		if (end_index > _max_pixel_index) {
			end_index = _max_pixel_index;
		}

		for (int i = start_index; i <= end_index; i += 1)
		{
			*pixelPointer = color;
			pixelPointer++;
		}
	}

	/**
	 *	|---|---|---|
	 *	| 0 | 1 | 2 |	pixel ordinals
	 *	|---|---|---|
	 *	0   1   2   3	position ordinals
	 *
	 * x, y0 & y1 parameters are the pixel and NOT the position ordinals
	 */
	void DrawVerticalLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x, int y0, int y1)
	{
		int yDiff = y1 - y0;
		int yDiffMod = (yDiff < 0) ? -1 * yDiff : yDiff;
		int yIncrement = (yDiff < 0) ? -1 : 1;
		for (int i = 0; i <= yDiffMod; i += 1)
		{
			PlotPixel(renderBuffer, color, x, y0);
			y0 += yIncrement;
		}
	}

	/**
	 *	|---|---|---|
	 *	| 0 | 1 | 2 |	pixel ordinals
	 *	|---|---|---|
	 *	0   1   2   3	position ordinals
	 *
	 * p0 & p1 are pixel and NOT position ordinals
	 */
	// Implemented with Bresenham's algorithm
	void DrawLineInPixels(
		const RenderBuffer &renderBuffer,
		uint32_t color,
		const Vec2<int> &p0,
		const Vec2<int> &p1
	) {
		int x0 = p0.x;
		int y0 = p0.y;
		int x1 = p1.x;
		int y1 = p1.y;
		
		int xDiff = x1 - x0;
		if (xDiff == 0)
		{
			DrawVerticalLineInPixels(renderBuffer, color, x0, y0, y1);
			return;
		}

		int yDiff = y1 - y0;
		if (yDiff == 0)
		{
			DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y0);
			return;
		}
		bool negativeXDiff = (xDiff < 0);
		bool negativeYDiff = (yDiff < 0);
		int xDiffMod = (negativeXDiff) ? -1 * xDiff : xDiff;
		int yDiffMod = (negativeYDiff) ? -1 * yDiff : yDiff;
		int xIncrement = (negativeXDiff) ? -1 : 1;
		int yIncrement = (negativeYDiff) ? -1 : 1;

		// If the gradient is 1 simply increment both X & Y at on every iteration
		if (xDiffMod == yDiffMod)
		{
			for (int i = 0; i <= xDiffMod; ++i)
			{
				PlotPixel(renderBuffer, color, x0, y0);
				x0 += xIncrement;
				y0 += yIncrement;
			}
			return;
		}

		// If the gradient is more than one then y gets incremented on every step along the line and x sometimes gets incremented
		// If the gradient is less than one then x gets incremented on every step along the line and y sometimes gets incremented
		int longDimensionDiff;
		int* longDimensionVar;	// Make this a pointer so PlotPixel can still be called with x0 & y0 arguments
		int longDimensionIncrement;
		int shortDimensionDiff;
		int* shortDimensionVar;	// Make this a pointer so PlotPixel can still be called with x0 & y0 arguments
		int shortDimensionIncrement;

		if (yDiffMod < xDiffMod)
		{
			longDimensionDiff = xDiffMod;
			longDimensionVar = &x0;
			longDimensionIncrement = xIncrement;

			shortDimensionDiff = yDiffMod;
			shortDimensionVar = &y0;
			shortDimensionIncrement = yIncrement;
		}
		else
		{
			longDimensionDiff = yDiffMod;
			longDimensionVar = &y0;
			longDimensionIncrement = yIncrement;

			shortDimensionDiff = xDiffMod;
			shortDimensionVar = &x0;
			shortDimensionIncrement = xIncrement;
		}

		int negativePIncrement = 2 * shortDimensionDiff;
		int p = negativePIncrement - longDimensionDiff;
		int positivePIncrement = negativePIncrement - (2 * longDimensionDiff);

		for (int i = 0; i <= longDimensionDiff; i += 1)
		{
			PlotPixel(renderBuffer, color, x0, y0);
			*longDimensionVar += longDimensionIncrement;
			if (p < 0)
			{
				p += negativePIncrement;
			}
			else
			{
				p += positivePIncrement;
				*shortDimensionVar += shortDimensionIncrement;
			}
		}
	}
	void DrawLineInPixels(
		const RenderBuffer &renderBuffer,
		uint32_t color,
		const Vec2<float> &p0,
		const Vec2<float> &p1
	) {
		Vec2<int> intP0 = {
			(int)p0.x,
			(int)p0.y
		};
		Vec2<int> intP1 = {
			(int)p1.x,
			(int)p1.y
		};
		DrawLineInPixels(renderBuffer, color, intP0, intP1);
	}

	static int ClampInt(int min, int val, int max)
	{
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}

	static int ConvertFloatToInt(float floatValue)
	{
		return (int)(floatValue + 0.5f);
	}

	void RenderBuffer::fill_rect(uint32_t color, unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) const {
		if (x0 > max_width) x0 = max_width;
		if (x1 > max_width) x1 = max_width;
		if (y0 > _max_height) y0 = _max_height;
		if (y1 > _max_height) y1 = _max_height;

		for (int y = y0; y < y1; y++)
		{
			int x0_pixel_index = get_pixel_index(x0, y);
			uint32_t* pixel = pixels + x0_pixel_index;
			for (int x = x0; x < x1; x++)
			{
				*pixel = color;
				pixel++;
			}
		}
	}

	static void DrawCircleInPixels(const RenderBuffer& renderBuffer, uint32_t color, int centerX, int centerY, int radius)
	{
		int r2 = radius + radius;
		int circleX = radius;
		int circleY = 0;
		int dY = -2;
		int dX = r2 + r2 - 4;
		int decision = r2 - 1;

		while (circleY <= circleX)
		{
			PlotPixel(renderBuffer, color, centerX - circleX, centerY - circleY);
			PlotPixel(renderBuffer, color, centerX - circleX, centerY + circleY);
			PlotPixel(renderBuffer, color, centerX + circleX, centerY - circleY);
			PlotPixel(renderBuffer, color, centerX + circleX, centerY + circleY);
			PlotPixel(renderBuffer, color, centerX - circleY, centerY - circleX);
			PlotPixel(renderBuffer, color, centerX - circleY, centerY + circleX);
			PlotPixel(renderBuffer, color, centerX + circleY, centerY - circleX);
			PlotPixel(renderBuffer, color, centerX + circleY, centerY + circleX);

			decision += dY;
			dY -= 4;

			circleY += 1; // always increment up
			if (decision < 0) // decide whether or not to decrement left toward the center of the circle
			{
				decision += dX;
				dX -= 4;
				circleX -= 1;
			}
		}
	}

	void DrawCircle(const RenderBuffer &renderBuffer, uint32_t color, const Vec2<float>& center, float radius)
	{
		int centerX = ConvertFloatToInt(center.x);
		int centerY = ConvertFloatToInt(center.y);
		int radiusInt = ConvertFloatToInt(radius);

		DrawCircleInPixels(renderBuffer, color, centerX, centerY, radiusInt);
	}

	void DrawRect(const RenderBuffer &renderBuffer, uint32_t color, const Rect<float> &rect)
	{
		int x0 = ConvertFloatToInt(rect.x_min());
		int x1 = ConvertFloatToInt(rect.x_max());
		int y0 = ConvertFloatToInt(rect.y_min());
		int y1 = ConvertFloatToInt(rect.y_max());


		renderBuffer.fill_rect(color, x0, y0, x1, y1);
	}

	void render_interface_fill_rect_rgba(const RenderBuffer& render_buffer, uint32_t color, const Rect<float> footprint)
	{

	}


	void ClearScreen(const RenderBuffer &renderBuffer, uint32_t color)
	{
		renderBuffer.fill(color);
	}

	void RenderBuffer::fill(uint32_t color) const {
		uint32_t* pixel = pixels;
		for (unsigned int i = 0; i < _max_pixel_index; i += 1) {
			*pixel = color;
			pixel++;
		}
	}

	uint32_t GetColorFromRGB(int red, int green, int blue)
	{
		// Use bitwise operators to construct a single uint32_t value from three 0-255 RGB values.
		// There are 32 bits to fill up.
		// Each 0-255 value is a single byte, or 8 bits. So the 32 bits will be split into 4 segments (32 bits / 8 bits = 4).
		// |--1--|--2--|--3--|--4--|	bytes
		// 1     8     16    24    32	bits
		//
		// So the 0xRRGGBB value is made by constructing:
		// 1. the RR value and shifting it 16 places to the left
		//		|00|00|00|RR|	--> |00|RR|00|00|
		// 2. the GG value and shifting it 8 places to the left
		//		|00|00|00|GG|	-->	|00|00|GG|00|
		// 3. the BB value
		//							|00|00|00|BB|
		// Adding these together gives us the 0xRRGGBB value:
		//		|0x|00|00|00| + |00|RR|00|00| + |00|00|GG|00| + |00|00|00|BB| = |0x|RR|GG|BB|
		uint32_t color = (uint32_t)(0x000000 + (red << 16) + (green << 8) + blue);
		return color;
	}



}
