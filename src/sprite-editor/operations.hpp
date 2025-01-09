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

struct delete_row_operation
{
	delete_row_operation();
	delete_row_operation(Grid* grid);
	delete_row_operation(Grid* grid, int row_index);

	void execute();
	void undo();

	private:
		Grid* _grid = nullptr;
		int _row_index = 0;
};

struct insert_row_operation
{
	insert_row_operation();
	insert_row_operation(Grid* grid, int row_index);
	insert_row_operation(Grid* grid);

	void execute();
	void undo();

	private:
		Grid* _grid = nullptr;
		int _insert_at_row_index = 0;
};

struct insert_column_operation
{
	public:
		insert_column_operation();
		insert_column_operation(Grid* grid);

		void execute();

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

		void do_execute(insert_row_operation& operation);

		int do_undo();

	private:
		stack_ring_buffer<any_operation, N> all_operations;
};

#endif
