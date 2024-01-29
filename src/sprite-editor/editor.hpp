#ifndef EDITOR_H
#define EDITOR_H

enum EditorControl
{
	SpriteGrid,
	PaletteGrid,

	EditorControlCount
};

struct EditorState
{
	tl::SpriteC sprite;
	int selectedPixelIndex = 0;
	tl::SpriteC* palette; // Palette can change, so store a pointer to the selected one.
	int selectedPaletteIndex = 0;
	int selectedPalettePixelIndex = 0;
	char* commandBuffer;
	char* displayBuffer;
	EditorControl activeControl = SpriteGrid;
	tl::Color currentColor;
	tl::Color copiedColor;
	tl::Vec2<int> mouse = { 0, 0 };
	char* filePath;
	int windowWidth;
	int windowHeight;
};

#endif
