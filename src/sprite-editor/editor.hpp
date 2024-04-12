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

struct EditorState
{
	Grid pixels;
	Grid palette_;
	tl::SpriteC* palette; // Palette can change, so store a pointer to the selected one.
	char* commandBuffer;
	char* displayBuffer;
	EditorControl activeControl = SpriteGrid;
	int windowWidth;
	int windowHeight;
};

#endif
