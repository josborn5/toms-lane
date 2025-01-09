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
	_grid->size();
}
void delete_row_operation::undo()
{
	insert_row_operation undo_operation = insert_row_operation(_grid, _row_index);
	undo_operation.execute();
}


insert_row_operation::insert_row_operation() {}
insert_row_operation::insert_row_operation(Grid* grid, int row_index)
{
	_grid = grid;
	_insert_at_row_index = row_index;
}
insert_row_operation::insert_row_operation(Grid* grid)
{
	_grid = grid;
	_insert_at_row_index = _grid->cursor.row_index();
}
void insert_row_operation::execute()
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
void insert_row_operation::undo()
{
	delete_row_operation delete_op = delete_row_operation(_grid, _insert_at_row_index);
	delete_op.execute();
}


