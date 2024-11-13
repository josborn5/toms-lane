#include "./editor.hpp"

void TwoByTwoGridTests()
{
	SpriteC sprite;
	sprite.height = 2;
	sprite.width = 2;

	Grid grid;
	grid.sprite = &sprite;
}

void RunEditorTests()
{
	TwoByTwoGridTests();
}
