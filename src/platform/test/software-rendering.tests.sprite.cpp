#include "../software-rendering.hpp"
#include <assert.h>
#include <iostream>

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

static void ClearBuffer(tl::RenderBuffer& buffer)
{
	int pixelCount = buffer.width * buffer.height;
	for (int i = 0; i < pixelCount; i += 1)
	{
		buffer.pixels[i] = 0x000000;
	}
}

static void RunSpriteRenderTests()
{
	tl::RenderBuffer buffer;
	buffer.width = 5;
	buffer.height = 4;
	int pixelCount = buffer.width * buffer.height;
	buffer.pixels = (uint32_t*)malloc(sizeof(uint32_t) * pixelCount);

	ClearBuffer(buffer);

	tl::Sprite testSprite;
	testSprite.width = 4;
	testSprite.height = 3;
	testSprite.content = "0\n  0000000000\n00\n00\n\n";

	tl::Rect<float> footprint;
	footprint.halfSize = { 0.5f * (float)testSprite.width, 0.5f * (float)testSprite.height };
	footprint.position = footprint.halfSize;

	DrawSprite(buffer, testSprite, footprint, 0xFFFFFF);

	assert(*buffer.pixels == 0xFFFFFF);
	assert(*(buffer.pixels + 1) == 0xFFFFFF);
	assert(*(buffer.pixels + 2) == 0x000000);
	assert(*(buffer.pixels + 3) == 0x000000);
	assert(*(buffer.pixels + 4) == 0x000000);

	assert(*(buffer.pixels + 5) == 0x000000);
	assert(*(buffer.pixels + 6) == 0x000000);
	assert(*(buffer.pixels + 7) == 0xFFFFFF);
	assert(*(buffer.pixels + 8) == 0xFFFFFF);
	assert(*(buffer.pixels + 9) == 0x000000);

	assert(*(buffer.pixels + 10) == 0xFFFFFF);
	assert(*(buffer.pixels + 11) == 0x000000);
	assert(*(buffer.pixels + 12) == 0x000000);
	assert(*(buffer.pixels + 13) == 0x000000);
	assert(*(buffer.pixels + 14) == 0x000000);

	ClearBuffer(buffer);

	footprint.position.y = (float)buffer.height - footprint.halfSize.y;

	DrawSprite(buffer, testSprite, footprint, 0xFFFFFF);

	assert(*buffer.pixels == 0x000000);
	assert(*(buffer.pixels + 1) == 0x000000);
	assert(*(buffer.pixels + 2) == 0x000000);
	assert(*(buffer.pixels + 3) == 0x000000);
	assert(*(buffer.pixels + 4) == 0x000000);

	assert(*(buffer.pixels + 5) == 0xFFFFFF);
	assert(*(buffer.pixels + 6) == 0xFFFFFF);
	assert(*(buffer.pixels + 7) == 0x000000);
	assert(*(buffer.pixels + 8) == 0x000000);
	assert(*(buffer.pixels + 9) == 0x000000);

	assert(*(buffer.pixels + 10) == 0x000000);
	assert(*(buffer.pixels + 11) == 0x000000);
	assert(*(buffer.pixels + 12) == 0xFFFFFF);
	assert(*(buffer.pixels + 13) == 0xFFFFFF);
	assert(*(buffer.pixels + 14) == 0x000000);

	assert(*(buffer.pixels + 15) == 0xFFFFFF);
	assert(*(buffer.pixels + 16) == 0x000000);
	assert(*(buffer.pixels + 17) == 0x000000);
	assert(*(buffer.pixels + 18) == 0x000000);
	assert(*(buffer.pixels + 19) == 0x000000);
}

void RunSpriteTests()
{
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
}
