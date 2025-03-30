#include "./editor.hpp"
#include <stdio.h>
#include <assert.h>

void TwoByTwoGridTests()
{
	SpriteC editor_sprite;
	editor_sprite.height = 2;
	editor_sprite.width = 2;

	Grid editor_grid;
	editor_grid.initialize(&editor_sprite);

	// bottom left
	editor_grid.cursor.move_start();
	assert(editor_grid.cursor.row_index() == 0);
	assert(editor_grid.cursor.column_index() == 0);
	assert(editor_grid.cursor.column_end_index() == 2);

	// bottom right
	editor_grid.cursor.move_right();
	editor_grid.cursor.move_right();
	assert(editor_grid.cursor.row_index() == 0);
	assert(editor_grid.cursor.column_index() == 1);
	assert(editor_grid.cursor.column_end_index() == 3);

	// top left
	editor_grid.cursor.move_left();
	editor_grid.cursor.move_left();
	editor_grid.cursor.move_up();
	editor_grid.cursor.move_up();
	assert(editor_grid.cursor.row_index() == 1);
	assert(editor_grid.cursor.column_index() == 0);
	assert(editor_grid.cursor.column_end_index() == 2);

	// top right
	editor_grid.cursor.move_right();
	assert(editor_grid.cursor.row_index() == 1);
	assert(editor_grid.cursor.column_index() == 1);
	assert(editor_grid.cursor.column_end_index() == 3);

	// bottom right
	editor_grid.cursor.move_down();
	editor_grid.cursor.move_down();
	assert(editor_grid.cursor.row_index() == 0);
	assert(editor_grid.cursor.column_index() == 1);
	assert(editor_grid.cursor.column_end_index() == 3);

	// top left
	editor_grid.cursor.move_end();
	editor_grid.cursor.move_left();
	assert(editor_grid.cursor.row_index() == 1);
	assert(editor_grid.cursor.column_index() == 0);
	assert(editor_grid.cursor.column_end_index() == 2);

	editor_grid.cursor.move_start();
	editor_grid.cursor.move_row_end();
	assert(editor_grid.cursor.index() == 1);
	editor_grid.cursor.move_row_start();
	assert(editor_grid.cursor.index() == 0);
}

static void five_by_five_grid_tests()
{
	SpriteC editor_sprite;
	editor_sprite.height = 5;
	editor_sprite.width = 5;

	Grid editor_grid;
	editor_grid.initialize(&editor_sprite);

	editor_grid.camera_focus.center();

	assert(editor_grid.camera_focus.index() == 12);
	assert(editor_grid.camera_focus.bottom_row_index() == 0);
	assert(editor_grid.camera_focus.top_row_index() == 4);
	assert(editor_grid.camera_focus.left_column_index() == 0);
	assert(editor_grid.camera_focus.right_column_index() == 4);

	editor_grid.camera_focus.move_up();
	editor_grid.camera_focus.move_up();
	editor_grid.camera_focus.move_up();

	assert(editor_grid.camera_focus.index() == 22);
	assert(editor_grid.camera_focus.bottom_row_index() == 2);
	assert(editor_grid.camera_focus.top_row_index() == 4);

	editor_grid.camera_focus.center();
	editor_grid.camera_focus.move_down();
	editor_grid.camera_focus.move_down();
	editor_grid.camera_focus.move_down();

	assert(editor_grid.camera_focus.index() == 2);
	assert(editor_grid.camera_focus.bottom_row_index() == 0);
	assert(editor_grid.camera_focus.top_row_index() == 2);

	editor_grid.camera_focus.center();
	editor_grid.camera_focus.move_left();
	editor_grid.camera_focus.move_left();
	editor_grid.camera_focus.move_left();

	assert(editor_grid.camera_focus.index() == 10);
	assert(editor_grid.camera_focus.bottom_row_index() == 0);
	assert(editor_grid.camera_focus.top_row_index() == 4);
	assert(editor_grid.camera_focus.left_column_index() == 0);

	editor_grid.camera_focus.center();
	editor_grid.camera_focus.move_right();
	editor_grid.camera_focus.move_right();
	editor_grid.camera_focus.move_right();

	assert(editor_grid.camera_focus.index() == 14);
	assert(editor_grid.camera_focus.bottom_row_index() == 0);
	assert(editor_grid.camera_focus.top_row_index() == 4);
	assert(editor_grid.camera_focus.left_column_index() == 2);

	editor_sprite.height = 100;
	editor_sprite.width = 100;

	editor_grid.camera_focus.center();
	editor_grid.camera_focus.zoom_in();

	assert(editor_grid.camera_focus.index() == 5050);
	assert(editor_grid.camera_focus.bottom_row_index() == 12);
	assert(editor_grid.camera_focus.top_row_index() == 87);
	assert(editor_grid.camera_focus.left_column_index() == 12);
	assert(editor_grid.camera_focus.right_column_index() == 87);
}

static void run_sprite_copy_tests()
{
	SpriteC source_sprite, target_sprite;
	const int ARRAY_SIZE = 8;
	uint32_t source_memory[ARRAY_SIZE];
	uint32_t target_memory[ARRAY_SIZE];
	tl::MemorySpace source_space, target_space;
	source_space.content = &source_memory[0];
	source_space.sizeInBytes = ARRAY_SIZE * sizeof(uint32_t);
	target_space.content = &target_memory[0];
	target_space.sizeInBytes = ARRAY_SIZE * sizeof(uint32_t);

	source_sprite.height = 2;
	source_sprite.width = 8;
	source_sprite.init(source_space);

	assert(source_sprite.height != target_sprite.height);
	assert(source_sprite.width != target_sprite.width);

	target_sprite.copy_from(source_sprite);

	assert(source_sprite.height == target_sprite.height);
	assert(source_sprite.width == target_sprite.width);
}

void RunEditorTests()
{
	TwoByTwoGridTests();
	five_by_five_grid_tests();
	run_sprite_copy_tests();

	printf("Editor tests complete!\n");
}
