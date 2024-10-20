#include <assert.h>
#include <stdio.h>
#include "./commands.hpp"

// mock implementations of libraries used
uint32_t tl::GetColorFromRGB(int r, int g, int b)
{
	return 0x000000;
}

int tl::file_interface_write(
	char* fileName,
	const tl::MemorySpace& writeBuffer
)
{
	return 0;
}

int tl::IntToCharString(int a, char* b)
{
	return 0;
}

int tl::bitmap_interface_write(
	const tl::bitmap& bitmap,
	const tl::MemorySpace& memory)
{
	return 0;
}

tl::MemorySpace tl::CarveMemorySpace(uint64_t carveSizeInBytes, tl::MemorySpace& toCarve)
{
	return toCarve;
}

int tl::CharStringToInt(char* string)
{
	return 0;
}

char* tl::CopyToEndOfNumberChar(char* one, char* two)
{
	return "";
}

char* tl::GetNextNumberChar(char* string)
{
	return "";
}

int tl::bitmap_interface_get_color(const tl::bitmap& bitmap,
		int x,
		int y,
		uint32_t& output_color)
{
	output_color = 0x000000;
	return 0;
}



const int pixelCount = 32;
static uint32_t spriteContent[pixelCount];
static SpriteC sprite;
tl::MemorySpace memory;
static Grid grid = {0};

static const int EMPTY_COLOR = 0x000000;
static const int SET_COLOR = 0xFFDDEE;

static void ResetState()
{
	grid = {0};
	grid.sprite = &sprite;
	memory.content = spriteContent;
	memory.sizeInBytes = sizeof(uint32_t) * pixelCount;

	sprite.pixel_memory = memory;

	for (int i = 0; i < pixelCount; i += 1)
	{
		spriteContent[i] = EMPTY_COLOR;
	}
}


static void FillSprite()
{
	int count = sprite.width * sprite.height;
	for (int i = 0; i < count; i += 1)
	{
		spriteContent[i] = SET_COLOR;
	}
}

static void AssertSetColorForPixel(int pixelIndex)
{
	uint32_t pixel = sprite.pixels()[pixelIndex];
	printf("checking pixel %d is set\n", pixelIndex);

	assert(pixel == SET_COLOR);
}

static void AssertEmptyColorForPixel(int pixelIndex)
{
	uint32_t pixel = sprite.pixels()[pixelIndex];
	printf("checking pixel %d is empty\n", pixelIndex);

	assert(pixel == EMPTY_COLOR);
}

static void InsertRowTests()
{
	printf("\n\nInsertRow tests\n================\n");
	printf("\n1x1 pixel content test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 1;
	FillSprite();

	AssertSetColorForPixel(0);
	AssertEmptyColorForPixel(1);

	InsertRow(grid);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	InsertRow(grid);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);

	printf("\n1x1 selected index test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 1;
	grid.selectedIndex = 0;

	InsertRow(grid);

	assert(grid.selectedIndex == 1);

	InsertRow(grid);

	assert(grid.selectedIndex == 2);

	printf("\n2x1 pixel content test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 1;
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	InsertRow(grid);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);

	printf("\n2x1 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 1;
	grid.selectedIndex = 1; // last column in first row

	InsertRow(grid);

	assert(grid.selectedIndex == 3); // last column in second row

	InsertRow(grid);

	assert(grid.selectedIndex == 5); // last colun in third row

	printf("\n2x2 pixel content test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 2;
	FillSprite();
	grid.selectedIndex = 2;

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertEmptyColorForPixel(4);

	InsertRow(grid);

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	printf("\n2x2 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 2;
	grid.selectedIndex = 2; // select the first pixel in the second row

	InsertRow(grid);

	assert(grid.selectedIndex == 4); // selected pixel is now the first pixel on the third row

	InsertRow(grid);

	assert(grid.selectedIndex == 6); // selected pixel is now the first pixel on the fourth row

	printf("\n8x2 selected index test\n");
	ResetState();
	sprite.width = 8;
	sprite.height = 2;
	grid.selectedIndex = 8; // select the first pixel in the second row

	InsertRow(grid);

	assert(grid.selectedIndex == 16); // selected pixel is now the first pixel on the third row

	InsertRow(grid);

	assert(grid.selectedIndex == 24); // selected pixel is now the first pixel on the fourth row

	printf("\n2x8 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 8;
	grid.selectedIndex = 3; // select the last pixel in the second row

	InsertRow(grid);

	assert(grid.selectedIndex == 5); // selected pixel is now the last pixel on the third row

	InsertRow(grid);

	assert(grid.selectedIndex == 7); // selected pixel is now the last pixel on the fourth row
}

static void InsertColumnTests()
{
	printf("\n\nInsertColumn tests\n================\n");
	printf("\n\n1x1 pixel content test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 1;
	FillSprite();

	AssertSetColorForPixel(0);
	AssertEmptyColorForPixel(1);

	InsertColumn(grid, memory);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	InsertColumn(grid, memory);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);

	printf("\n\n1x1 selected index test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 1;

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 1);

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 2);

	printf("\n\n1x2 pixel content test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 2;
	FillSprite();
	grid.selectedIndex = 1; // select first column in bottom row

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertEmptyColorForPixel(3);

	InsertColumn(grid, memory);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertSetColorForPixel(3);

	InsertColumn(grid, memory);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertEmptyColorForPixel(4);
	AssertSetColorForPixel(5);

	printf("\n\n1x2 selected index test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 2;
	grid.selectedIndex = 1; // select first column in bottom row

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 3); // second column in bottom row is now selected

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 5); // third column on bottom row is now selected

	printf("\n\n2x1 pixel content test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 1;
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	InsertColumn(grid, memory);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);

	InsertColumn(grid, memory);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);

	printf("\n\n2x1 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 1;
	grid.selectedIndex = 1;

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 2);

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 3);

	printf("\n\n2x2 pixel content test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 2;
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertEmptyColorForPixel(4);

	InsertColumn(grid, memory);

	assert(sprite.width == 3);
	assert(sprite.height == 2);
	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	InsertColumn(grid, memory);

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

	printf("\n\n2x2 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 2;
	grid.selectedIndex = 2; // first column of second row is selected

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 4); // second column of second row is selected

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 6); // third column of second for is selected

	printf("\n8x2 selected index test\n");
	ResetState();
	sprite.width = 8;
	sprite.height = 2;
	grid.selectedIndex = 8; // select the first pixel in the second row

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 10); // selected pixel is now the second pixel on the second row

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 12); // selected pixel is now the third pixel on the second row

	printf("\n2x8 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 8;
	grid.selectedIndex = 15; // select the last pixel in the last row

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 23); // selected pixel is now the last pixel on the last row

	InsertColumn(grid, memory);

	assert(grid.selectedIndex == 31); // selected pixel is now the last pixel on the last row
}

int main()
{

	InsertRowTests();
	InsertColumnTests();

	printf("All tests complete!!!\n");
	return 0;
}
