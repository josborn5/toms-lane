#ifndef EDITOR_H
#define EDITOR_H

#include "../tl-application.hpp"
#include "../tl-library.hpp"

struct SpriteC
{
	unsigned int width = 0;
	unsigned int height = 0;
	uint16_t bitsPerPixel = 24;

	SpriteC* p_color_table = nullptr;

	tl::array<uint32_t> pixel_array;

	void init(tl::MemorySpace memory_space)
	{
		pixel_memory = memory_space;
		pixel_array.initialize(pixel_memory, pixel_count());
	}

	void refresh() // TODO: this is a hack and should be removed. any update to width/height should trigger this refresh
	{
		pixel_array.initialize(pixel_memory, pixel_count());
	}

	unsigned int pixel_count() const
	{
		return width * height;
	}

	bool has_color_table() const
	{
		return (bitsPerPixel != 24);
	}

	uint32_t get_pixel_data(unsigned int index) const
	{
		return pixel_array.get(index);
	}

	void set_pixel_data(unsigned int index, uint32_t data)
	{
		pixel_array.get(index) = data;
	}

	unsigned int max_index() const
	{
		return pixel_count() - 1;
	}

	unsigned int max_index_on_row(unsigned int row_index) const
	{
		return (width * (row_index + 1)) - 1;
	}

	unsigned int min_index_on_row(unsigned int row_index) const
	{
		return width * row_index;
	}

	unsigned int column_index(unsigned int index) const
	{
		if (width == 1)
		{
			return 0;
		}

		return index % width;
	}

	unsigned int row_index(unsigned int index) const
	{
		if (height == 1)
		{
			return 0;
		}

		if (width == 0)
		{
			return index;
		}

		return index / width;
	}

	uint64_t size_in_bytes() const
	{
		return pixel_count() * sizeof(uint32_t);
	}

	uint64_t capacity_in_bytes() const
	{
		return pixel_memory.sizeInBytes;
	}

	int copy_from(const SpriteC& source)
	{
		if (capacity_in_bytes() < source.size_in_bytes())
		{
			return -1;
		}

		height = source.height;
		width = source.width;

		for (unsigned int i = 0; i < source.pixel_count(); i += 1)
		{
			uint32_t to_copy = source.get_pixel_data(i);
			set_pixel_data(i, to_copy);
		}

		return 0;
	}

	private:
		tl::MemorySpace pixel_memory = {0};
};

typedef void grid_size_change_callback();

struct item_in_grid
{
	void initialize(const SpriteC* sprite)
	{
		_sprite = sprite;
	}

	unsigned int index() const
	{
		return _index;
	}

	uint32_t color() const
	{
		return _sprite->get_pixel_data(_index);
	}

	unsigned int row_index() const
	{
		return _sprite->row_index(_index);
	}

	unsigned int column_index() const
	{
		return _sprite->column_index(_index);
	}

	unsigned int column_end_index() const
	{
		unsigned int top_left_index = _sprite->pixel_count() - _sprite->width;
		return top_left_index + column_index();
	}

	void move_left()
	{
		unsigned int current_row_index = row_index();
		int next_index = _index - 1;
		if (next_index >= (int)_sprite->min_index_on_row(current_row_index))
		{
			_index = next_index;
		}
	}

	void move_right()
	{
		unsigned int current_row_index = row_index();
		unsigned int next_index = _index + 1;
		if (next_index <= _sprite->max_index_on_row(current_row_index))
		{
			_index = next_index;
		}
	}

	void move_row_start()
	{
		unsigned int current_row_index = row_index();
		_index = _sprite->min_index_on_row(current_row_index);
	}

	void move_row_end()
	{
		unsigned int current_row_index = row_index();
		_index = _sprite->max_index_on_row(current_row_index);
	}

	void move_up()
	{
		unsigned int next_index = _index + _sprite->width;
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
		unsigned int current_row_index = row_index();
		unsigned int min_row = _sprite->min_index_on_row(current_row_index);
		jump_to_next_color(-1, min_row, _sprite->max_index());
	}

	void color_jump_right()
	{
		unsigned int current_row_index = row_index();
		unsigned int max_row = _sprite->max_index_on_row(current_row_index);
		jump_to_next_color(1, 0, max_row);
	}

	void color_jump_up()
	{
		jump_to_next_color(_sprite->width, 0, _sprite->max_index());
	}

	void color_jump_down()
	{
		jump_to_next_color(-(int)_sprite->width, 0, _sprite->max_index());
	}

	void set_index(unsigned int index)
	{
		if (index > _sprite->max_index()) return;
		_index = index;
	}

	void set_index(unsigned int col_index, unsigned int row_index)
	{
		unsigned int index = col_index + (_sprite->width * row_index);
		set_index(index);
	}

	void center()
	{
		unsigned int center_col_index = _sprite->width / 2;
		unsigned int center_row_index = _sprite->height / 2;
		set_index(center_col_index, center_row_index);
	}

	protected:
		const SpriteC* _sprite = nullptr;
		unsigned int _index = 0;

	private:
		void jump_to_next_color(int step, int inclusiveMinPixelIndex, int inclusiveMaxPixelIndex)
		{
			uint32_t active_color = color();
			int cursor = _index;
			int next_index = _index + step;
			bool sameColor = true;
			while (next_index >= inclusiveMinPixelIndex && next_index <= inclusiveMaxPixelIndex && sameColor)
			{
				cursor = next_index;
				sameColor = (active_color == _sprite->get_pixel_data(cursor));
				next_index += step;
			}
			_index = cursor;
		}
};

struct sprite_camera : item_in_grid
{
	unsigned int bottom_row_index()
	{
		if (zoom == 1.0f)
		{
			unsigned int half_height = _sprite->height / 2;
			unsigned int r_index = row_index();
			return r_index > half_height ? r_index - half_height : 0;
		}

		float half_height = zoom * (float)_sprite->height * 0.5f;
		unsigned int r_index = row_index();
		return ((float)r_index > half_height) ? (unsigned int)((float)r_index - half_height) : 0;
	}

	unsigned int top_row_index()
	{
		unsigned int max_top_index = _sprite->height - 1;
		if (zoom == 1.0f)
		{
			unsigned int half_height = _sprite->height / 2;
			unsigned int r_index = row_index();
			unsigned int top_index = r_index + half_height;
			return (top_index > max_top_index) ? max_top_index : top_index;
		}

		float half_height = zoom * (float)_sprite->height * 0.5f;
		unsigned int r_index = row_index();
		float top_index = (float)r_index + half_height;
		return (top_index > (float)max_top_index) ? max_top_index : (unsigned int)top_index;
	}

	unsigned int left_column_index()
	{
		if (zoom == 1.0f)
		{
			unsigned int half_width = _sprite->width / 2;
			unsigned int c_index = column_index();
			return c_index > half_width ? c_index - half_width : 0;
		}
		float half_width = zoom * (float)_sprite->width * 0.5f;
		unsigned int c_index = column_index();
		return ((float)c_index > half_width) ? (unsigned int)((float)c_index - half_width) : 0;
	}

	unsigned int right_column_index()
	{
		unsigned int max_right_index = _sprite->width - 1;
		if (zoom == 1.0f)
		{
			unsigned int half_width = _sprite->width / 2;
			unsigned int c_index = column_index();
			unsigned int right_index = c_index + half_width;
			return (right_index > max_right_index) ? max_right_index : right_index;
		}

		float half_width = zoom * (float)_sprite->width * 0.5f;
		unsigned int c_index = column_index();
		float right_index = (float)c_index + half_width;
		return (right_index > (float)max_right_index) ? max_right_index : (unsigned int)right_index;
	}

	void zoom_in()
	{
		zoom *= 0.75f;
	}

	void zoom_out()
	{
		zoom /= 0.75f;
	}

	float width_in_pixels()
	{
		return zoom * (float)_sprite->width;
	}

	float height_in_pixels()
	{
		return zoom * (float)_sprite->height;
	}

	void reset()
	{
		center();
		zoom = 1.0f;
	}

	float zoom = 1.0f;
};

struct Grid
{
	SpriteC* sprite;

	item_in_grid cursor;
	item_in_grid range;
	sprite_camera camera_focus;

	grid_size_change_callback* size_change_callback;

	void initialize(SpriteC* sprite_data)
	{
		sprite = sprite_data;
		cursor.initialize(sprite_data);
		range.initialize(sprite_data);
		camera_focus.initialize(sprite_data);
	}

	void reset_items()
	{
		cursor.move_start();
		range.move_start();
		camera_focus.center();
	}

	uint32_t selected_color() const
	{
		return cursor.color();
	}

	void size()
	{
		sprite->refresh();
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
	Insert,
	ViewNoGrid
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

	bool quit = false;

	bool pixels_are_selected() const
	{
		return activeControl == &pixels;
	}
};

#endif
