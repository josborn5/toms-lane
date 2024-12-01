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

int tl::bitmap_interface_get_pixel_data(const tl::bitmap& bitmap,
		int x,
		int y,
		uint32_t& pixel_data)
{
	pixel_data = 0x000000;
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
	grid.initialize(&sprite);
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

	grid.cursor.move_start();
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
	grid.cursor.move_start();

	InsertRow(grid);

	assert(grid.cursor.index() == 1);

	InsertRow(grid);

	assert(grid.cursor.index() == 2);

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
	grid.cursor.move_start();
	grid.cursor.move_right(); // last column in first row

	InsertRow(grid);

	assert(grid.cursor.index() == 3); // last column in second row

	InsertRow(grid);

	assert(grid.cursor.index() == 5); // last colun in third row

	printf("\n2x2 pixel content test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 2;
	FillSprite();
	grid.cursor.move_start();
	grid.cursor.move_up();

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
	grid.cursor.move_start();
	grid.cursor.move_up(); // select the first pixel in the second row

	InsertRow(grid);

	assert(grid.cursor.index() == 4); // selected pixel is now the first pixel on the third row

	InsertRow(grid);

	assert(grid.cursor.index() == 6); // selected pixel is now the first pixel on the fourth row
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

	InsertColumn(grid);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	InsertColumn(grid);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);

	printf("\n\n1x1 selected index test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 1;

	grid.cursor.move_start();
	InsertColumn(grid);

	assert(grid.cursor.index() == 1);

	InsertColumn(grid);

	assert(grid.cursor.index() == 2);

	printf("\n\n1x2 pixel content test\n");
	ResetState();
	sprite.width = 1;
	sprite.height = 2;
	FillSprite();
	grid.cursor.move_start();
	grid.cursor.move_up();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertEmptyColorForPixel(3);

	InsertColumn(grid);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertSetColorForPixel(3);

	InsertColumn(grid);

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
	grid.cursor.move_start();
	grid.cursor.move_up();

	InsertColumn(grid);

	assert(grid.cursor.index() == 3); // second column in bottom row is now selected

	InsertColumn(grid);

	assert(grid.cursor.index() == 5); // third column on bottom row is now selected

	printf("\n\n2x1 pixel content test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 1;
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	InsertColumn(grid);

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);

	InsertColumn(grid);

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);

	printf("\n\n2x1 selected index test\n");
	ResetState();
	sprite.width = 2;
	sprite.height = 1;
	grid.cursor.move_start();
	grid.cursor.move_right();

	InsertColumn(grid);

	assert(grid.cursor.index() == 2);

	InsertColumn(grid);

	assert(grid.cursor.index() == 3);

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

	InsertColumn(grid);

	assert(sprite.width == 3);
	assert(sprite.height == 2);
	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	InsertColumn(grid);

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
	grid.cursor.move_start();
	grid.cursor.move_up(); // first column of second row is selected

	InsertColumn(grid);

	assert(grid.cursor.index() == 4); // second column of second row is selected

	InsertColumn(grid);

	assert(grid.cursor.index() == 6); // third column of second for is selected
}

static void arrange_3x3_for_copy_test()
{
	ResetState();
	sprite.width = 3;
	sprite.height = 3;

	for (int j = 0; j < sprite.height; j += 1)
	{
		int i = j * sprite.width;
		spriteContent[i + 0] = 0xFF0000;
		spriteContent[i + 1] = 0x00FF00;
		spriteContent[i + 2] = 0x0000FF;
	}
}

void RunCopyTests()
{
	printf("\nRunning copy tests\n");

	// <-|-:
	// R G B
	// 0 1 2
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(0);
	copy_pixels(grid, 1, 2);

	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(8) == 0x0000FF);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	// |-:->
	// R G B
	// 0 1 2
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(1);
	copy_pixels(grid, 0, 1);

	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(2) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(8) == 0x0000FF);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	// <-:-|
	// R G B
	// 0 1 2
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(0);
	copy_pixels(grid, 2, 1);

	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(8) == 0x0000FF);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);


	// copying a square from high index to low index
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(0);
	copy_pixels(grid, 1, 5);

	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(4) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(8) == 0x0000FF);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	// copying a square from low index to high index
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(1);
	copy_pixels(grid, 0, 4);

	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(2) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(5) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(8) == 0x0000FF);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	// copying a square from low index to high index going out of bounds to the right
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(5);
	copy_pixels(grid, 0, 4);

	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(5) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(8) == 0xFF0000);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	// copying a square from low index to high index going out of bounds to the top
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(7);
	copy_pixels(grid, 0, 4);

	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(8) == 0x00FF00);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	printf("\nCopy tests complete\n");
}

int RunCommandTests()
{
	printf("\nRunning command tests\n");
	InsertRowTests();
	InsertColumnTests();
	RunCopyTests();
	printf("\nCommand tests complete!\n");
	return 0;
}
