#ifndef SPRITE_EDITOR_OPERATIONS_HPP
#define SPRITE_EDITOR_OPERATIONS_HPP

#include "./editor.hpp"
#include "./ring-buffer.hpp"

struct set_pixel_data_operation
{
	public:
		set_pixel_data_operation() {}

		set_pixel_data_operation(Grid* grid, uint32_t data_to_set)
		{
			_sprite = grid->sprite;
			_set_at_index = grid->cursor.index();
			_data_to_set = data_to_set;
		}

		void execute()
		{
			_original_data = _sprite->get_pixel_data(_set_at_index);
			_sprite->set_pixel_data(_set_at_index, _data_to_set);
		}

		void undo()
		{
			_sprite->set_pixel_data(_set_at_index, _original_data);
		}

	private:
		SpriteC* _sprite = nullptr;
		unsigned int _set_at_index = 0;
		uint32_t _data_to_set = 0;
		uint32_t _original_data = 0;
};

struct paste_pixel
{
	unsigned int index = 0;
	uint32_t data_to_set = 0;
	uint32_t original_data = 0;
};

struct paste_pixel_data_operation
{
	paste_pixel_data_operation() {}

	paste_pixel_data_operation(SpriteC* sprite)
	{
		_sprite = sprite;
	}

	void execute()
	{
		for (unsigned int i = 0; i < _pixels.length(); i += 1)
		{
			paste_pixel* pixel = _pixels.get_pointer(i).value;
			pixel->original_data = _sprite->get_pixel_data(pixel->index);
			_sprite->set_pixel_data(pixel->index, pixel->data_to_set);
		}
	}

	int add_pixel(unsigned int pixel_index, uint32_t pixel_data)
	{
		paste_pixel pixel;
		pixel.index = pixel_index;
		pixel.data_to_set = pixel_data;
		return _pixels.append(pixel);
	}

	void undo()
	{
		for (unsigned int i = 0; i < _pixels.length(); i += 1)
		{
			paste_pixel* pixel = _pixels.get_pointer(i).value;
			_sprite->set_pixel_data(pixel->index, pixel->original_data);
		}
	}

	private:
		SpriteC* _sprite;
		tl::stack_array<paste_pixel, 256> _pixels;
};

struct insert_row_operation
{
	insert_row_operation() {}

	insert_row_operation(Grid* grid)
	{
		_grid = grid;
		_insert_at_row_index = _grid->cursor.row_index();
	}

	void execute()
	{
		// Shift down rows after the new row
		int first_move_pixel_index = _insert_at_row_index * _grid->sprite->width;
		for (int source_index = _grid->sprite->max_index(); source_index >= first_move_pixel_index; source_index -= 1)
		{
			int target_index = source_index + _grid->sprite->width;
			uint32_t to_move = _grid->sprite->get_pixel_data(source_index);
			_grid->sprite->set_pixel_data(target_index, to_move);
		}

		// Clear out pixels in the new row
		int first_new_pixel_index = first_move_pixel_index;
		int first_moved_pixel_index = first_new_pixel_index  + _grid->sprite->width;
		for (int i = first_new_pixel_index ; i < first_moved_pixel_index ; i += 1)
		{
			_grid->sprite->set_pixel_data(i, 0x000000);
		}

		_grid->sprite->height += 1;
		_grid->cursor.move_up();
		_grid->size();
	}

	private:
		Grid* _grid = nullptr;
		int _insert_at_row_index = 0;
};

struct insert_column_operation
{
	public:
		insert_column_operation() {}

		insert_column_operation(Grid* grid)
		{
			_grid = grid;
			_insert_at_col_index = _grid->cursor.column_index();
		}

		void execute()
		{
			SpriteC* _sprite = _grid->sprite;
			// shift any columns to the right of the new column
			for (int source_index = _sprite->max_index(); source_index >= 0; source_index -= 1)
			{
				bool after_new_column = (source_index % _sprite->width) >= _grid->cursor.column_index();
				int offset_for_column = (after_new_column) ? 1 : 0;
				int offset = _sprite->row_index(source_index) + offset_for_column;
				int target_index = source_index + offset;
				uint32_t to_move = _sprite->get_pixel_data(source_index);
				_sprite->set_pixel_data(target_index, to_move);
			}

			int row_index = _grid->cursor.row_index();
			_sprite->width += 1;
			// clear pixels in the new column
			for (int i = _insert_at_col_index; i < _sprite->pixel_count(); i += _sprite->width)
			{
				_sprite->set_pixel_data(i, 0x000000);
			}

			int new_cursor_index = _grid->cursor.index() + row_index;
			_grid->cursor.set_index(new_cursor_index);
			_grid->cursor.move_right();
			_grid->size();
		}

	private:
		Grid* _grid = nullptr;
		int _insert_at_col_index = 0;
};

union generic_operation
{
	set_pixel_data_operation set_single_pixel;
	paste_pixel_data_operation set_multiple_pixels;
	insert_row_operation insert_row;
	insert_column_operation insert_column;

	generic_operation() {}
};

enum operation_type
{
	single,
	multiple,
	insert_row,
	insert_column
};

struct any_operation
{
	union generic_operation generic;
	operation_type type;
};

template<int N>
struct operation_executor
{
	public:
		void do_set_single_pixel(set_pixel_data_operation& operation)
		{
			any_operation any_op;
			any_op.generic.set_single_pixel = operation;
			any_op.type = single;
			all_operations.push(any_op);

			operation.execute();
		}

		void do_set_multiple_pixels(paste_pixel_data_operation& operation)
		{
			any_operation any_op;
			any_op.generic.set_multiple_pixels = operation;
			any_op.type = multiple;
			all_operations.push(any_op);

			operation.execute();
		}

		void do_insert_row(insert_row_operation& operation)
		{
			any_operation any_op;
			any_op.generic.insert_row = operation;
			any_op.type = insert_row;
			all_operations.push(any_op);

			operation.execute();
		}

		void do_insert_column(insert_column_operation& operation)
		{
			any_operation any_op;
			any_op.generic.insert_column = operation;
			any_op.type = insert_column;
			all_operations.push(any_op);

			operation.execute();
		}

		int do_undo()
		{
			operation<any_operation> result = all_operations.pop();
			if (result.result == operation_success)
			{
				switch (result.value.type)
				{
					case single:
						{
							result.value.generic.set_single_pixel.undo();;
						}
						break;
					case multiple:
						{
							result.value.generic.set_multiple_pixels.undo();
						}
						break;
				}

				return 0;
			}
			return 1;
		}

	private:
		stack_ring_buffer<any_operation, N> all_operations;
};

#endif
