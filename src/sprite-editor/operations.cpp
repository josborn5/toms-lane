#include "./operations.hpp"


set_pixel_data_operation::set_pixel_data_operation() {}
set_pixel_data_operation::set_pixel_data_operation(Grid* grid, uint32_t data_to_set)
{
	_sprite = grid->sprite;
	_set_at_index = grid->cursor.index();
	_data_to_set = data_to_set;
}
void set_pixel_data_operation::execute()
{
	_original_data = _sprite->get_pixel_data(_set_at_index);
	_sprite->set_pixel_data(_set_at_index, _data_to_set);
}
void set_pixel_data_operation::undo()
{
	_sprite->set_pixel_data(_set_at_index, _original_data);
}


paste_pixel_data_operation::paste_pixel_data_operation() {}
paste_pixel_data_operation::paste_pixel_data_operation(SpriteC* sprite)
{
	_sprite = sprite;
}
void paste_pixel_data_operation::execute()
{
	for (unsigned int i = 0; i < _pixels.length(); i += 1)
	{
		paste_pixel* pixel = _pixels.get_pointer(i).value;
		pixel->original_data = _sprite->get_pixel_data(pixel->index);
		_sprite->set_pixel_data(pixel->index, pixel->data_to_set);
	}
}
int paste_pixel_data_operation::add_pixel(unsigned int pixel_index, uint32_t pixel_data)
{
	paste_pixel pixel;
	pixel.index = pixel_index;
	pixel.data_to_set = pixel_data;
	return _pixels.append(pixel);
}
void paste_pixel_data_operation::undo()
{
	for (unsigned int i = 0; i < _pixels.length(); i += 1)
	{
		paste_pixel* pixel = _pixels.get_pointer(i).value;
		_sprite->set_pixel_data(pixel->index, pixel->original_data);
	}
}



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
	unsigned int col_index = _grid->cursor.column_index();
	unsigned int start_index = _grid->sprite->min_index_on_row(_row_index);
	unsigned int end_index = _grid->sprite->max_index_on_row(_row_index);
	unsigned int total_length = _grid->sprite->pixel_count();

	// Call tl::DeleteFromArray with the sprite content
	tl::DeleteFromArray(_grid->sprite->pixels(), start_index, end_index, total_length);

	_grid->sprite->height -= 1;
	_grid->size();

	int row_index = (_row_index >= _grid->sprite->height) ? _grid->sprite->height - 1 : _row_index;
	_grid->cursor.set_index(col_index, row_index);
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
	int col_index = _grid->cursor.column_index();
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
	_grid->cursor.set_index(col_index, _insert_at_row_index);
	_grid->size();
}
void insert_row_operation::undo()
{
	delete_row_operation delete_op = delete_row_operation(_grid, _insert_at_row_index);
	delete_op.execute();
}


delete_column_operation::delete_column_operation() {}
delete_column_operation::delete_column_operation(Grid* grid)
{
	_grid = grid;
	_col_index = grid->cursor.index();
}
void delete_column_operation::execute()
{
	unsigned int sprite_length = _grid->sprite->pixel_count();
	for (int i = _grid->sprite->height - 1; i >= 0; i -= 1)
	{
		unsigned int delete_index = (i * _grid->sprite->width) + _col_index;
		tl::DeleteFromArray(_grid->sprite->pixels(), delete_index, delete_index, sprite_length);
		sprite_length -= 1;
	}

	_grid->sprite->width -= 1;
	_grid->size();
}
void delete_column_operation::undo()
{

}


insert_column_operation::insert_column_operation() {}
insert_column_operation::insert_column_operation(Grid* grid)
{
	_grid = grid;
	_insert_at_col_index = _grid->cursor.column_index();
}
void insert_column_operation::execute()
{
	int row_index = _grid->cursor.row_index();
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

	_sprite->width += 1;
	// clear pixels in the new column
	for (int i = _insert_at_col_index; i < _sprite->pixel_count(); i += _sprite->width)
	{
		_sprite->set_pixel_data(i, 0x000000);
	}

	_grid->cursor.set_index(_insert_at_col_index, row_index);
	_grid->size();
}


void operation_executor::do_execute(set_pixel_data_operation& operation)
{
	any_operation any_op;
	any_op.generic.set_single_pixel = operation;
	any_op.type = single;
	all_operations.push(any_op);

	operation.execute();
}
void operation_executor::do_execute(paste_pixel_data_operation& operation)
{
	any_operation any_op;
	any_op.generic.set_multiple_pixels = operation;
	any_op.type = multiple;
	all_operations.push(any_op);

	operation.execute();
}
void operation_executor::do_execute(insert_row_operation& operation)
{
	any_operation any_op;
	any_op.generic.insert_row = operation;
	any_op.type = insert_row;
	all_operations.push(any_op);

	operation.execute();
}
void operation_executor::do_execute(delete_row_operation& operation)
{
	any_operation any_op;
	any_op.generic.delete_row = operation;
	any_op.type = delete_row;
	all_operations.push(any_op);

	operation.execute();
}
void operation_executor::do_execute(insert_column_operation& operation)
{
	any_operation any_op;
	any_op.generic.insert_column = operation;
	any_op.type = insert_column;
	all_operations.push(any_op);

	operation.execute();
}
void operation_executor::do_execute(delete_column_operation& operation)
{
	any_operation any_op;
	any_op.generic.delete_column = operation;
	any_op.type = delete_column;
	all_operations.push(any_op);

	operation.execute();
}
int operation_executor::do_undo()
{
	operation<any_operation> result = all_operations.pop();
	if (result.result == operation_success)
	{
		switch (result.value.type)
		{
			case single:
			{
				result.value.generic.set_single_pixel.undo();
				break;
			}
			case multiple:
			{
				result.value.generic.set_multiple_pixels.undo();
				break;
			}
			case insert_row:
			{
				result.value.generic.insert_row.undo();
				break;
			}
			case delete_row:
			{
				result.value.generic.delete_row.undo();
			}
		}

		return 0;
	}
	return 1;
}

