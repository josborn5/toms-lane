#include "../tl-library.hpp"

void DrawTriangleInPixels(const tl::RenderBuffer& renderBuffer, uint32_t color, const tl::Vec2<int>& p0, const tl::Vec2<int>& p1, const tl::Vec2<int>& p2)
{
	tl::DrawLineInPixels(renderBuffer, color, p0, p1);
	tl::DrawLineInPixels(renderBuffer, color, p1, p2);
	tl::DrawLineInPixels(renderBuffer, color, p2, p0);
}


/**
 *	|---|---|---|
 *	| 0 | 1 | 2 |	pixel ordinals
 *	|---|---|---|
 *	0   1   2   3	position ordinals
 *
 * x1, x2 & y parameters are the pixel and NOT the position ordinals
 */
static void DrawHorizontalLineInPixels(const tl::RenderBuffer& renderBuffer, uint32_t color, int x0, int x1, int y)
{
	int start_x = x0;
	int end_x = x1;
	if (x0 > x1) {
		start_x = x1;
		end_x = x0;
	}
	int start_row_position = renderBuffer.width * y;
	int start_position = start_row_position + start_x;
	uint32_t* pixel = renderBuffer.pixels + start_position;
	for (int i = start_x; i <= end_x; i += 1)
	{
		*pixel = color;
		pixel++;
	}
}


/*	p0------p1
 *	\       /	|
 *	 \     /	|
 *	  \   /		V
 *	   \ /	  +ve y (if +ve y is up, this is actually a flat bottom triangle)
 *	    p2
 */
void FillFlatTopTriangle(const tl::RenderBuffer& renderBuffer, uint32_t color, const tl::Vec3<int>& p0, const tl::Vec3<int>& p1, const tl::Vec3<int>& p2)
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
		DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y);

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
void FillFlatBottomTriangle(const tl::RenderBuffer& renderBuffer, uint32_t color, const tl::Vec3<int>& p0, const tl::Vec3<int>& p1, const tl::Vec3<int>& p2)
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
		DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y);

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
	DrawHorizontalLineInPixels(renderBuffer, color, p1.x, p2.x, p1.y);
}

void FillTriangleInPixels(const tl::RenderBuffer& renderBuffer, uint32_t color, const tl::Vec3<int>& p0, const tl::Vec3<int>& p1, const tl::Vec3<int>& p2)
{
	const tl::Vec3<int>* pp0 = &p0;
	const tl::Vec3<int>* pp1 = &p1;
	const tl::Vec3<int>* pp2 = &p2;

	/* Sort the three points of the triangle by their y co-ordinate
	 *
	 * pp0->y	low		|
	 * pp1->y			|
	 * pp2->y	high	V
	 *				  +ve y
	 */
	if (pp1->y < pp0->y)
	{
		tl::swap(pp0, pp1);
	}
	if (pp2->y < pp1->y)
	{
		tl::swap(pp1, pp2);
	}
	if (pp1->y < pp0->y)
	{
		tl::swap(pp0, pp1);
	}

	// Check for natural flat top
	if (pp0 -> y == pp1->y)
	{
		// sort top two points of flat top by their x co-ordinate
		if (pp1->x < pp0->x)
		{
			tl::swap(pp0, pp1);
		}
		FillFlatTopTriangle(renderBuffer, color, *pp0, *pp1, *pp2);
	}
	else if (pp1->y == pp2->y) // natural flat bottom
	{
		// sort bottom two points of flat bottom by their x co-ordinate
		if (pp2->x < pp1->x)
		{
			tl::swap(pp1, pp2);
		}
		FillFlatBottomTriangle(renderBuffer, color, *pp0, *pp1, *pp2);
	}
	else // general triangle
	{
		// Start scanning vertically from the top point (lowest y value) to the center with the flat bottom logic.
		// Then when we reach the center point, continue scanning but switch to the flat top triangle logic until we reach the bottom point (highest y value).
		// That should mean there is no need worry about finding the split point.

		// At this point we know that p0 has lowest y value. But we need to work out if p1 is left or right of p2 in order to start scanning.
		bool pp1xIsLessThanPp2X = (pp1->x < pp2->x);
		const tl::Vec3<int>* leftPoint = (pp1xIsLessThanPp2X) ? pp1 : pp2;
		const tl::Vec3<int>* rightPoint = (pp1xIsLessThanPp2X) ? pp2 : pp1;

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
			DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y);

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
			tl::Vec3<int> intermediatePoint = { x1, pp1->y, 0 };
			FillFlatTopTriangle(renderBuffer, color, *pp1, intermediatePoint, *pp2);
		}
		else	// pp1->y is the rightPoint. i.e. Left major triangle
		{
			tl::Vec3<int> intermediatePoint = { x0, pp1->y, 0 };
			FillFlatTopTriangle(renderBuffer, color, intermediatePoint, *pp1, *pp2);
		}
	}
}


