#ifndef EDITOR_H
#define EDITOR_H

enum EditorControl
{
	SpriteGrid,
	PaletteGrid,

	EditorControlCount
};

struct Grid
{
	tl::SpriteC* sprite;
	tl::Rect<float> footprint;
	tl::Rect<float> container;
	int selectedIndex = 0;
};

enum Mode
{
	View,
	Command,
	Visual,
	Insert
};

struct EditorState
{
	Grid pixels;
	Grid palette_;
	char* commandBuffer;
	EditorControl activeControl = SpriteGrid;
	int windowWidth;
	int windowHeight;
	Mode mode;
};

#endif
