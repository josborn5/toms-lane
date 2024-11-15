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
	editor_grid.selectedIndex = 0;
	assert(editor_grid.cursor.row_index() == 0);
	assert(editor_grid.cursor.column_index() == 0);
	assert(editor_grid.cursor.column_end_index() == 2);

	// bottom right
	editor_grid.selectedIndex = 1;
	assert(editor_grid.cursor.row_index() == 0);
	assert(editor_grid.cursor.column_index() == 1);
	assert(editor_grid.cursor.column_end_index() == 3);

	// top left
	editor_grid.selectedIndex = 2;
	assert(editor_grid.cursor.row_index() == 1);
	assert(editor_grid.cursor.column_index() == 0);
	assert(editor_grid.cursor.column_end_index() == 2);
}

void RunEditorTests()
{
	TwoByTwoGridTests();

	printf("Editor tests complete!\n");
}
