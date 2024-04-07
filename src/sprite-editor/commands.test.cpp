#include <assert.h>
#include <stdio.h>
#include "./commands.hpp"

// mock implementations of libraries used
int tl::file_interface_write(
	char* fileName,
	const MemorySpace& writeBuffer
)
{
	return 0;
}

int tl::IntToCharString(int a, char* b)
{
	return 0;
}

const int pixelCount = 8;
static tl::Color spriteContent[pixelCount];
static tl::SpriteC sprite;
tl::MemorySpace memory;

static void ResetState()
{
	sprite.content = spriteContent;
	memory.content = spriteContent;
	memory.sizeInBytes = sizeof(tl::Color) * pixelCount;

	for (int i = 0; i < pixelCount; i += 1)
	{
		spriteContent[i].r = 0.0f;
		spriteContent[i].g = 0.0f;
		spriteContent[i].b = 0.0f;
		spriteContent[i].a = 0.0f;
	}
}

// tests
static void SetColor(tl::Color& color)
{
	color.r = 255.0f;
	color.g = 254.0f;
	color.b = 253.0f;
	color.a = 252.0f;
}

static void AssertSetColorForPixel(int pixelIndex)
{
	tl::Color pixel = sprite.content[pixelIndex];
	printf("checking pixel %d is set\n", pixelIndex);

	assert(pixel.r == 255.0f);
	assert(pixel.g == 254.0f);
	assert(pixel.b == 253.0f);
	assert(pixel.a == 252.0f);
}

static void AssertEmptyColorForPixel(int pixelIndex)
{
	tl::Color pixel = sprite.content[pixelIndex];
	printf("checking pixel %d is empty\n", pixelIndex);

	assert(pixel.r == 0.0f);
	assert(pixel.g == 0.0f);
	assert(pixel.b == 0.0f);
	assert(pixel.a == 0.0f);
}


static void AddColumnTests()
{
	// 1x1
	printf("\n\n1x1 test\n");
	ResetState();
	SetColor(spriteContent[0]);
	sprite.width = 1;
	sprite.height = 1;

	AssertSetColorForPixel(0);
	AssertEmptyColorForPixel(1);

	AppendColumnToSpriteC(sprite, memory);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);

	AppendColumnToSpriteC(sprite, memory);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);

	// 1x2
	printf("\n\n1x2 test\n");
	ResetState();
	SetColor(spriteContent[0]);
	SetColor(spriteContent[1]);
	sprite.width = 1;
	sprite.height = 2;

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertEmptyColorForPixel(3);

	AppendColumnToSpriteC(sprite, memory);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertSetColorForPixel(3);

	AppendColumnToSpriteC(sprite, memory);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertEmptyColorForPixel(4);
	AssertSetColorForPixel(5);

	// 2x1
	printf("\n\n2x1 test\n");
	ResetState();
	SetColor(spriteContent[0]);
	SetColor(spriteContent[1]);
	sprite.width = 2;
	sprite.height = 1;

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	AppendColumnToSpriteC(sprite, memory);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);

	AppendColumnToSpriteC(sprite, memory);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);

	// 2x2
	printf("\n\n2x2 test\n");
	ResetState();
	SetColor(spriteContent[0]);
	SetColor(spriteContent[1]);
	SetColor(spriteContent[2]);
	SetColor(spriteContent[3]);
	sprite.width = 2;
	sprite.height = 2;

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertEmptyColorForPixel(4);

	AppendColumnToSpriteC(sprite, memory);

	assert(sprite.width == 3);
	assert(sprite.height == 2);
	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	AppendColumnToSpriteC(sprite, memory);

	assert(sprite.width == 4);
	assert(sprite.height == 2);
	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertEmptyColorForPixel(4);
	AssertEmptyColorForPixel(5);
	AssertSetColorForPixel(6);
	AssertSetColorForPixel(7);

	printf("All tests complete!!!\n");
}

int main()
{
	AddColumnTests();
	return 0;
}
