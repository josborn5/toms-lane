#include <assert.h>
#include <stdio.h>
#include "../commands.hpp"

// mock implementations of libraries used
int tl::file_interface_write(
	char* fileName,
	const tl::MemorySpace& writeBuffer
)
{
	return 0;
}



const int pixelCount = 32;
static uint32_t spriteContent[pixelCount];
static SpriteC sprite;
tl::MemorySpace test_memory;
static Grid grid = {0};

static const int EMPTY_COLOR = 0x000000;
static const int SET_COLOR = 0xFFDDEE;

static void ResetState()
{
	grid = {0};
	grid.initialize(&sprite);
	test_memory.content = spriteContent;
	test_memory.sizeInBytes = sizeof(uint32_t) * pixelCount;

	sprite.init(test_memory);

	for (int i = 0; i < pixelCount; i += 1)
	{
		spriteContent[i] = EMPTY_COLOR;
	}
}


static void FillSprite()
{
	for (unsigned int i = 0; i < sprite.pixel_count(); i += 1)
	{
		spriteContent[i] = SET_COLOR;
	}
}

static void AssertSetColorForPixel(int pixelIndex)
{
	uint32_t pixel = sprite.get_pixel_data(pixelIndex);
	printf("checking pixel %d is set\n", pixelIndex);

	assert(pixel == SET_COLOR);
}

static void AssertEmptyColorForPixel(int pixelIndex)
{
	uint32_t pixel = sprite.get_pixel_data(pixelIndex);
	printf("checking pixel %d is empty\n", pixelIndex);

	assert(pixel == EMPTY_COLOR);
}

static void execute_insert_row_operation()
{
	bool can_do = can_insert_row(*grid.sprite);
	assert(can_do == true);
	insert_row_operation operation = insert_row_operation(&grid);

	operation.execute();
}

static void InsertRowTests()
{
	printf("\n\nInsertRow tests\n================\n");
	printf("\n1x1 pixel content test\n");
	sprite.width = 1;
	sprite.height = 1;
	ResetState();
	FillSprite();

	AssertSetColorForPixel(0);
	AssertEmptyColorForPixel(1);

	grid.cursor.move_start();
	execute_insert_row_operation();

	assert(sprite.height == 2);
	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	execute_insert_row_operation();

	assert(sprite.height == 3);
	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);

	printf("\n1x1 selected index test\n");
	sprite.width = 1;
	sprite.height = 1;
	ResetState();
	grid.cursor.move_start();

	execute_insert_row_operation();

	assert(grid.cursor.index() == 0);

	execute_insert_row_operation();

	assert(grid.cursor.index() == 0);

	printf("\n2x1 pixel content test\n");
	sprite.width = 2;
	sprite.height = 1;
	ResetState();
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	execute_insert_row_operation();

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);

	printf("\n2x2 pixel content test\n");
	sprite.width = 2;
	sprite.height = 2;
	ResetState();
	FillSprite();
	grid.cursor.move_start();
	grid.cursor.move_up();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertEmptyColorForPixel(4);

	execute_insert_row_operation();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	printf("\n2x2 selected index test\n");
	sprite.width = 2;
	sprite.height = 2;
	ResetState();
	grid.cursor.move_start();
	grid.cursor.move_up(); // select the first pixel in the second row

	execute_insert_row_operation();

	assert(grid.cursor.index() == 2);

	execute_insert_row_operation();

	assert(grid.cursor.index() == 2);

	printf("\n2x2 undo test\n");
	sprite.width = 2;
	sprite.height = 2;
	ResetState();
	grid.cursor.move_start();
	FillSprite();

	insert_row_operation row_operation = insert_row_operation(&grid);

	row_operation.execute();

	assert(sprite.height == 3);
	AssertEmptyColorForPixel(0);
 	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	row_operation.undo();

	assert(sprite.height == 2);
	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
}

static void execute_insert_column_operation()
{
	bool can_do = can_insert_column(*grid.sprite);
	assert(can_do == true);

	insert_column_operation operation = insert_column_operation(&grid);
	operation.execute();
}

static void InsertColumnTests()
{
	printf("\n\nInsertColumn tests\n================\n");
	printf("\n\n1x1 pixel content test\n");
	sprite.width = 1;
	sprite.height = 1;
	ResetState();
	FillSprite();

	AssertSetColorForPixel(0);
	AssertEmptyColorForPixel(1);

	execute_insert_column_operation();

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	execute_insert_column_operation();

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);

	printf("\n\n1x1 selected index test\n");
	sprite.width = 1;
	sprite.height = 1;
	ResetState();

	grid.cursor.move_start();
	execute_insert_column_operation();

	assert(grid.cursor.index() == 0);

	execute_insert_column_operation();

	assert(grid.cursor.index() == 0);

	printf("\n\n1x2 pixel content test\n");
	sprite.width = 1;
	sprite.height = 2;
	ResetState();
	FillSprite();
	grid.cursor.move_start();
	grid.cursor.move_up();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertEmptyColorForPixel(3);

	execute_insert_column_operation();

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);
	AssertSetColorForPixel(3);

	execute_insert_column_operation();

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertEmptyColorForPixel(4);
	AssertSetColorForPixel(5);

	printf("\n\n1x2 selected index test\n");
	sprite.width = 1;
	sprite.height = 2;
	ResetState();
	grid.cursor.move_start();
	grid.cursor.move_up();

	execute_insert_column_operation();

	assert(grid.cursor.index() == 2); // second column in bottom row is now selected

	execute_insert_column_operation();

	assert(grid.cursor.index() == 3); // third column on bottom row is now selected

	printf("\n\n2x1 pixel content test\n");
	sprite.width = 2;
	sprite.height = 1;
	ResetState();
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertEmptyColorForPixel(2);

	execute_insert_column_operation();

	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);

	execute_insert_column_operation();

	AssertEmptyColorForPixel(0);
	AssertEmptyColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);

	printf("\n\n2x2 pixel content test\n");
	sprite.width = 2;
	sprite.height = 2;
	ResetState();
	FillSprite();

	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertSetColorForPixel(3);
	AssertEmptyColorForPixel(4);

	execute_insert_column_operation();

	assert(sprite.width == 3);
	assert(sprite.height == 2);
	AssertEmptyColorForPixel(0);
	AssertSetColorForPixel(1);
	AssertSetColorForPixel(2);
	AssertEmptyColorForPixel(3);
	AssertSetColorForPixel(4);
	AssertSetColorForPixel(5);

	execute_insert_column_operation();

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

	// undo
	sprite.width = 1;
	sprite.height = 2;
	ResetState();
	FillSprite();

	insert_column_operation insert_op = insert_column_operation(&grid);
	insert_op.execute();

	insert_op.undo();
	assert(sprite.width == 1);
	assert(sprite.height == 2);
	AssertSetColorForPixel(0);
	AssertSetColorForPixel(1);
}

static void arrange_3x3_for_copy_test()
{
	sprite.width = 3;
	sprite.height = 3;
	ResetState();

	for (unsigned int j = 0; j < sprite.height; j += 1)
	{
		unsigned int i = j * sprite.width;
		spriteContent[i + 0] = 0xFF0000;
		spriteContent[i + 1] = 0x00FF00;
		spriteContent[i + 2] = 0x0000FF;
	}
}

static void test_copy_pixels(Grid& test_grid, int cursor_index, int range_index)
{
	clipboard test_clipboard;

	copy_to_clipboard(*test_grid.sprite, cursor_index, range_index, test_clipboard);

	paste_pixel_data_operation paste_operation = paste_pixel_data_operation(test_grid.sprite);
	paste_from_clipboard_operation(test_grid, test_clipboard, paste_operation);
	paste_operation.execute();
}

void RunCopyTests()
{
	printf("\nRunning copy tests\n");

	// <-|-:
	// R G B
	// 0 1 2
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(0);
	test_copy_pixels(grid, 1, 2);

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
	test_copy_pixels(grid, 0, 1);

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
	test_copy_pixels(grid, 2, 1);

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
	test_copy_pixels(grid, 1, 5);

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
	test_copy_pixels(grid, 0, 4);

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
	test_copy_pixels(grid, 0, 4);

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
	test_copy_pixels(grid, 0, 4);

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

static void test_cut_pixels(Grid& test_grid, int cursor_index, int range_index)
{
	clipboard test_clipboard;

	paste_pixel_data_operation cut_operation = paste_pixel_data_operation(test_grid.sprite);
	cut_to_clipboard_operation(*test_grid.sprite, cursor_index, range_index, cut_operation, test_clipboard);
	cut_operation.execute();

	paste_pixel_data_operation paste_operation = paste_pixel_data_operation(test_grid.sprite);
	paste_from_clipboard_operation(test_grid, test_clipboard, paste_operation);
	paste_operation.execute();
}



void RunCutTests()
{
	printf("\nRunning cut tests\n");

	// <-|-:
	// R G B
	// 0 1 2
	arrange_3x3_for_copy_test();
	grid.cursor.set_index(0);
	test_cut_pixels(grid, 1, 2);

	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(2) == 0);
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
	test_cut_pixels(grid, 0, 1);

	assert(grid.sprite->get_pixel_data(0) == 0);
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
	test_cut_pixels(grid, 2, 1);

	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(2) == 0);
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
	test_cut_pixels(grid, 1, 5);

	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(2) == 0);
	assert(grid.sprite->get_pixel_data(3) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(4) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(5) == 0);
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
	test_cut_pixels(grid, 0, 4);

	assert(grid.sprite->get_pixel_data(0) == 0);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(2) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(3) == 0);
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
	test_cut_pixels(grid, 0, 4);

	assert(grid.sprite->get_pixel_data(0) == 0);
	assert(grid.sprite->get_pixel_data(1) == 0);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0);
	assert(grid.sprite->get_pixel_data(4) == 0);
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
	test_cut_pixels(grid, 0, 4);

	assert(grid.sprite->get_pixel_data(0) == 0);
	assert(grid.sprite->get_pixel_data(1) == 0);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0);
	assert(grid.sprite->get_pixel_data(4) == 0);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(6) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(7) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(8) == 0x00FF00);
	// check nothing is set outside the bounds of the sprite
	assert(spriteContent[9] == EMPTY_COLOR);
	assert(spriteContent[10] == EMPTY_COLOR);
	assert(spriteContent[11] == EMPTY_COLOR);

	// check undo
	arrange_3x3_for_copy_test();
	clipboard test_clipboard;

	paste_pixel_data_operation cut_operation = paste_pixel_data_operation(grid.sprite);
	cut_to_clipboard_operation(*grid.sprite, 0, 4, cut_operation, test_clipboard);
	cut_operation.execute();

	assert(grid.sprite->get_pixel_data(0) == 0);
	assert(grid.sprite->get_pixel_data(1) == 0);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0);
	assert(grid.sprite->get_pixel_data(4) == 0);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);

	cut_operation.undo();

	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(4) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);

	printf("\nCut tests complete\n");
}

static void run_delete_row_tests()
{
	printf("\nStarting delete row tests\n");

	sprite.width = 2;
	sprite.height = 3;
	ResetState();

	grid.sprite->set_pixel_data(0, 0xFF0000);
	grid.sprite->set_pixel_data(1, 0xFF0000);
	grid.sprite->set_pixel_data(2, 0x00FF00);
	grid.sprite->set_pixel_data(3, 0x00FF00);
	grid.sprite->set_pixel_data(4, 0x0000FF);
	grid.sprite->set_pixel_data(5, 0x0000FF);

	// delete the middle row
	grid.cursor.move_start();
	grid.cursor.move_up();
	bool can_delete = can_delete_row(*grid.sprite);
	assert(can_delete == true);
	delete_row_operation delete_operation = delete_row_operation(&grid);
	delete_operation.execute();

	assert(grid.sprite->width == 2);
	assert(grid.sprite->height == 2);
	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(2) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(3) == 0x0000FF);

	assert(grid.cursor.index() == 2); // cursor is on same column as before, on the index of the row that was deleted

	delete_operation.undo();

	assert(grid.sprite->width == 2);
	assert(grid.sprite->height == 3);
	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(2) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(3) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(4) == 0x0000FF);
	assert(grid.sprite->get_pixel_data(5) == 0x0000FF);

	// delete the top row
	grid.cursor.move_up();

	delete_row_operation delete_operation_2 = delete_row_operation(&grid);
	delete_operation_2.execute();

	assert(grid.sprite->width == 2);
	assert(grid.sprite->height == 2);
	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(2) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(3) == 0x00FF00);

	assert(grid.cursor.index() == 2);

	// Can't delete last row
	grid.sprite->height = 1;
	assert(can_delete_row(*grid.sprite) == false);

	printf("\nDelete row tests complete\n");
}

static void run_delete_column_tests()
{
	printf("\nStarting delete column tests\n");

	sprite.width = 2;
	sprite.height = 2;
	ResetState();

	grid.sprite->set_pixel_data(0, 0xFF0000);
	grid.sprite->set_pixel_data(1, 0x00FF00);
	grid.sprite->set_pixel_data(2, 0xFF0011);
	grid.sprite->set_pixel_data(3, 0x00FF11);

	grid.cursor.move_start();
	grid.cursor.move_up();

	assert(can_delete_column(*grid.sprite) == true);
	delete_column_operation op = delete_column_operation(&grid);
	op.execute();

	assert(sprite.height == 2);
	assert(sprite.width == 1);
	assert(grid.sprite->get_pixel_data(0) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(1) == 0x00FF11);

	assert(grid.cursor.index() == 1);

	// undo
	op.undo();

	assert(sprite.width == 2);
	assert(sprite.height == 2);
	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0x00FF00);
	assert(grid.sprite->get_pixel_data(2) == 0xFF0011);
	assert(grid.sprite->get_pixel_data(3) == 0x00FF11);

	// Can't delete the last column if it's the only one
	sprite.width = 1;
	sprite.height = 2;
	ResetState();
	assert(can_delete_column(*grid.sprite) == false);

	// Delete last column
	sprite.width = 2;
	sprite.height = 2;
	ResetState();

	grid.sprite->set_pixel_data(0, 0xFF0000);
	grid.sprite->set_pixel_data(1, 0x00FF00);
	grid.sprite->set_pixel_data(2, 0xFF0011);
	grid.sprite->set_pixel_data(3, 0x00FF11);

	grid.cursor.move_start();
	grid.cursor.move_up();
	grid.cursor.move_right();

	assert(grid.cursor.index() == 3);

	assert(can_delete_column(*grid.sprite) == true);
	delete_column_operation op2 = delete_column_operation(&grid);
	op2.execute();

	assert(grid.cursor.index() == 1);
	assert(sprite.width == 1);
	assert(sprite.height == 2);
	assert(grid.sprite->get_pixel_data(0) == 0xFF0000);
	assert(grid.sprite->get_pixel_data(1) == 0xFF0011);

	printf("\nDelete column tests complete!\n");
}

static void run_center_cursor_tests()
{
	sprite.width = 5;
	sprite.height = 15;
	ResetState();
	grid.cursor.move_start();

	assert(grid.cursor.index() == 0);

	grid.cursor.center();

	assert(grid.cursor.column_index() == 2);
	assert(grid.cursor.row_index() == 7);
}

int RunCommandTests()
{
	printf("\nRunning command tests\n");
	InsertRowTests();
	run_delete_row_tests();
	InsertColumnTests();
	run_delete_column_tests();
	RunCopyTests();
	RunCutTests();

	run_center_cursor_tests();
	printf("\nCommand tests complete!\n");
	return 0;
}
