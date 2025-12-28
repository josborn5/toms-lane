#include <assert.h>
#include <iostream>
#include <limits>
#include "../render.hpp"
#include "./math.tests.hpp"
#include "./camera.tests.hpp"

const uint32_t EMPTY = 0x000000;
const uint32_t FILLED = 0xFFFFFF;

static void ClearPixelAndDepthArray(uint32_t* pixelArray, float* depth_array, int arrayLength)
{
	for (int i = 0; i < arrayLength; i += 1)
	{
		pixelArray[i] = EMPTY;
		depth_array[i] = std::numeric_limits<float>::max();
	}
}

static void Run4x4FillTriangleTest(tl::Vec3<int> p0, tl::Vec3<int> p1, tl::Vec3<int> p2, uint32_t* expectedPixels)
{
	uint32_t pixelArray[18] = { EMPTY };	// define pixels as an an array of 16 uint32_t values
											// NB this array lives on the stack in the scope of the RunSoftwareRenderingTests function only.
											// The array is sized greater than the tl::RenderBuffer pixel array so it can pick up illegal memory writes to the pixel array

	float depthArray[10] = { 0 };
	ClearPixelAndDepthArray(pixelArray, depthArray, 18);
	z_buffer depth_buffer;
	depth_buffer.width = 4;
	depth_buffer.height = 4;
	depth_buffer.depths = depthArray;

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

	triangle_fill(renderBuffer, depth_buffer, FILLED,
		{ (float)p0.x, (float)p0.y, (float)p0.z },
		{ (float)p1.x, (float)p1.y, (float)p1.z },
		{ (float)p2.x, (float)p2.y, (float)p2.z }
	);

	assert(pixelArray[0] == EMPTY);	// Should NEVER get written to

	for (int i = 0; i < renderBuffer.height * renderBuffer.width; i += 1)
	{
		assert(pixelArray[i + 1] == expectedPixels[i]);
	}

	assert(pixelArray[17] == EMPTY);	// Should NEVER get written to
}

static void Run6x4FillTriangleTest(tl::Vec3<int> p0, tl::Vec3<int> p1, tl::Vec3<int> p2, uint32_t* expectedPixels)
{
	uint32_t pixelArray[26] = { EMPTY };	// define pixels as an an array of 26 uint32_t values
											// NB this array lives on the stack in the scope of the RunSoftwareRenderingTests function only.
											// The array is sized greater than the tl::RenderBuffer pixel array so it can pick up illegal memory writes to the pixel array
	float depthArray[26] = { 0.0f };
	z_buffer depth_buffer;
	depth_buffer.width = 6;
	depth_buffer.height = 4;
	depth_buffer.depths = depthArray;
	ClearPixelAndDepthArray(pixelArray, depthArray, 26);

	/**
	 * Set the tl::RenderBuffer to be a 5x5 grid of pixels (pixel ordinals 0 - 4)
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|-0
	 *	0 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|-1
	 *	1 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|-2
	 *	2 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|-3
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|-4
	 *    0   1   2   3   4   5   6
	 */
	tl::RenderBuffer renderBuffer;
	renderBuffer.height = 4;
	renderBuffer.width = 6;					// Size the buffer to 16 pixels. pixelArray is 25 pixels so the test can tell if the function ever oversteps the bounds of tl::RenderBuffer.
	renderBuffer.pixels = &pixelArray[1];	// Use the second element in pixelArray so we can tell if the zero-th element ever gets accessed.

	triangle_fill(renderBuffer, depth_buffer, FILLED,
		{ (float)p0.x, (float)p0.y, (float)p0.z },
		{ (float)p1.x, (float)p1.y, (float)p1.z },
		{ (float)p2.x, (float)p2.y, (float)p2.z }
	);

	assert(pixelArray[0] == EMPTY);	// Should NEVER get written to

	for (int i = 0; i < renderBuffer.height * renderBuffer.width; i += 1)
	{
		assert(pixelArray[i + 1] == expectedPixels[i]);
	}

	assert(pixelArray[25] == EMPTY);	// Should NEVER get written to
}

static void run_triangle_plane_coeffificent_tests() {
	plane_coeff coefficients;

	fill_triangle_plane_coeff(
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		coefficients
	);

	float z0 = plane_z_value_get(0.0f, 0.0f, coefficients);

	assert(z0 == 1.0f);
}

int main()
{
	std::cout << "Running camera tests" << std::endl;
	run_camera_tests();
	std::cout << "camera tests complete!!!" << std::endl;

	std::cout << "Running plane coefficient tests" << std::endl;
	run_triangle_plane_coeffificent_tests();
	std::cout << "Plane coefficient tests complete!!!" << std::endl;

	std::cout << "Running demo render tests" << std::endl;

	/**
	 * FLAT TOP RIGHT HAND SIDE RIGHT ANGLED TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 | O | x | x | O |
	 *	  |---|---|---|---|
	 *	1 |   | x | x | x |
	 *	  |---|---|---|---|
	 *	2 |   |   | x | x |
	 *	  |---|---|---|---|
	 *	3 |   |   |   | O |
	 *	  |---|---|---|---|
	 */
	uint32_t et1[16] = {
		FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	FILLED
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, et1);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, et1);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, et1);

	/**
	 * FLAT TOP LEFT HAND SIDE RIGHT ANGLED TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 | O | x | x | O |
	 *	  |---|---|---|---|
	 *	1 | x | x | x |   |
	 *	  |---|---|---|---|
	 *	2 | x | x |   |   |
	 *	  |---|---|---|---|
	 *	3 | O |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t et2[16] = {
		FILLED,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, et2);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, et2);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, et2);

	/**
	 * FLAT TOP TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 | O | x | x | O |
	 *	  |---|---|---|---|
	 *	1 |   | x | x | x |
	 *	  |---|---|---|---|
	 *	2 |   | x | x |   |
	 *	  |---|---|---|---|
	 *	3 |   |   | O |   |
	 *	  |---|---|---|---|
	 */
	uint32_t et3[16] = {
		FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 2, 3, 0 }, et3);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 2, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, et3);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 2, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, et3);

	/**
	 * FLAT BOTTOM RIGHT HAND SIDE RIGHT ANGLED TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   | O |
	 *	  |---|---|---|---|
	 *	1 |   |   | x | x |
	 *	  |---|---|---|---|
	 *	2 |   | x | x | x |
	 *	  |---|---|---|---|
	 *	3 | 0 | x | x | O |
	 *	  |---|---|---|---|
	 */
	uint32_t et4[16] = {
		EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	EMPTY,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	FILLED,	FILLED
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, et4);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 3, 0, 0 }, et4);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 3, 0 }, et4);

	/**
	 * FLAT BOTTOM LEFT HAND SIDE RIGHT ANGLED TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 | O |   |   |   |
	 *	  |---|---|---|---|
	 *	1 | x | x |   |   |
	 *	  |---|---|---|---|
	 *	2 | x | x | x |   |
	 *	  |---|---|---|---|
	 *	3 | O | x | x | O |
	 *	  |---|---|---|---|
	 */
	uint32_t et5[16] = {
		FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 3, 0 }, et5);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, et5);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 0, 0, 0 }, et5);

	/**
	 * FLAT BOTTOM TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   | O |   |
	 *	  |---|---|---|---|
	 *	1 |   | x | x |   |
	 *	  |---|---|---|---|
	 *	2 |   | x | x | x |
	 *	  |---|---|---|---|
	 *	3 | O | x | x | O |
	 *	  |---|---|---|---|
	 */
	uint32_t et6[16] = {
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	FILLED,	FILLED
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 3, 0 }, et6);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, et6);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 2, 0, 0 }, et6);

	/**
	 * RIGHT MAJOR TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   | O |   |
	 *	  |---|---|---|---|
	 *	1 |   | x | x |   |
	 *	  |---|---|---|---|
	 *	2 | O | x | x | x |
	 *	  |---|---|---|---|
	 *	3 |   |   | x | O |
	 *	  |---|---|---|---|
	 */
	uint32_t et7[16] = {
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	FILLED,	FILLED
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 2, 0 }, et7);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 2, 0 }, tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 3, 3, 0 }, et7);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 3, 0 }, tl::Vec3<int>{ 0, 2, 0 }, tl::Vec3<int>{ 2, 0, 0 }, et7);

	/**
	 * LEFT MAJOR TRIANGLE
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   | O |   |
	 *	  |---|---|---|---|
	 *	1 |   | x | x | x |
	 *	  |---|---|---|---|
	 *	2 |   | x | x | O |
	 *	  |---|---|---|---|
	 *	3 | O | x |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t et8[16] = {
		EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	EMPTY,	EMPTY
	};
	Run4x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 3, 2, 0 }, tl::Vec3<int>{ 0, 3, 0 }, et8);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 3, 2, 0 }, et8);
	Run4x4FillTriangleTest(tl::Vec3<int>{ 3, 2, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 2, 0, 0 }, et8);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - NARROW TALL 1
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | O |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x |   |   |   |
	 *	  |---|---|---|---|   |---|
	 *	3 |   |   | O |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft1[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest({ 0, 0, 0 }, { 3, 0, 0 }, { 2, 3, 0 }, eft1);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - NARROW TALL 2
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | O |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 | x | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   | O |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft2[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 1, 3, 0 }, eft2);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - SHORT WIDE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | x | O |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   |   | O |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft3[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 4, 0, 0 }, tl::Vec3<int>{ 2, 1, 0 }, eft3);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - SHORT WIDE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   |   | O |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft4[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 2, 2, 0 }, eft4);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - MIXED GRADIENTS
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | O | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft5[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 1, 2, 0 }, eft5);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - MIXED GRADIENTS 2
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   |   |   | x | O |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft6[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 4, 2, 0 }, eft6);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - RIGHT HAND RIGHT ANGLE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x | x |
	 *	  |---|---|---|---|---|---|
	 *	2 |   |   |   | x | x | x |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   | O |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft7[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	FILLED
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 5, 3, 0 }, eft7);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - LEFT HAND RIGHT ANGLE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	1 | x | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 | x | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 | O |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft8[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, eft8);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - SKEWED LEFT
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   | O | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 | O |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft9[24] = {
		EMPTY,	EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, eft9);

	/**
	 * NEW FLAT TOP TRIANGLE TEST - SKEWED RIGHT
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O | x | x | O |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   |   |   | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   | O |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t eft10[24] = {
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	FILLED
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 3, 0, 0 }, tl::Vec3<int>{ 5, 3, 0 }, eft10);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - NARROW TALL 1
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   | O |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x | x |   |   |
	 *	  |---|---|---|---|   |---|
	 *	3 | O | x | x | O |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb1[24] = {
		EMPTY,	EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 3, 3, 0 }, efb1);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - NARROW TALL 2
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   | O |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 | x | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 | O | x | x | O |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb2[24] = {
		EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 1, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 3, 3, 0 }, efb2);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - SHORT WIDE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   | O |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 | O | x | x | x | O |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb3[24] = {
		EMPTY,	EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 0, 1, 0 }, tl::Vec3<int>{ 4, 1, 0 }, efb3);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - SHORT WIDE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   | O |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb4[24] = {
		EMPTY,	EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 2, 0, 0 }, tl::Vec3<int>{ 0, 2, 0 }, tl::Vec3<int>{ 5, 2, 0 }, efb4);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - MIXED GRADIENTS
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   | O |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 | x | x | x | x |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb5[24] = {
		EMPTY,	FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 1, 0, 0 }, tl::Vec3<int>{ 0, 2, 0 }, tl::Vec3<int>{ 5, 2, 0 }, efb5);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - MIXED GRADIENTS 2
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   |   |   | O |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   |   | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb6[24] = {
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 4, 0, 0 }, tl::Vec3<int>{ 0, 2, 0 }, tl::Vec3<int>{ 5, 2, 0 }, efb6);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - RIGHT HAND RIGHT ANGLE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   |   |   |   | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   |   |   | x | x | x |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x | x | x | x |
	 *	  |---|---|---|---|---|---|
	 *	3 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb7[24] = {
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	FILLED,	FILLED,	FILLED,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 5, 3, 0 }, efb7);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - LEFT HAND RIGHT ANGLE
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 | x | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 | x | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	3 | O | x | x | x | x | O |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb8[24] = {
		FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	FILLED,	FILLED
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 5, 3, 0 }, efb8);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - SKEWED LEFT
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 | O |   |   |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	1 |   | x | x |   |   |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	3 |   |   | O | x | x | O |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb9[24] = {
		FILLED,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		EMPTY,	EMPTY,	FILLED,	FILLED,	FILLED,	FILLED
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 0, 0, 0 }, tl::Vec3<int>{ 2, 3, 0 }, tl::Vec3<int>{ 5, 3, 0 }, efb9);

	/**
	 * NEW FLAT BOTTOM TRIANGLE TEST - SKEWED RIGHT
	 *
	 *	    0   1   2   3   4   5
	 *	  |---|---|---|---|---|---|
	 *	0 |   |   |   |   |   | O |
	 *	  |---|---|---|---|---|---|
	 *	1 |   |   |   | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	2 |   | x | x | x | x |   |
	 *	  |---|---|---|---|---|---|
	 *	3 | O | x | x | O |   |   |
	 *	  |---|---|---|---|---|---|
	 */
	uint32_t efb10[24] = {
		EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	EMPTY,	EMPTY,	FILLED,	FILLED,	EMPTY,
		EMPTY,	FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,
		FILLED,	FILLED,	FILLED,	FILLED,	EMPTY,	EMPTY
	};
	Run6x4FillTriangleTest(tl::Vec3<int>{ 5, 0, 0 }, tl::Vec3<int>{ 0, 3, 0 }, tl::Vec3<int>{ 3, 3, 0 }, efb10);


	std::cout << "Demo render tests complete!!!" << std::endl;


	std::cout << "Running math tests" << std::endl;

	run_math_tests();

	std::cout << "math tests complete!!!" << std::endl;


	std::cout << "!!! ALL tests complete!!!" << std::endl;

	return 0;
}
