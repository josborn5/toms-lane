#include "./math.hpp"
#include "./geometry.hpp"
#include "./software-rendering.hpp"
#include "./utilities.hpp"

namespace tl
{
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
		// Make sure writing to the render buffer does not escape its bounds
		if (x < 0 || x >(renderBuffer.width - 1) || y < 0 || y >(renderBuffer.height - 1))
		{
			return;
		}

		int positionStartOfRow = renderBuffer.width * y;
		int positionStartOfX0InRow = positionStartOfRow + x;
		uint32_t* pixel = renderBuffer.pixels + positionStartOfX0InRow;
		*pixel = color;
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
		const int* startX = &x0;
		const int* endX = &x1;
		if (x1 < x0)
		{
			int temp = x1;
			x1 = x0;
			x0 = temp;
		}

		int positionStartOfRow = renderBuffer.width * y;
		int positionOfX0InRow = positionStartOfRow + *startX;
		uint32_t* pixelPointer = renderBuffer.pixels + positionOfX0InRow;
		for (int i = *startX; i <= *endX; i += 1)
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

	static void DrawRectInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x0, int y0, int x1, int y1)
	{
		// Make sure writing to the render buffer does not escape its bounds
		x0 = ClampInt(0, x0, renderBuffer.width);
		x1 = ClampInt(1, x1, renderBuffer.width);
		y0 = ClampInt(0, y0, renderBuffer.height);
		y1 = ClampInt(1, y1, renderBuffer.height);

		for (int y = y0; y < y1; y++)
		{
			int positionStartOfRow = renderBuffer.width * y;
			int positionStartOfX0InRow = positionStartOfRow + x0;
			uint32_t* pixel = renderBuffer.pixels + positionStartOfX0InRow;
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

		DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
	}

	void render_interface_fill_rect_rgba(const RenderBuffer& render_buffer, uint32_t color, const Rect<float> footprint)
	{

	}


	void ClearScreen(const RenderBuffer &renderBuffer, uint32_t color)
	{
		uint32_t* pixel = renderBuffer.pixels;

		for (int y = 0; y < renderBuffer.height; y += 1)
		{
			for (int x = 0; x < renderBuffer.width; x += 1)
			{
				*pixel = color;
				pixel++;
			}
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
