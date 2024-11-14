#include "./editor.hpp"
#include <stdio.h>

void TwoByTwoGridTests()
{
	SpriteC editor_sprite;
	editor_sprite.height = 2;
	editor_sprite.width = 2;

	Grid editor_grid;
	editor_grid.sprite = &editor_sprite;
}

void RunEditorTests()
{
	TwoByTwoGridTests();
}
