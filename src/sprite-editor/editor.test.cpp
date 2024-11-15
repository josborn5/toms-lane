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
	editor_grid.selectedIndex = 1;
	editor_grid.cursor.move_left();
	editor_grid.cursor.move_left();
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
}

void RunEditorTests()
{
	TwoByTwoGridTests();

	printf("Editor tests complete!\n");
}
