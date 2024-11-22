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

	void set_pixel_data(int index, uint32_t data)
	{
		pixels()[index] = data;
	}

	int max_index() const
	{
		return pixel_count() - 1;
	}

	int max_index_on_row(int row_index) const
	{
		return (width * (row_index + 1)) - 1;
	}

	int min_index_on_row(int row_index) const
	{
		return width * row_index;
	}
};

struct sprite_camera
{
	float zoom = 1.0f;
	tl::Vec2<float> displacement = {0};
};

typedef void grid_size_change_callback();

struct item_in_grid
{
	void initialize(const SpriteC* sprite)
	{
		_sprite = sprite;
	}

	int index() const
	{
		return _index;
	}

	uint32_t color() const
	{
		return _sprite->pixels()[_index];
	}

	int row_index() const
	{
		if (_sprite->height == 1)
		{
			return 0;
		}

		if (_sprite->width == 0)
		{
			return -1;
		}

		return _index / _sprite->width;
	}

	int column_index() const
	{
		if (_sprite->width == 1)
		{
			return 0;
		}

		return _index % _sprite->width;
	}

	int column_end_index() const
	{
		int top_left_index = _sprite->pixel_count() - _sprite->width;
		return top_left_index + column_index();
	}

	void move_left()
	{
		int current_row_index = row_index();
		int next_index = _index - 1;
		if (next_index >= _sprite->min_index_on_row(current_row_index))
		{
			_index = next_index;
		}
	}

	void move_right()
	{
		int current_row_index = row_index();
		int next_index = _index + 1;
		if (next_index <= _sprite->max_index_on_row(current_row_index))
		{
			_index = next_index;
		}
	}

	void move_row_start()
	{
		int current_row_index = row_index();
		_index = _sprite->min_index_on_row(current_row_index);
	}

	void move_row_end()
	{
		int current_row_index = row_index();
		_index = _sprite->max_index_on_row(current_row_index);
	}

	void move_up()
	{
		int next_index = _index + _sprite->width;
		if (next_index <= _sprite->max_index())
		{
			_index = next_index;
		}
	}

	void move_down()
	{
		int next_index = _index - _sprite->width;
		if (next_index >= 0)
		{
			_index = next_index;
		}
	}

	void move_start()
	{
		_index = 0;
	}

	void move_end()
	{
		_index = _sprite->max_index();
	}

	void color_jump_left()
	{
		int current_row_index = row_index();
		int min_row = _sprite->min_index_on_row(current_row_index);
		jump_to_next_color(-1, min_row, _sprite->max_index());
	}

	void color_jump_right()
	{
		int current_row_index = row_index();
		int max_row = _sprite->max_index_on_row(current_row_index);
		jump_to_next_color(1, 0, max_row);
	}

	void color_jump_up()
	{
		jump_to_next_color(_sprite->width, 0, _sprite->max_index());
	}

	void color_jump_down()
	{
		jump_to_next_color(-_sprite->width, 0, _sprite->max_index());
	}

	void set_index(int index)
	{
		if (index < 0 || index > _sprite->max_index()) return;
		_index = index;
	}

	private:
		const SpriteC* _sprite = nullptr;
		int _index = 0;

		void jump_to_next_color(int step, int inclusiveMinPixelIndex, int inclusiveMaxPixelIndex)
		{
			uint32_t active_color = color();
			int cursor = _index;
			int next_index = _index + step;
			bool sameColor = true;
			while (next_index >= inclusiveMinPixelIndex && next_index <= inclusiveMaxPixelIndex && sameColor)
			{
				cursor = next_index;
				sameColor = (active_color == _sprite->pixels()[cursor]);
				next_index += step;
			}
			_index = cursor;
		}
};

struct Grid
{
	SpriteC* sprite;
	sprite_camera camera;

	item_in_grid cursor;
	item_in_grid range;

	grid_size_change_callback* size_change_callback;

	void initialize(SpriteC* sprite_data)
	{
		sprite = sprite_data;
		cursor.initialize(sprite_data);
		range.initialize(sprite_data);
	}

	uint32_t selected_color() const
	{
		return cursor.color();
	}

	void size()
	{
		if (size_change_callback != nullptr)
		{
			size_change_callback();
		}
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
