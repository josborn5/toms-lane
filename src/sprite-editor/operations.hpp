#ifndef SPRITE_EDITOR_OPERATIONS_HPP
#define SPRITE_EDITOR_OPERATIONS_HPP

#include "./editor.hpp"
#include "./ring-buffer.hpp"

struct set_pixel_data_operation
{
	set_pixel_data_operation();
	set_pixel_data_operation(Grid* grid, uint32_t data_to_set);

	void execute();
	void undo();

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
	paste_pixel_data_operation();
	paste_pixel_data_operation(SpriteC* sprite);

	void execute();
	int add_pixel(unsigned int pixel_index, uint32_t pixel_data);
	void undo();

	private:
		SpriteC* _sprite;
		tl::stack_array<paste_pixel, 256> _pixels;
};

struct delete_row_operation
{
	delete_row_operation();
	delete_row_operation(Grid* grid);

	void execute();
	void undo();

	private:
		Grid* _grid = nullptr;
		unsigned int _row_index = 0;
		tl::stack_array<uint32_t, 256> _deleted_pixels;
};

struct insert_row_operation
{
	insert_row_operation();
	insert_row_operation(Grid* grid, unsigned int row_index);
	insert_row_operation(Grid* grid);

	void execute();
	void undo();

	private:
		Grid* _grid = nullptr;
		unsigned int _insert_at_row_index = 0;
};

struct delete_column_operation
{
	delete_column_operation();
	delete_column_operation(Grid* grid);

	void execute();
	void undo();

	private:
		Grid* _grid = nullptr;
		unsigned int _col_index = 0;
		tl::stack_array<uint32_t, 256> _deleted_pixels;
};

struct insert_column_operation
{
	insert_column_operation();
	insert_column_operation(Grid* grid);
	insert_column_operation(Grid* grid, unsigned int column_index);

	void execute();

	private:
		Grid* _grid = nullptr;
		unsigned int _insert_at_col_index = 0;
};

union generic_operation
{
	set_pixel_data_operation set_single_pixel;
	paste_pixel_data_operation set_multiple_pixels;
	insert_row_operation insert_row;
	delete_row_operation delete_row;
	insert_column_operation insert_column;
	delete_column_operation delete_column;

	generic_operation() {}
};

enum operation_type
{
	single,
	multiple,
	insert_row,
	delete_row,
	insert_column,
	delete_column
};

struct any_operation
{
	union generic_operation generic;
	operation_type type;
};

struct operation_executor
{
	void do_execute(set_pixel_data_operation& operation);
	void do_execute(paste_pixel_data_operation& operation);
	void do_execute(insert_column_operation& operation);
	void do_execute(delete_column_operation& operation);

	insert_column_operation& get_insert_column(Grid* grid);
	insert_row_operation& get_insert_row(Grid* grid);
	delete_row_operation& get_delete_row(Grid* grid);

	int do_undo();

	private:
		stack_ring_buffer<any_operation, 8> all_operations;
};

#endif
