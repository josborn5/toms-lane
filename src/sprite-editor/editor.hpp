#ifndef EDITOR_H
#define EDITOR_H

enum EditorControl
{
	SpriteGrid,
	PaletteGrid,
	CommandConsole,

	EditorControlCount
};

struct EditorState
{
	tl::SpriteC sprite;
	int selectedPixelIndex = 0;
	tl::SpriteC* palette; // Palette can change, so store a pointer to the selected one.
	int selectedPalettePixelIndex = 0;
	char* commandBuffer;
	char* displayBuffer;
	EditorControl activeControl = SpriteGrid;
	tl::Vec2<int> mouse = { 0, 0 };
	int windowWidth;
	int windowHeight;
};

#endif
