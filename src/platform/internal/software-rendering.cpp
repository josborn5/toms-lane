#include "./platform.hpp"
#include "./math.hpp"
#include "./geometry.hpp"
#include "./software-rendering.hpp"
#include <list>
#include <vector>

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
			std::swap(x0, x1);
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
	 * x1, x2 & y parameters are the pixel and NOT the position ordinals
	 */
	static void DrawHorizontalLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x0, int x1, int y, float z)
	{
		const int* startX = &x0;
		const int* endX = &x1;
		if (x1 < x0)
		{
			std::swap(x0, x1);
		}

		int positionStartOfRow = renderBuffer.width * y;
		int positionOfX0InRow = positionStartOfRow + *startX;
		uint32_t* pixelPointer = renderBuffer.pixels + positionOfX0InRow;
		float* depthPointer = renderBuffer.depth + positionOfX0InRow;
		for (int i = *startX; i <= *endX; i += 1)
		{
			if (*depthPointer < z)
			{
				*depthPointer = z;
				*pixelPointer = color;
			}
			pixelPointer++;
			depthPointer++;
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
	void DrawLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, const Vec2<int> &p0, const Vec2<int> &p1)
	{
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
		x0 = ClampInt(1, x0, renderBuffer.width);
		x1 = ClampInt(1, x1, renderBuffer.width);
		y0 = ClampInt(1, y0, renderBuffer.height);
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

	void DrawRect(const RenderBuffer &renderBuffer, uint32_t color, const Rect<float> &rect)
	{
		int x0 = ConvertFloatToInt(rect.position.x - rect.halfSize.x);
		int x1 = ConvertFloatToInt(rect.position.x + rect.halfSize.x);
		int y0 = ConvertFloatToInt(rect.position.y - rect.halfSize.y);
		int y1 = ConvertFloatToInt(rect.position.y + rect.halfSize.y);

		DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
	}

	void DrawSprite(
		const RenderBuffer &renderBuffer,
		const Sprite &sprite,
		const Rect<float> &footprint,
		uint32_t color
	)
	{
		// Work out the size of each block in the sprite
		float footprintHeight = footprint.halfSize.y * 2.0f;
		float footprintWidth = footprint.halfSize.x * 2.0f;

		float blockWidth = footprintWidth / (float)sprite.width;
		float blockHeight = footprintHeight / (float)sprite.height;
		Vec2<float> blockHalf = { 0.5f * blockWidth, 0.5f * blockHeight };

		// Calculate the cursor area over which to position each block. Position is measured fro the center
		// so apply an offset for the block size
		Vec2<float> pCopy = Vec2<float>
		{
			footprint.position.x - footprint.halfSize.x + blockHalf.x,
			footprint.position.y + footprint.halfSize.y - blockHalf.y
		};

		// iterate through the sprite content and fill blocks in the render buffer
		float xMinCursorPos = pCopy.x;
		char* content = sprite.content;
		while (*content)
		{
			if (*content == '\n')
			{
				pCopy.y -= blockHeight;	// We're populating blocks in the sprint left to right, top to bottom. So y is decreasing.
				pCopy.x = xMinCursorPos; // reset cursor to start of next row
			}
			else
			{
				if (*content != ' ')
				{
					Rect<float> blockRect;
					blockRect.position = pCopy;
					blockRect.halfSize = blockHalf;
					DrawRect(renderBuffer, color, blockRect);
				}
				pCopy.x += blockWidth;
			}
			content++;
		}
	}

	/*	p0------p1
	 *	\       /	|
	 *	 \     /	|
	 *	  \   /		V
	 *	   \ /	  +ve y (if +ve y is up, this is actually a flat bottom triangle)
	 *	    p2
	 */
	void FillFlatTopTriangle(const RenderBuffer &renderBuffer, uint32_t color, const Vec3<int> &p0, const Vec3<int> &p1, const Vec3<int> &p2, float z)
	{
		// LINE 0-->2
		bool p2IsRightOfP0 = (p0.x < p2.x);
		int xDiff0 = (p2IsRightOfP0) ? p2.x - p0.x : p0.x - p2.x;
		int yDiff0 = p2.y - p0.y;

		bool isLongDimension0X = (yDiff0 < xDiff0);
		int longDelta0 = (isLongDimension0X) ? xDiff0 : yDiff0;
		int shortDelta0 = (isLongDimension0X) ? yDiff0 : xDiff0;

		int negIncrement0 = 2 * shortDelta0;
		int acc0 = negIncrement0 - longDelta0;
		int posIncrement0 = negIncrement0 - (2 * longDelta0);
		int x0Increment = (p2IsRightOfP0) ? 1 : -1;

		// LINE 1-->2
		// Vertical distance for 1-->2 is the same as 0-->2, so no need for a separate yDiff1 variable. Can reuse yDiff0.
		bool p2IsLeftOfP1 = (p2.x < p1.x);
		int xDiff1 = (p2IsLeftOfP1) ? p1.x - p2.x : p2.x - p1.x;

		bool isLongDimension1X = (yDiff0 < xDiff1);
		int longDelta1 = (isLongDimension1X) ? xDiff1 : yDiff0;
		int shortDelta1 = (isLongDimension1X) ? yDiff0 : xDiff1;

		int negIncrement1 = 2 * shortDelta1;
		int acc1 = negIncrement1 - longDelta1;
		int posIncrement1 = negIncrement1 - (2 * longDelta1);
		int x1Increment = (p2IsLeftOfP1) ? -1 : 1;

		// Copy the x & y values for p0 & p1 so we can modify them safely inside this function
		// Note that p0.y == p1.y so we only need one variable for the y position
		int x0 = p0.x;
		int x1 = p1.x;
		for (int y = p0.y; y <= p2.y; y += 1)
		{
			// draw scanline to fill in triangle between x0 & x1
			DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y, z);

			// Loop through the x0 / acc0 evaluation until acc0 is +ve.
			// acc0 turning +ve is the indication we should plot.
			if (isLongDimension0X)
			{
				while ((acc0 < 0) && (negIncrement0 > 0)) // x0 needs to increment AT LEAST once since it's the long dimension. i.e. it needs to increment more than y
				{
					acc0 += negIncrement0;
					x0 += x0Increment;
				}
			}

			if (isLongDimension1X)
			{
				while ((acc1 < 0) && (negIncrement1 > 0))
				{
					acc1 += negIncrement1;
					x1 += x1Increment;
				}
			}

			// line p0 --> p2: decide to increment x0 or not for next y
			if (isLongDimension0X) // X0 needs to increment AT LEAST once since it's the long dimension. i.e. it needs to increment more than y
			{
				acc0 += posIncrement0;
				x0 += x0Increment;
			}
			else
			{
				if (acc0 < 0)
				{
					acc0 += negIncrement0;
				}
				else
				{
					acc0 += posIncrement0;
					x0 += x0Increment;
				}
			}

			// line p1 --> p2: decide to decrement x1 or not for next y
			if (isLongDimension1X)
			{
				acc1 += posIncrement1;
				x1 += x1Increment;
			}
			else
			{
				if (acc1 < 0)
				{
					acc1 += negIncrement1;
				}
				else
				{
					acc1 += posIncrement1;
					x1 += x1Increment;
				}
			}
		}
	}

	/*`     p0
	 *`     /\			|
	 *`    /  \			|
	 *`   /    \		V
	 *	 /      \	  +ve y (if +ve y is up, this is actually a flat top triangle)
	 *	p1------p2
	 */
	void FillFlatBottomTriangle(const RenderBuffer &renderBuffer, uint32_t color, const Vec3<int> &p0, const Vec3<int> &p1, const Vec3<int> &p2, float z)
	{
		// LINE 0-->1
		bool p1IsLeftOfP0 = (p1.x < p0.x);
		int xDiff0 = (p1IsLeftOfP0) ? p0.x - p1.x : p1.x - p0.x;
		int yDiff0 = p1.y - p0.y;

		bool isLongDimension0X = (yDiff0 < xDiff0);
		int longDelta0 = (isLongDimension0X) ? xDiff0 : yDiff0;
		int shortDelta0 = (isLongDimension0X) ? yDiff0 : xDiff0;

		int negIncrement0 = 2 * shortDelta0;
		int acc0 = negIncrement0 - longDelta0;
		int posIncrement0 = negIncrement0 - (2 * longDelta0);
		int x0Increment = (p1IsLeftOfP0) ? -1 : 1;

		// LINE 0-->2
		// Vertical distance for 1-->2 is the same as 0-->2, so no need for a separate yDiff1 variable. Can reuse yDiff0.
		bool p2IsRightOfP0 = (p0.x < p2.x);
		int xDiff1 = (p2IsRightOfP0) ? p2.x - p0.x : p0.x - p2.x;

		bool isLongDimension1X = (yDiff0 < xDiff1);
		int longDelta1 = (isLongDimension1X) ? xDiff1 : yDiff0;
		int shortDelta1 = (isLongDimension1X) ? yDiff0 : xDiff1;

		int negIncrement1 = 2 * shortDelta1;
		int acc1 = negIncrement1 - longDelta1;
		int posIncrement1 = negIncrement1 - (2 * longDelta1);
		int x1Increment = (p2IsRightOfP0) ? 1 : -1;

		// Copy the x & y values for p0 & p1 so we can modify them safely inside this function
		// Note that p0.y == p1.y so we only need one variable for the y position
		int x0 = p0.x;
		int x1 = p0.x;

		for (int y = p0.y; y <= p1.y - 1; y += 1)
		{
			// Loop through the x0 / acc0 evaluation until acc0 is +ve.
			// acc0 turning +ve is the indication we should plot.
			if (isLongDimension0X)
			{
				while ((acc0 < 0) && (negIncrement0 > 0)) // x0 needs to increment AT LEAST once since it's the long dimension. i.e. it needs to increment more than y
				{
					acc0 += negIncrement0;
					x0 += x0Increment;
				}
			}

			if (isLongDimension1X)
			{
				while ((acc1 < 0) && (negIncrement1 > 0))
				{
					acc1 += negIncrement1;
					x1 += x1Increment;
				}
			}

			// draw scanline to fill in triangle between x0 & x1
			DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y, z);

			// line p0 --> p1: decide to increment x0 or not for current y
			if (isLongDimension0X)
			{
				acc0 += posIncrement0;
				x0 += x0Increment;
			}
			else
			{
				if (acc0 < 0)
				{
					acc0 += negIncrement0;
				}
				else
				{
					acc0 += posIncrement0;
					x0 += x0Increment;
				}
			}

			// line p0 --> p2: decide to decrement x1 or not for next y
			if (isLongDimension1X)
			{
				acc1 += posIncrement1;
				x1 += x1Increment;
			}
			else
			{
				if (acc1 < 0)
				{
					acc1 += negIncrement1;
				}
				else
				{
					acc1 += posIncrement1;
					x1 += x1Increment;
				}
			}
		}

		// draw final scanline to fill in triangle between x0 & x1
		DrawHorizontalLineInPixels(renderBuffer, color, p1.x, p2.x, p1.y, z);
	}

	void FillTriangleInPixels(const RenderBuffer &renderBuffer, uint32_t color, const Vec3<int> &p0, const Vec3<int> &p1, const Vec3<int> &p2, float z)
	{
		const Vec3<int>* pp0 = &p0;
		const Vec3<int>* pp1 = &p1;
		const Vec3<int>* pp2 = &p2;

		/* Sort the three points of the triangle by their y co-ordinate
		 *
		 * pp0->y	low		|
		 * pp1->y			|
		 * pp2->y	high	V
		 *				  +ve y
		 */
		if (pp1->y < pp0->y)
		{
			std::swap(pp0, pp1);
		}
		if (pp2->y < pp1->y)
		{
			std::swap(pp1, pp2);
		}
		if (pp1->y < pp0->y)
		{
			std::swap(pp0, pp1);
		}

		// Check for natural flat top
		if (pp0 -> y == pp1->y)
		{
			// sort top two points of flat top by their x co-ordinate
			if (pp1->x < pp0->x)
			{
				std::swap(pp0, pp1);
			}
			FillFlatTopTriangle(renderBuffer, color, *pp0, *pp1, *pp2, z);
		}
		else if (pp1->y == pp2->y) // natural flat bottom
		{
			// sort bottom two points of flat bottom by their x co-ordinate
			if (pp2->x < pp1->x)
			{
				std::swap(pp1, pp2);
			}
			FillFlatBottomTriangle(renderBuffer, color, *pp0, *pp1, *pp2, z);
		}
		else // general triangle
		{
			// Start scanning vertically from the top point (lowest y value) to the center with the flat bottom logic.
			// Then when we reach the center point, continue scanning but switch to the flat top triangle logic until we reach the bottom point (highest y value).
			// That should mean there is no need worry about finding the split point.

			// At this point we know that p0 has lowest y value. But we need to work out if p1 is left or right of p2 in order to start scanning.
			bool pp1xIsLessThanPp2X = (pp1->x < pp2->x);
			const Vec3<int>* leftPoint = (pp1xIsLessThanPp2X) ? pp1 : pp2;
			const Vec3<int>* rightPoint = (pp1xIsLessThanPp2X) ? pp2 : pp1;

			// LEFT LINE
			bool leftPointIsLeftOfP0 = (leftPoint->x < pp0->x);
			int xDiff0 = (leftPointIsLeftOfP0) ? pp0->x - leftPoint->x : leftPoint->x - pp0->x;
			int yDiff0 = leftPoint->y - pp0->y;

			bool isLongDimension0X = (yDiff0 < xDiff0);
			int longDelta0 = (isLongDimension0X) ? xDiff0 : yDiff0;
			int shortDelta0 = (isLongDimension0X) ? yDiff0 : xDiff0;

			int negIncrement0 = 2 * shortDelta0;
			int acc0 = negIncrement0 - longDelta0;
			int posIncrement0 = negIncrement0 - (2 * longDelta0);
			int x0Increment = (leftPointIsLeftOfP0) ? -1 : 1;

			// RIGHT LINE
			bool rightPointIsRightOfP0 = (pp0->x < rightPoint->x);
			int xDiff1 = (rightPointIsRightOfP0) ? rightPoint->x - pp0->x : pp0->x - rightPoint->x;
			int yDiff1 = rightPoint->y - pp0->y;

			bool isLongDimension1X = (yDiff1 < xDiff1);
			int longDelta1 = (isLongDimension1X) ? xDiff1 : yDiff1;
			int shortDelta1 = (isLongDimension1X) ? yDiff1 : xDiff1;

			int negIncrement1 = 2 * shortDelta1;
			int acc1 = negIncrement1 - longDelta1;
			int posIncrement1 = negIncrement1 - (2 * longDelta1);
			int x1Increment = (rightPointIsRightOfP0) ? 1 : -1;

			// Copy the x & y values for leftPoint & rightPoint so we can modify them safely inside this function
			int x0 = pp0->x;
			int x1 = pp0->x;

			for (int y = pp0->y; y <= pp1->y - 1; y += 1)	// Note that p1.y has already been sorte to be the vertical mid point of the triangle
			{
				// Loop through the x0 / acc0 evaluation until acc0 is +ve.
				// acc0 turning +ve is the indication we should plot.
				if (isLongDimension0X)
				{
					while ((acc0 < 0) && (negIncrement0 > 0)) // x0 needs to increment AT LEAST once since it's the long dimension. i.e. it needs to increment more than y
					{
						acc0 += negIncrement0;
						x0 += x0Increment;
					}
				}

				if (isLongDimension1X)
				{
					while ((acc1 < 0) && (negIncrement1 > 0))
					{
						acc1 += negIncrement1;
						x1 += x1Increment;
					}
				}

				// draw scanline to fill in triangle between x0 & x1
				DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y, z);

				// line p0 --> p1: decide to increment x0 or not for current y
				if (isLongDimension0X)
				{
					acc0 += posIncrement0;
					x0 += x0Increment;
				}
				else
				{
					if (acc0 < 0)
					{
						acc0 += negIncrement0;
					}
					else
					{
						acc0 += posIncrement0;
						x0 += x0Increment;
					}
				}

				// line p0 --> p2: decide to decrement x1 or not for next y
				if (isLongDimension1X)
				{
					acc1 += posIncrement1;
					x1 += x1Increment;
				}
				else
				{
					if (acc1 < 0)
					{
						acc1 += negIncrement1;
					}
					else
					{
						acc1 += posIncrement1;
						x1 += x1Increment;
					}
				}
			}

			// Now y is at pp1->y, so draw the scanline. Need to work out if pp1->y is left or right.
			if (pp1xIsLessThanPp2X) // pp1->y is the leftPoint. i.e. Right major triangle
			{
				Vec3<int> intermediatePoint = { x1, pp1->y, 0 };
				FillFlatTopTriangle(renderBuffer, color, *pp1, intermediatePoint, *pp2, z);
			}
			else	// pp1->y is the rightPoint. i.e. Left major triangle
			{
				Vec3<int> intermediatePoint = { x0, pp1->y, 0 };
				FillFlatTopTriangle(renderBuffer, color, intermediatePoint, *pp1, *pp2, z);
			}
		}
	}


	void DrawTriangleInPixels(const RenderBuffer &renderBuffer, uint32_t color, const Vec2<int> &p0, const Vec2<int> &p1, const Vec2<int> &p2)
	{
		DrawLineInPixels(renderBuffer, color, p0, p1);
		DrawLineInPixels(renderBuffer, color, p1, p2);
		DrawLineInPixels(renderBuffer, color, p2, p0);
	}

	void ClearScreen(const RenderBuffer &renderBuffer, uint32_t color)
	{
		uint32_t* pixel = renderBuffer.pixels;
		float* depth = renderBuffer.depth;

		for (int y = 0; y < renderBuffer.height; y += 1)
		{
			for (int x = 0; x < renderBuffer.width; x += 1)
			{
				*pixel = color;
				pixel++;

				*depth = 0.0f;;
				depth++;
			}
		}
	}

	unsigned int GetColorFromRGB(int red, int green, int blue)
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
		return (unsigned int)color;
	}

	template<typename T>
	void TransformAndRenderMesh(const RenderBuffer &renderBuffer, const Mesh<T> &mesh, const Camera<T> &camera, const Matrix4x4<T> transformMatrix, const Matrix4x4<T> projectionMatrix)
	{
		const int RED = 0;
		const int GREEN = 255;
		const int BLUE = 0;

		// Camera matrix
		Vec4<T> target = AddVectors(camera.position, camera.direction);
		Matrix4x4<T> cameraMatrix = PointAt(camera.position, target, camera.up);

		// View matrix
		Matrix4x4<T> viewMatrix = LookAt(cameraMatrix);

		std::vector<Triangle4d<T>> trianglesToDraw;

		Plane<T> bottomOfScreen = { (T)0, (T)0, (T)0,						(T)0, (T)1, (T)0 };
		Plane<T> topOfScreen = { (T)0, (T)(renderBuffer.height - 1), (T)0,	(T)0, (T)-1, (T)0 };
		Plane<T> leftOfScreen = { (T)0, (T)0, (T)0,							(T)1, (T)0, (T)0 };
		Plane<T> rightOfScreen = { (T)(renderBuffer.width - 1), (T)0, (T)0,	(T)-1, (T)0, (T)0 };

		for (Triangle4d<T> tri : mesh.triangles)
		{
			Triangle4d<T> transformed;
			Triangle4d<T> viewed;
			Triangle4d<T> projected;

			// Transform the triangle in the mesh
			MultiplyVectorWithMatrix(tri.p[0], transformed.p[0], transformMatrix);
			MultiplyVectorWithMatrix(tri.p[1], transformed.p[1], transformMatrix);
			MultiplyVectorWithMatrix(tri.p[2], transformed.p[2], transformMatrix);

			// Work out the normal of the triangle
			Vec4<T> line1 = SubtractVectors(transformed.p[1], transformed.p[0]);
			Vec4<T> line2 = SubtractVectors(transformed.p[2], transformed.p[0]);
			Vec4<T> normal = UnitVector(CrossProduct(line1, line2));

			Vec4<T> fromCameraToTriangle = SubtractVectors(transformed.p[0], camera.position);
			T dot = DotProduct(normal, fromCameraToTriangle);

			if (dot >= (T)0)
			{
				Vec4<T> lightDirection = { (T)0, (T)0, (T)1 };
				Vec4<T> normalizedLightDirection = UnitVector(lightDirection);
				T shade = DotProduct(normal, normalizedLightDirection);

				unsigned int triangleColor = GetColorFromRGB(int(RED * shade), int(GREEN * shade), int(BLUE * shade));

				// Convert the triangle position from world space to view space
				MultiplyVectorWithMatrix(transformed.p[0], viewed.p[0], viewMatrix);
				MultiplyVectorWithMatrix(transformed.p[1], viewed.p[1], viewMatrix);
				MultiplyVectorWithMatrix(transformed.p[2], viewed.p[2], viewMatrix);

				// Clip the triangles before they get projected. Define a plane just in fron of the camera to clip against
				Triangle4d<T> clipped[2];
				Plane<T> inFrontOfScreen = { (T)0, (T)0, (T)0.1,	 (T)0, (T)0, (T)1 };
				int clippedTriangleCount = ClipTriangleAgainstPlane(inFrontOfScreen, viewed, clipped[0], clipped[1]);

				for (int i = 0; i < clippedTriangleCount; i += 1)
				{
					// Project each triangle in 3D space onto the 2D space triangle to render
					Project3DPointTo2D(clipped[i].p[0], projected.p[0], projectionMatrix);
					Project3DPointTo2D(clipped[i].p[1], projected.p[1], projectionMatrix);
					Project3DPointTo2D(clipped[i].p[2], projected.p[2], projectionMatrix);

					// Scale to view
					const float sf = 500.0f;
					Triangle4d<T> triToRender = projected;
					triToRender.p[0].x *= sf;
					triToRender.p[0].y *= sf;
					triToRender.p[1].x *= sf;
					triToRender.p[1].y *= sf;
					triToRender.p[2].x *= sf;
					triToRender.p[2].y *= sf;

					const T translateX = (T)0.5 * (T)renderBuffer.width;
					const T translateY = (T)0.5 * (T)renderBuffer.height;
					triToRender.p[0].x += translateX; triToRender.p[0].y += translateY;
					triToRender.p[1].x += translateX; triToRender.p[1].y += translateY;
					triToRender.p[2].x += translateX; triToRender.p[2].y += translateY;

					triToRender.color = triangleColor;

					trianglesToDraw.push_back(triToRender);
				}
			}
		}

		for (Triangle4d<T> triToRender : trianglesToDraw)
		{
			Triangle4d<T> clipped[2];
			std::list<Triangle4d<T>> triangleQueue;
			triangleQueue.push_back(triToRender);
			int newTriangles = 1;

			// Clip against each screen edge
			for (int edge = 0; edge < 4; edge += 1)
			{
				int trianglesToAdd = 0;
				while (newTriangles > 0)
				{
					Triangle4d<T> test = triangleQueue.front();
					triangleQueue.pop_front();
					newTriangles -= 1;

					switch (edge)
					{
						case 0:
						{
							trianglesToAdd = ClipTriangleAgainstPlane(bottomOfScreen, test, clipped[0], clipped[1]);
							break;
						}
						case 1:
						{
							trianglesToAdd = ClipTriangleAgainstPlane(topOfScreen, test, clipped[0], clipped[1]);
							break;
						}
						case 2:
						{
							trianglesToAdd = ClipTriangleAgainstPlane(leftOfScreen, test, clipped[0], clipped[1]);
							break;
						}
						case 3:
						{
							trianglesToAdd = ClipTriangleAgainstPlane(rightOfScreen, test, clipped[0], clipped[1]);
							break;
						}
					}

					for (int i = 0; i < trianglesToAdd; i += 1)
					{
						triangleQueue.push_back(clipped[i]);
					}
				}

				newTriangles = (int)triangleQueue.size();
			}

			for (Triangle4d<T> draw : triangleQueue)
			{
				// Vec2<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
				// Vec2<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
				// Vec2<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };
				// DrawTriangleInPixels(renderBuffer, 0xFFFFFF, p0Int, p1Int, p2Int);

				Vec3<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
				Vec3<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
				Vec3<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };

				// Super rough, take the depth as the average z value
				// For whatever reason, the z values are inverted for the teapot. i.e. closer triangles have a lower Z value.
				// As an ultra-hack I'm doing 10 minus the z-value to invert them.
				T z = (T)10 - ((draw.p[0].z + draw.p[1].z + draw.p[2].z) / (T)3);
				FillTriangleInPixels(renderBuffer, draw.color, p0Int, p1Int, p2Int, z);
			}
		}
	}
	template void TransformAndRenderMesh(const RenderBuffer &renderBuffer, const Mesh<float> &mesh, const Camera<float> &camera, const Matrix4x4<float> transformMatrix, const Matrix4x4<float> projectionMatrix);


	// Render characters
	const float CHARACTER_HEIGHT = 7.0f;
	Sprite letters[26] = {
	LoadSprite("\
 00\n\
0  0\n\
0  0\n\
0000\n\
0  0\n\
0  0\n\
0  0"),

	LoadSprite("\
000\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
000"),

	LoadSprite("\
 000\n\
0\n\
0\n\
0\n\
0\n\
0\n\
 000"),

	LoadSprite("\
000\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
000"),

	LoadSprite("\
0000\n\
0\n\
0\n\
000\n\
0\n\
0\n\
0000"),

	LoadSprite("\
0000\n\
0\n\
0\n\
000\n\
0\n\
0\n\
0"),

	LoadSprite("\
 000\n\
0\n\
0\n\
0 00\n\
0  0\n\
0  0\n\
 000"),

	LoadSprite("\
0  0\n\
0  0\n\
0  0\n\
0000\n\
0  0\n\
0  0\n\
0  0"),

	LoadSprite("\
000\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
000"),

	LoadSprite("\
 000\n\
   0\n\
   0\n\
   0\n\
0  0\n\
0  0\n\
 000"),

	LoadSprite("\
0  0\n\
0  0\n\
0 0\n\
00\n\
0 0\n\
0  0\n\
0  0"),

	LoadSprite("\
0\n\
0\n\
0\n\
0\n\
0\n\
0\n\
0000"),

	LoadSprite("\
00 00\n\
0 0 0\n\
0 0 0\n\
0   0\n\
0   0\n\
0   0\n\
0   0"),

	LoadSprite("\
0   0\n\
0   0\n\
00  0\n\
0 0 0\n\
0  00\n\
0   0\n\
0   0"),

	LoadSprite("\
 00 \n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
 00"),

	LoadSprite("\
000\n\
0  0\n\
0  0\n\
000\n\
0\n\
0\n\
0"),

	LoadSprite("\
 000\n\
0   0\n\
0   0\n\
0   0\n\
0 0 0\n\
0  0\n\
 00 0"),

	LoadSprite("\
000\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
0  0"),

	LoadSprite("\
 000\n\
0\n\
0\n\
 00\n\
   0\n\
   0\n\
000"),

	LoadSprite("\
000\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0"),

	LoadSprite("\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
 00"),

	LoadSprite("\
0   0\n\
0   0\n\
0   0\n\
 0 0\n\
 0 0\n\
  0\n\
  0"),

	LoadSprite("\
0   0\n\
0   0\n\
0   0\n\
0 0 0\n\
0 0 0\n\
 0 0\n\
 0 0"),

	LoadSprite("\
0   0\n\
0   0\n\
 0 0\n\
  0\n\
 0 0\n\
0   0\n\
0   0"),

	LoadSprite("\
0   0\n\
0   0\n\
 0 0\n\
 0 0\n\
  0\n\
  0\n\
  0"),

	LoadSprite("\
0000\n\
   0\n\
  0\n\
 0\n\
0\n\
0\n\
0000")
	};

	Sprite digits[10] = {
	LoadSprite("\
 000 \n\
0   0\n\
0  00\n\
0 0 0\n\
00  0\n\
0   0\n\
 000"),

	LoadSprite("\
 0\n\
00\n\
 0\n\
 0\n\
 0\n\
 0\n\
000"),

	LoadSprite("\
 00\n\
0  0\n\
   0\n\
  0\n\
 0\n\
0\n\
0000"),

	LoadSprite("\
 00\n\
0  0\n\
   0\n\
 00\n\
   0\n\
0  0\n\
 00"),

	LoadSprite("\
  00\n\
 0 0\n\
0  0\n\
0000\n\
   0\n\
   0\n\
   0"),

	LoadSprite("\
0000\n\
0\n\
0\n\
000\n\
   0\n\
   0\n\
000"),

	LoadSprite("\
 000\n\
0\n\
0\n\
000\n\
0  0\n\
0  0\n\
 00"),

	LoadSprite("\
0000\n\
   0\n\
   0\n\
  0\n\
 0\n\
0\n\
0"),

	LoadSprite("\
 00\n\
0  0\n\
0  0\n\
 00\n\
0  0\n\
0  0\n\
 00"),

	LoadSprite("\
 00\n\
0  0\n\
0  0\n\
 00\n\
  0\n\
 0\n\
0")
	};
	Sprite negSprite = LoadSprite("\
\n\
\n\
\n\
0000\n\
\n\
\n\
");

	int GetLetterIndex(char c)
	{
		return c - 'A';
	}

	Sprite LoadSprite(char* content)
	{
		char* copy = content;
		int height = 0;
		int width = 0;
		int rowCounter = 0;
		while (*copy)
		{
			if (rowCounter == 0 && height == 0)
			{
				height += 1;
			}
			if (*copy == '\n')
			{
				if (width < rowCounter)
				{
					width = rowCounter;
				}
				rowCounter = 0;
				height += 1;
			}
			else
			{
				rowCounter += 1;
			}
			copy++;
		}

		// Check the final row (it may not end in a \n char)
		if (width < rowCounter)
		{
			width = rowCounter;
		}

		// Set the width value if no newline char exists in the content
		if (height == 1)
		{
			width = rowCounter;
		}

		Sprite sprite = Sprite();
		sprite.content = content;
		sprite.height = height;
		sprite.width = width;
		return sprite;
	}

	void DrawAlphabetCharacters(
		const RenderBuffer &renderBuffer,
		char *text,
		const tl::Rect<float> &firstCharFootprint,
		uint32_t color
	)
	{
		Rect<float> charRect;
		charRect.halfSize = firstCharFootprint.halfSize;
		charRect.position = tl::Vec2<float> { firstCharFootprint.position.x, firstCharFootprint.position.y };
		float spaceWidth = 0.2f * firstCharFootprint.halfSize.x;
		float charWidth = (2.0f * firstCharFootprint.halfSize.x) + spaceWidth;
		for (char *letterAt = text; *letterAt; letterAt++)
		{
			if (*letterAt != ' ')
			{
				int letterIndex = GetLetterIndex(*letterAt);
				Sprite letter = letters[letterIndex];

				tl::DrawSprite(renderBuffer, letter, charRect, color);
			}
			charRect.position.x += charWidth;
		}
	}

	void DrawNumber(
		const RenderBuffer &renderBuffer,
		int number,
		const tl::Rect<float> &firstCharFootprint,
		uint32_t color
	)
	{
		Rect<float> charRect;
		charRect.halfSize = firstCharFootprint.halfSize;
		charRect.position = tl::Vec2<float> { firstCharFootprint.position.x, firstCharFootprint.position.y };
		float spaceWidth = 0.2f * firstCharFootprint.halfSize.x;
		float charWidth = (2.0f * firstCharFootprint.halfSize.x) + spaceWidth;

		// Special case: zero
		if (number == 0)
		{
			tl::DrawSprite(renderBuffer, digits[0], charRect, color);
			return;
		}

		// Handle negative numbers
		int safeNumber = (number < 0) ? -1 * number : number;
		if (number < 0)
		{
			tl::DrawSprite(renderBuffer, negSprite, charRect, color);
			charRect.position.x += charWidth;
		}

		int baseTenMultiplier = 1;
		int digit = safeNumber / baseTenMultiplier;
		int digitCount = 1;
		while (digit > 0)
		{
			baseTenMultiplier *= 10;
			digitCount += 1;
			digit = safeNumber / baseTenMultiplier;
		}

		int workingNumber = safeNumber;
		while (baseTenMultiplier >= 10)
		{
			baseTenMultiplier /= 10;
			digit = workingNumber / baseTenMultiplier;

			workingNumber -= (digit * baseTenMultiplier);

			Sprite charDigit = digits[digit];
			tl::DrawSprite(renderBuffer, charDigit, charRect, color);

			charRect.position.x += charWidth;
		}
	}
}

