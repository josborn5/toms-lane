void RunLineDrawTest(tl::Vec2<int> p0, tl::Vec2<int> p1, uint32_t* expectedPixels)
{
	uint32_t pixelArray[18] = { EMPTY };	// define pixels as an an array of 16 uint32_t values
											// NB this array lives on the stack in the scope of the RunSoftwareRenderingTests function only.
											// The array is sized greater than the tl::RenderBuffer pixel array so it can pick up illegal memory writes to the pixel array
	ClearPixelArray(pixelArray, 18);

	/**
	 * Set the tl::RenderBuffer to be a 4x4 grid of pixels (pixel ordinals 0 - 3)
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|-0
	 *	0 |   |   |   |   |
	 *	  |---|---|---|---|-1
	 *	1 |   |   |   |   |
	 *	  |---|---|---|---|-2
	 *	2 |   |   |   |   |
	 *	  |---|---|---|---|-3
	 *	3 |   |   |   |   |
	 *	  |---|---|---|---|-4
	 *    0   1   2   3   4
	 */
	tl::RenderBuffer renderBuffer;
	renderBuffer.height = 4;
	renderBuffer.width = 4;					// Size the buffer to 16 pixels. pixelArray is 18 pixels so the test can tell if the function ever oversteps the bounds of tl::RenderBuffer.
	renderBuffer.pixels = &pixelArray[1];	// Use the second element in pixelArray so we can tell if the zero-th element ever gets accessed.

	tl::DrawLineInPixels(renderBuffer, FILLED, p0, p1);

	assert(pixelArray[0] == EMPTY);	// Should NEVER get written to

	for (int i = 0; i < renderBuffer.height * renderBuffer.width; i += 1)
	{
		assert(pixelArray[i + 1] == expectedPixels[i]);
	}

	assert(pixelArray[17] == EMPTY);	// Should NEVER get written to
}

void RunLineDrawingTests()
{
		/**
	 * STRAIGHT HORIZONTAL LINE TO BOUNDARY OF BUFFER
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	1 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	2 | O | x | x | O |
	 *	  |---|---|---|---|
	 *	3 |   |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el1[16] = {
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 2 }, tl::Vec2<int>{ 3, 2 }, el1);
	RunLineDrawTest(tl::Vec2<int>{ 3, 2 }, tl::Vec2<int>{ 0, 2 }, el1);

	/**
	 * STRAIGHT VERTICAL LINE TO BOUNDARY OF BUFFER
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 | O |   |   |   |
	 *	  |---|---|---|---|
	 *	1 | x |   |   |   |
	 *	  |---|---|---|---|
	 *	2 | x |   |   |   |
	 *	  |---|---|---|---|
	 *	3 | O |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el2[16] = {
		FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 3 }, tl::Vec2<int>{ 0, 0 }, el2);
	RunLineDrawTest(tl::Vec2<int>{ 0, 0 }, tl::Vec2<int>{ 0, 3 }, el2);

	/**
	 * DIAGONAL LINE TO BOUNDARY OF BUFFER
	 * X +VE INCREMENT, Y +VE INCREMENT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 | O |   |   |   |
	 *	  |---|---|---|---|
	 *	1 |   | x |   |   |
	 *	  |---|---|---|---|
	 *	2 |   |   | x |   |
	 *	  |---|---|---|---|
	 *	3 |   |   |   | O |
	 *	  |---|---|---|---|
	 */
	uint32_t el3[16] = {
		FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	FILLED
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 0 }, tl::Vec2<int>{ 3, 3 }, el3);
	RunLineDrawTest(tl::Vec2<int>{ 3, 3 }, tl::Vec2<int>{ 0, 0 }, el3);

	/**
	 * DIAGONAL LINE TO BOUNDARY OF BUFFER
	 * X +VE INCREMENT, Y -VE INCREMENT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   | O |
	 *	  |---|---|---|---|
	 *	1 |   |   | x |   |
	 *	  |---|---|---|---|
	 *	2 |   | x |   |   |
	 *	  |---|---|---|---|
	 *	3 | O |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el4[16] = {
		EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	FILLED,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 3 }, tl::Vec2<int>{ 3, 0 }, el4);
	RunLineDrawTest(tl::Vec2<int>{ 3, 0 }, tl::Vec2<int>{ 0, 3 }, el4);

	/**
	 * STEEP GRADIENT, X +VE INCREMENT, Y +VE INCREMENT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   | O |   |   |
	 *	  |---|---|---|---|
	 *	1 |   | x |   |   |
	 *	  |---|---|---|---|
	 *	2 |   |   | x |   |
	 *	  |---|---|---|---|
	 *	3 |   |   | O |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el5[16] = {
		EMPTY,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 1, 0 }, tl::Vec2<int>{ 2, 3 }, el5);
	RunLineDrawTest(tl::Vec2<int>{ 2, 3 }, tl::Vec2<int>{ 1, 0 }, el5);

	/**
	 * SHALLOW GRADIENT, X +VE INCREMENT, Y +VE INCREMENT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	1 | O | x |   |   |
	 *	  |---|---|---|---|
	 *	2 |   |   | x | O |
	 *	  |---|---|---|---|
	 *	3 |   |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el6[16] = {
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 1 }, tl::Vec2<int>{ 3, 2 }, el6);
	RunLineDrawTest(tl::Vec2<int>{ 3, 2 }, tl::Vec2<int>{ 0, 1 }, el6);

	/**
	 * SHALLOW GRADIENT, X +VE INCREMENT, Y -VE INCREMENT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	1 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	2 |   |   | x | O |
	 *	  |---|---|---|---|
	 *	3 | O | x |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el7[16] = {
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	FILLED,
		FILLED,	FILLED,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 3 }, tl::Vec2<int>{ 3, 2 }, el7);
	RunLineDrawTest(tl::Vec2<int>{ 3, 2 }, tl::Vec2<int>{ 0, 3 }, el7);

	/**
	 * STEEP GRADIENT, X +VE INCREMENT, Y +VE INCREMENT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   | O |
	 *	  |---|---|---|---|
	 *	1 |   |   | x |   |
	 *	  |---|---|---|---|
	 *	2 |   |   | x |   |
	 *	  |---|---|---|---|
	 *	3 |   | O |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el8[16] = {
		EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	FILLED,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 3, 0 }, tl::Vec2<int>{ 1, 3 }, el8);
	RunLineDrawTest(tl::Vec2<int>{ 1, 3 }, tl::Vec2<int>{ 3, 0 }, el8);

	/**
	 * SHORT, SHALLOW GRADIENT - RIGHT TO LEFT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   | O |
	 *	  |---|---|---|---|
	 *	1 |   | O | x |   |
	 *	  |---|---|---|---|
	 *	2 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	3 |   |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el9[16] = {
		EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 3, 0 }, tl::Vec2<int>{ 1, 1 }, el9);

	/**
	 * SHORT, SHALLOW GRADIENT - LEFT TO RIGHT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   | x | O |
	 *	  |---|---|---|---|
	 *	1 |   | O |   |   |
	 *	  |---|---|---|---|
	 *	2 |   |   |   |   |
	 *	  |---|---|---|---|
	 *	3 |   |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el10[16] = {
		EMPTY,	EMPTY,	FILLED,	FILLED,
		EMPTY,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 1, 1 }, tl::Vec2<int>{ 3, 0 }, el10);
}
