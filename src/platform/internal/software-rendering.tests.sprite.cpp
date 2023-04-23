#include "./software-rendering.hpp"
#include <assert.h>
#include <iostream>

void TestSprite(char* inputContent, int expectedHeight, int expectedWidth)
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

	// ParseColorFromCharArray
	printf("\n===== ParseColorFromCharArray =====\n");
	tl::MemorySpace tempSpace;
	tempSpace.content = malloc(8);
	tl::Color testColor;
	ParseColorFromCharArray(" 255 255 255 255 ", tempSpace, testColor);
	assert(testColor.r == 1.0f);
	assert(testColor.g == 1.0f);
	assert(testColor.b == 1.0f);
	assert(testColor.a == 1.0f);

	ParseColorFromCharArray("", tempSpace, testColor);
	assert(testColor.r == 0.0f);
	assert(testColor.g == 0.0f);
	assert(testColor.b == 0.0f);
	assert(testColor.a == 0.0f);

	free(tempSpace.content);


	// Load SpriteC
	printf("\n===== LoadSpriteC =====\n");
	tl::MemorySpace space;
	space.content = malloc(8);
	tl::SpriteC testSprite;
	testSprite.content = (tl::Color *)malloc(1024);
	tl::LoadSpriteC("2\n1\n255 102 51 255\n204 102 51 153", space, testSprite);
	printf("\nactual SpriteC width: %d\nactual SpriteC height: %d\n", testSprite.width, testSprite.height);
	assert(testSprite.width == 2);
	assert(testSprite.height == 1);
	assert(testSprite.content[0].r == 1.0f);
	assert(testSprite.content[0].g == 0.4f);
	assert(testSprite.content[0].b == 0.2f);
	assert(testSprite.content[0].a == 1.0f);

	assert(testSprite.content[1].r == 0.8f);
	assert(testSprite.content[1].g == 0.4f);
	assert(testSprite.content[1].b == 0.2f);
	assert(testSprite.content[1].a == 0.6f);

	tl::LoadSpriteC("\
3\n\
2\n\
255 102 51 255\n\
204 102 51 153\n\
255 0 0 255\n\
0 255 0 255\n\
0 0 255 255\n\
255 255 255 255\n\
", space, testSprite);
	printf("\nactual SpriteC width: %d\nactual SpriteC height: %d\n", testSprite.width, testSprite.height);
	assert(testSprite.width == 3);
	assert(testSprite.height == 2);
	assert(testSprite.content[0].r == 1.0f);
	assert(testSprite.content[0].g == 0.4f);
	assert(testSprite.content[0].b == 0.2f);
	assert(testSprite.content[0].a == 1.0f);

	assert(testSprite.content[1].r == 0.8f);
	assert(testSprite.content[1].g == 0.4f);
	assert(testSprite.content[1].b == 0.2f);
	assert(testSprite.content[1].a == 0.6f);

	assert(testSprite.content[2].r == 1.0f);
	assert(testSprite.content[2].g == 0.0f);
	assert(testSprite.content[2].b == 0.0f);
	assert(testSprite.content[2].a == 1.0f);

	assert(testSprite.content[3].r == 0.0f);
	assert(testSprite.content[3].g == 1.0f);
	assert(testSprite.content[3].b == 0.0f);
	assert(testSprite.content[3].a == 1.0f);

	assert(testSprite.content[4].r == 0.0f);
	assert(testSprite.content[4].g == 0.0f);
	assert(testSprite.content[4].b == 1.0f);
	assert(testSprite.content[4].a == 1.0f);

	assert(testSprite.content[5].r == 1.0f);
	assert(testSprite.content[5].g == 1.0f);
	assert(testSprite.content[5].b == 1.0f);
	assert(testSprite.content[5].a == 1.0f);

	free(space.content);
	free(testSprite.content);
}
