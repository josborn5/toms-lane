#include "../software-rendering.hpp"
#include "../assert.hpp"
#include <iostream>
#include <stdio.h>

static void TestSprite(char* inputContent, int expectedHeight, int expectedWidth)
{
	std::cout << "Sprite tests" << std::endl;
	
	tl::Sprite testSprite = tl::LoadSprite(inputContent);

	std::cout << "\nSprite test" << std::endl;
	std::cout << "Input content: '" << inputContent << "'" << std::endl;
	std::cout << "Expected height: " << expectedHeight << std::endl;
	std::cout << "Actual height: " << testSprite.height << std::endl;
	std::cout << "Actual width: " << testSprite.width << std::endl;
	std::cout << "Expected width: " << expectedWidth << std::endl;
	assert(inputContent == testSprite.content);
	assert(expectedHeight == testSprite.height);
	assert(expectedWidth == testSprite.width);
}

static void RunSpriteRenderTests()
{
	tl::RenderBuffer buffer;
	uint32_t* frame_buffer_memory = (uint32_t*)malloc(sizeof(uint32_t) * 5 * 4);
	buffer.init(frame_buffer_memory, 5, 4, tl::frame_buffer_origin_bottom_left);

	buffer.fill(0x000000);

	tl::Sprite testSprite;
	testSprite.width = 4;
	testSprite.height = 3;
	testSprite.content = "0\n  0000000000\n00\n00\n\n";

	tl::Rect<float> footprint;
	footprint.halfSize = { 0.5f * (float)testSprite.width, 0.5f * (float)testSprite.height };
	footprint.position = footprint.halfSize;

	DrawSprite(buffer, testSprite, footprint, 0xFFFFFF);

	assert_uint32_t(buffer.get_pixel(0), 0xFFFFFF, "pixel index 0 color");
	assert_uint32_t(buffer.get_pixel(1), 0xFFFFFF, "pixel index 1 color");
	assert_uint32_t(buffer.get_pixel(2), 0x000000, "pixel index 2 color");
	assert_uint32_t(buffer.get_pixel(3), 0x000000, "pixel index 3 color");
	assert_uint32_t(buffer.get_pixel(4), 0x000000, "pixel index 4 color");

	assert_uint32_t(buffer.get_pixel(5), 0x000000, "pixel index 5 color");
	assert_uint32_t(buffer.get_pixel(6), 0x000000, "pixel index 6 color");
	assert_uint32_t(buffer.get_pixel(7), 0xFFFFFF, "pixel index 7 color");
	assert_uint32_t(buffer.get_pixel(8), 0xFFFFFF, "pixel index 8 color");
	assert_uint32_t(buffer.get_pixel(9), 0x000000, "pixel index 9 color");

	assert_uint32_t(buffer.get_pixel(10), 0xFFFFFF, "pixel index 10 color");
	assert_uint32_t(buffer.get_pixel(11), 0x000000, "pixel index 11 color");
	assert_uint32_t(buffer.get_pixel(12), 0x000000, "pixel index 12 color");
	assert_uint32_t(buffer.get_pixel(13), 0x000000, "pixel index 13 color");
	assert_uint32_t(buffer.get_pixel(14), 0x000000, "pixel index 14 color");

	buffer.fill(0x000000);

	footprint.position.y = (float)buffer.height - footprint.halfSize.y;

	DrawSprite(buffer, testSprite, footprint, 0xFFFFFF);

	assert_uint32_t(buffer.get_pixel(0), 0x000000, "pixel index 0 color");
	assert_uint32_t(buffer.get_pixel(1), 0x000000, "pixel index 1 color");
	assert_uint32_t(buffer.get_pixel(2), 0x000000, "pixel index 2 color");
	assert_uint32_t(buffer.get_pixel(3), 0x000000, "pixel index 3 color");
	assert_uint32_t(buffer.get_pixel(4), 0x000000, "pixel index 4 color");

	assert_uint32_t(buffer.get_pixel(5), 0xFFFFFF, "pixel index 5 color");
	assert_uint32_t(buffer.get_pixel(6), 0xFFFFFF, "pixel index 6 color");
	assert_uint32_t(buffer.get_pixel(7), 0x000000, "pixel index 7 color");
	assert_uint32_t(buffer.get_pixel(8), 0x000000, "pixel index 8 color");
	assert_uint32_t(buffer.get_pixel(9), 0x000000, "pixel index 9 color");

	assert_uint32_t(buffer.get_pixel(10), 0x000000, "pixel index 10 color");
	assert_uint32_t(buffer.get_pixel(11), 0x000000, "pixel index 11 color");
	assert_uint32_t(buffer.get_pixel(12), 0xFFFFFF, "pixel index 12 color");
	assert_uint32_t(buffer.get_pixel(13), 0xFFFFFF, "pixel index 13 color");
	assert_uint32_t(buffer.get_pixel(14), 0x000000, "pixel index 14 color");

	assert_uint32_t(buffer.get_pixel(15), 0xFFFFFF, "pixel index 15 color");
	assert_uint32_t(buffer.get_pixel(16), 0x000000, "pixel index 16 color");
	assert_uint32_t(buffer.get_pixel(17), 0x000000, "pixel index 17 color");
	assert_uint32_t(buffer.get_pixel(18), 0x000000, "pixel index 18 color");
	assert_uint32_t(buffer.get_pixel(19), 0x000000, "pixel index 19 color");
}

void RunSpriteTests()
{
	printf("Start RunSpriteTests...\n");
	// Sprites

	char* inputSprite = "\
0\n\
0\n\
0";
	TestSprite(inputSprite, 3, 1);

	inputSprite = "0000";
	TestSprite(inputSprite, 1, 4);

	inputSprite = "\
   0\n\
  0\n\
 0\n\
0";
	TestSprite(inputSprite, 4, 4);

	inputSprite = "\
0\n\
0\n\
0\n\
0\n\
0\n\
0\n\
0000";
	TestSprite(inputSprite, 7, 4);

	inputSprite = "\
 \n\
 \n\
 \n\
0  0\n\
 \n\
 \n\
 \n\
                        ";
	TestSprite(inputSprite, 8, 24);

	RunSpriteRenderTests();

	printf("... RunSpriteTests complete!\n");
}
