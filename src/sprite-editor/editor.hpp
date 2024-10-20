#ifndef EDITOR_H
#define EDITOR_H

#include "../tl-application.hpp"
#include "../tl-library.hpp"


struct SpriteC
{
	int width;
	int height;
	tl::MemorySpace pixel_memory = {0};
	uint16_t bitsPerPixel = 24;

	uint32_t* pixels() const
	{
		return (uint32_t*)pixel_memory.content;
	}
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

	uint32_t selected_color() const
	{
		return sprite->pixels()[selectedIndex];
	}

	int row_index(int index) const
	{
		if (sprite->height == 1)
		{
			return 0;
		}

		return index / sprite->width;
	}

	int selected_row_index() const
	{
		return row_index(selectedIndex);
	}

	int column_index(int index) const
	{
		if (sprite->width == 1)
		{
			return 0;
		}

		return index % sprite->width;
	}

	int selected_column_index() const
	{
		return column_index(selectedIndex);
	}
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
