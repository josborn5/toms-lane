#ifndef EDITOR_H
#define EDITOR_H

#include "../tl-application.hpp"
#include "../tl-library.hpp"

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
	int selectedRangeIndex = 0;
};

enum Mode
{
	View,
	Command,
	Visual,
	Insert,
	NoFile
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
