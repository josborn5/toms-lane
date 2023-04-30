#ifndef EDITOR_H
#define EDITOR_H

struct EditorState
{
	tl::SpriteC sprite;
	int selectedPixelIndex = 0;
	tl::SpriteC palette;
	int selectedPaletteIndex = 0;
	char* commandBuffer;
	char* displayBuffer;
};

#endif