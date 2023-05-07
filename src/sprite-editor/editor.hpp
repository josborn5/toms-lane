#ifndef EDITOR_H
#define EDITOR_H

struct EditorState
{
	tl::SpriteC sprite;
	int selectedPixelIndex = 0;
	tl::SpriteC* palette; // Palette can change, so store a pointer to the selected one.
	int selectedPaletteIndex = 0;
	char* commandBuffer;
	char* displayBuffer;
};

#endif