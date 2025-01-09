#include "./operations.hpp"

delete_row_operation::delete_row_operation() {}

delete_row_operation::delete_row_operation(Grid* grid)
{
	_grid = grid;
	_row_index = _grid->cursor.row_index();
}

delete_row_operation::delete_row_operation(Grid* grid, int row_index)
{
	_grid = grid;
	_row_index = row_index;
}

void delete_row_operation::execute()
{
	unsigned int start_index = _grid->sprite->min_index_on_row(_row_index);
	unsigned int end_index = _grid->sprite->max_index_on_row(_row_index);
	unsigned int total_length = _grid->sprite->pixel_count();

	// Call tl::DeleteFromArray with the sprite content
	tl::DeleteFromArray(_grid->sprite->pixels(), start_index, end_index, total_length);

	_grid->sprite->height -= 1;
}

void delete_row_operation::undo()
{
	insert_row_operation undo_operation = insert_row_operation(_grid, _row_index);
	undo_operation.execute();
}

