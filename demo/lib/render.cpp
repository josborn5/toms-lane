#include "./render.hpp"


void DrawTriangleInPixels(const tl::RenderBuffer& renderBuffer, uint32_t color, const tl::Vec2<int>& p0, const tl::Vec2<int>& p1, const tl::Vec2<int>& p2)
{
	if (p0.y < 0 || p0.y > renderBuffer.height - 1) return;
	if (p1.y < 0 || p1.y > renderBuffer.height - 1) return;
	if (p2.y < 0 || p2.y > renderBuffer.height - 1) return;

	tl::DrawLineInPixels(renderBuffer, color, p0, p1);
	tl::DrawLineInPixels(renderBuffer, color, p1, p2);
	tl::DrawLineInPixels(renderBuffer, color, p2, p0);
}

static unsigned int z_buffer_get_index(
	const z_buffer& buffer,
	float plane_x_coef,
	float plane_y_coef,
	float place_z_coef,
	float x, float y
) {
	return (y * buffer.width) + x;
}

float plane_z_value_get(
	float x,
	float y,
	const plane_coeff& coefficients
) {
	float z0 = (-coefficients.d - (coefficients.b * y) - (coefficients.a * x)) / coefficients.c;
	return z0;
}

/**
 *	|---|---|---|
 *	| 0 | 1 | 2 |	pixel ordinals
 *	|---|---|---|
 *	0   1   2   3	position ordinals
 *
 * x1, x2 & y parameters are the pixel and NOT the position ordinals
 */
static void DrawHorizontalLineInPixels(
	const tl::RenderBuffer& renderBuffer,
	z_buffer& depth_buffer,
	uint32_t color,
	int x0,
	int x1,
	int y,
	float z0,
	float dz_dx
) {
	int start_x = x0;
	int end_x = x1;
	if (x0 > x1) {
		start_x = x1;
		end_x = x0;
	}
	int start_row_position = renderBuffer.width * y;
	int start_position = start_row_position + start_x;
	uint32_t* pixel = renderBuffer.pixels + start_position;
	float* pixel_depth = depth_buffer.depths + start_position;

	float running_depth = z0;

	for (int i = start_x; i <= end_x; i += 1)
	{
		float existing_depth = *pixel_depth;

		// small z is closer
		if (running_depth > existing_depth) {
			pixel++;
			pixel_depth++;
			running_depth += dz_dx;
			continue;
		}

		*pixel = color;
		*pixel_depth = running_depth;

		pixel++;
		pixel_depth++;
		running_depth += dz_dx;
	}
}


/*	p0------p1
 *	\       /	|
 *	 \     /	|
 *	  \   /		V
 *	   \ /	  +ve y (if +ve y is up, this is actually a flat bottom triangle)
 *	    p2
 */
static void FillFlatTopTriangle(
	const tl::RenderBuffer& renderBuffer,
	z_buffer& depth_buffer,
	uint32_t color,
	const tl::Vec3<float>& p0,
	const tl::Vec3<float>& p1,
	const tl::Vec3<float>& p2,
	plane_coeff& coefficients
) {
	// a.x + b.y + c.z + d = 0
	// z = (-d - b.y - a.x) / c
	// dz/dx = -a / c
	float z_delta_per_x = -coefficients.a / coefficients.c;

	// LINE 0-->2
	bool p2IsRightOfP0 = (p0.x < p2.x);
	int xDiff0 = (p2IsRightOfP0) ? (int)p2.x - (int)p0.x : (int)p0.x - (int)p2.x;
	int yDiff0 = (int)p2.y - (int)p0.y;

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
	int xDiff1 = (p2IsLeftOfP1) ? (int)p1.x - (int)p2.x : (int)p2.x - (int)p1.x;

	bool isLongDimension1X = (yDiff0 < xDiff1);
	int longDelta1 = (isLongDimension1X) ? xDiff1 : yDiff0;
	int shortDelta1 = (isLongDimension1X) ? yDiff0 : xDiff1;

	int negIncrement1 = 2 * shortDelta1;
	int acc1 = negIncrement1 - longDelta1;
	int posIncrement1 = negIncrement1 - (2 * longDelta1);
	int x1Increment = (p2IsLeftOfP1) ? -1 : 1;

	// Copy the x & y values for p0 & p1 so we can modify them safely inside this function
	// Note that p0.y == p1.y so we only need one variable for the y position
	int x0 = (int)p0.x;
	int x1 = (int)p1.x;
	for (int y = (int)p0.y; y <= (int)p2.y; y += 1)
	{
		// a.x + b.y + c.z + d = 0
		// z = (-d - b.y - a.x) / c
		float z0 = (-coefficients.d - (coefficients.b * (float)y) - (coefficients.a * (float)x0)) / coefficients.c;

		// draw scanline to fill in triangle between x0 & x1
		DrawHorizontalLineInPixels(renderBuffer,
			depth_buffer,
			color, x0, x1, y,
			z0,
			z_delta_per_x);

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
static void FillFlatBottomTriangle(
	const tl::RenderBuffer& renderBuffer,
	z_buffer& depth_buffer,
	uint32_t color,
	const tl::Vec3<float>& p0,
	const tl::Vec3<float>& p1,
	const tl::Vec3<float>& p2,
	const plane_coeff& coefficients
) {
	// a.x + b.y + c.z + d = 0
	// z = (-d - b.y - a.x) / c
	// dz/dx = -a / c
	float z_delta_per_x = -coefficients.a / coefficients.c;

	// LINE 0-->1
	bool p1IsLeftOfP0 = (p1.x < p0.x);
	int xDiff0 = (p1IsLeftOfP0) ? (int)p0.x - (int)p1.x : (int)p1.x - (int)p0.x;
	int yDiff0 = (int)p1.y - (int)p0.y;

	bool isLongDimension0X = (yDiff0 < xDiff0);
	int longDelta0 = (isLongDimension0X) ? xDiff0 : yDiff0;
	int shortDelta0 = (isLongDimension0X) ? yDiff0 : xDiff0;

	int negIncrement0 = shortDelta0 + shortDelta0;
	int acc0 = negIncrement0 - longDelta0;
	int posIncrement0 = negIncrement0 - (longDelta0 + longDelta0);
	int x0Increment = (p1IsLeftOfP0) ? -1 : 1;

	// LINE 0-->2
	// Vertical distance for 1-->2 is the same as 0-->2, so no need for a separate yDiff1 variable. Can reuse yDiff0.
	bool p2IsRightOfP0 = (p0.x < p2.x);
	int xDiff1 = (p2IsRightOfP0) ? (int)p2.x - (int)p0.x : (int)p0.x - (int)p2.x;

	bool isLongDimension1X = (yDiff0 < xDiff1);
	int longDelta1 = (isLongDimension1X) ? xDiff1 : yDiff0;
	int shortDelta1 = (isLongDimension1X) ? yDiff0 : xDiff1;

	int negIncrement1 = shortDelta1 + shortDelta1;
	int acc1 = negIncrement1 - longDelta1;
	int posIncrement1 = negIncrement1 - (longDelta1 + longDelta1);
	int x1Increment = (p2IsRightOfP0) ? 1 : -1;

	// Copy the x & y values for p0 & p1 so we can modify them safely inside this function
	// Note that p0.y == p1.y so we only need one variable for the y position
	int x0 = (int)p0.x;
	int x1 = (int)p0.x;

	for (int y = (int)p0.y; y <= (int)p1.y - 1; y += 1)
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

		// a.x + b.y + c.z + d = 0
		// z = (-d - b.y - a.x) / c
		float z0 = (-coefficients.d - (coefficients.b * (float)y) - (coefficients.a * (float)x0)) / coefficients.c;

		// draw scanline to fill in triangle between x0 & x1
		DrawHorizontalLineInPixels(
			renderBuffer, depth_buffer,
			color, x0, x1, y,
			z0, z_delta_per_x);

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

	// a.x + b.y + c.z + d = 0
	// z = (-d - b.y - a.x) / c
	float z0 = (-coefficients.d - (coefficients.b * (float)p1.y) - (coefficients.a * (float)p1.x)) / coefficients.c;

	// draw final scanline to fill in triangle between x0 & x1
	DrawHorizontalLineInPixels(
		renderBuffer,
		depth_buffer,
		color,
		p1.x,
		p2.x,
		p1.y,
		z0,
		z_delta_per_x);
}

void fill_triangle_plane_coeff(
	const tl::Vec3<float>& p0,
	const tl::Vec3<float>& p1,
	const tl::Vec3<float>& p2,
	plane_coeff& coefficients
) {
	tl::Vec3<float> p0_to_p1 = tl::SubtractVectors(p1, p0);
	tl::Vec3<float> p0_to_p2 = tl::SubtractVectors(p2, p0);

	tl::Vec3<float> normal = tl::CrossProduct(p0_to_p1, p0_to_p2);

	// a.x + b.y + c.z + d = 0

	coefficients.a = normal.x;
	coefficients.b = normal.y;
	coefficients.c = normal.z;

	coefficients.d = -(coefficients.a * p0.x) - (coefficients.b * p0.y) - (coefficients.c * p0.z);
};

void triangle_fill(
	const tl::RenderBuffer& render_buffer,
	z_buffer& depth_buffer,
	uint32_t color,
	const tl::Vec3<float>& p0,
	const tl::Vec3<float>& p1,
	const tl::Vec3<float>& p2
) {
	const tl::Vec3<float>* pp0 = &p0;
	const tl::Vec3<float>* pp1 = &p1;
	const tl::Vec3<float>* pp2 = &p2;

	plane_coeff coefficients;
	fill_triangle_plane_coeff(
		p0, p1, p2,
		coefficients
	);

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
	if (pp0->y == pp1->y)
	{
		// sort top two points of flat top by their x co-ordinate
		if (pp1->x < pp0->x)
		{
			tl::swap(pp0, pp1);
		}
		FillFlatTopTriangle(render_buffer, depth_buffer, color, *pp0, *pp1, *pp2, coefficients);
	}
	else if (pp1->y == pp2->y) // natural flat bottom
	{
		// sort bottom two points of flat bottom by their x co-ordinate
		if (pp2->x < pp1->x)
		{
			tl::swap(pp1, pp2);
		}
		FillFlatBottomTriangle(render_buffer, depth_buffer, color, *pp0, *pp1, *pp2, coefficients);
	}
	else // general triangle
	{
		// Start scanning vertically from the top point (lowest y value) to the center with the flat bottom logic.
		// Then when we reach the center point, continue scanning but switch to the flat top triangle logic until we reach the bottom point (highest y value).
		// That should mean there is no need worry about finding the split point.

		// a.x + b.y + c.z + d = 0
		// z = (-d - b.y - a.x) / c
		// dz/dx = -a / c
		float z_delta_per_x = -coefficients.a / coefficients.c;

		// At this point we know that p0 has lowest y value. But we need to work out if p1 is left or right of p2 in order to start scanning.
		bool pp1xIsLessThanPp2X = (pp1->x < pp2->x);
		const tl::Vec3<float>* leftPoint = (pp1xIsLessThanPp2X) ? pp1 : pp2;
		const tl::Vec3<float>* rightPoint = (pp1xIsLessThanPp2X) ? pp2 : pp1;

		// LEFT LINE
		bool leftPointIsLeftOfP0 = (leftPoint->x < pp0->x);
		int xDiff0 = (leftPointIsLeftOfP0) ? int(pp0->x - leftPoint->x) : int(leftPoint->x - pp0->x);
		int yDiff0 = (int)(leftPoint->y - pp0->y);

		bool isLongDimension0X = (yDiff0 < xDiff0);
		int longDelta0 = (isLongDimension0X) ? xDiff0 : yDiff0;
		int shortDelta0 = (isLongDimension0X) ? yDiff0 : xDiff0;

		int negIncrement0 = 2 * shortDelta0;
		int acc0 = negIncrement0 - longDelta0;
		int posIncrement0 = negIncrement0 - (2 * longDelta0);
		int x0Increment = (leftPointIsLeftOfP0) ? -1 : 1;

		// RIGHT LINE
		bool rightPointIsRightOfP0 = (pp0->x < rightPoint->x);
		int xDiff1 = (rightPointIsRightOfP0) ? int(rightPoint->x - pp0->x) : int(pp0->x - rightPoint->x);
		int yDiff1 = int(rightPoint->y - pp0->y);

		bool isLongDimension1X = (yDiff1 < xDiff1);
		int longDelta1 = (isLongDimension1X) ? xDiff1 : yDiff1;
		int shortDelta1 = (isLongDimension1X) ? yDiff1 : xDiff1;

		int negIncrement1 = 2 * shortDelta1;
		int acc1 = negIncrement1 - longDelta1;
		int posIncrement1 = negIncrement1 - (2 * longDelta1);
		int x1Increment = (rightPointIsRightOfP0) ? 1 : -1;

		// Copy the x & y values for leftPoint & rightPoint so we can modify them safely inside this function
		int x0 = (int)pp0->x;
		int x1 = (int)pp0->x;

		for (int y = (int)pp0->y; y <= (int)pp1->y - 1; y += 1)	// Note that p1.y has already been sorte to be the vertical mid point of the triangle
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

			// a.x + b.y + c.z + d = 0
			// z = (-d - b.y - a.x) / c
			float z0 = (-coefficients.d - (coefficients.b * (float)y) - (coefficients.a * (float)x0)) / coefficients.c;

			// draw scanline to fill in triangle between x0 & x1
			DrawHorizontalLineInPixels(render_buffer,
				depth_buffer,
				color, x0, x1, y,
				z0,
				z_delta_per_x
			);

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
			tl::Vec3<float> intermediatePoint = { (float)x1, pp1->y, 0 };
			FillFlatTopTriangle(render_buffer, depth_buffer, color, *pp1, intermediatePoint, *pp2, coefficients);
		}
		else	// pp1->y is the rightPoint. i.e. Left major triangle
		{
			tl::Vec3<float> intermediatePoint = { (float)x0, pp1->y, 0 };
			FillFlatTopTriangle(render_buffer, depth_buffer, color, intermediatePoint, *pp1, *pp2, coefficients);
		}
	}
}


