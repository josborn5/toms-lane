#ifndef EDITOR_H
#define EDITOR_H

#include "../tl-application.hpp"
#include "../tl-library.hpp"

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;
};

struct SpriteC
{
	int width;
	int height;
	Color* content;
};

enum EditorControl
{
	SpriteGrid,
	PaletteGrid,

	EditorControlCount
};

struct Grid
{
	SpriteC* sprite;
	tl::Rect<float> footprint;
	tl::Rect<float> container;
	tl::Rect<float> camera;
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
