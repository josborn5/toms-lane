#ifndef EDITOR_H
#define EDITOR_H

#include "../tl-application.hpp"
#include "../tl-library.hpp"

struct SpriteC
{
	int width = 0;
	int height = 0;
	uint16_t bitsPerPixel = 24;

	tl::MemorySpace pixel_memory = {0};

	SpriteC* p_color_table = nullptr;

	int pixel_count() const
	{
		return width * height;
	}

	uint32_t* pixels() const
	{
		return (uint32_t*)pixel_memory.content;
	}

	int color_table_length() const
	{
		return (p_color_table == nullptr) ? 0 : p_color_table->pixel_count();
	}

	bool has_color_table() const
	{
		return color_table_length() > 0;
	}

	uint32_t get_pixel_data(int index) const
	{
		return pixels()[index];
	}

	void set_pixel_data(uint32_t data, int index)
	{
		pixels()[index] = data;
	}
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
	Grid color_table;
	char* commandBuffer;
	Grid* activeControl = &pixels;
	int windowWidth;
	int windowHeight;
	Mode mode;

	SpriteC canvas;
	SpriteC canvas_color_table;

	bool pixels_are_selected() const
	{
		return activeControl == &pixels;
	}
};

#endif
