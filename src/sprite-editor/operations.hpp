#ifndef SPRITE_EDITOR_OPERATIONS_HPP
#define SPRITE_EDITOR_OPERATIONS_HPP

#include "./editor.hpp"

struct set_pixel_data_operation
{
	set_pixel_data_operation(Grid* grid, uint32_t data_to_set)
	{
		_grid = grid;
		_set_at_index = grid->cursor.index();
		_data_to_set = data_to_set;
	}

	void execute()
	{
		_original_data = _grid->sprite->get_pixel_data(_set_at_index);
		_grid->sprite->set_pixel_data(_set_at_index, _data_to_set);
	}

	void undo()
	{
		_grid->sprite->set_pixel_data(_set_at_index, _original_data);
	}

	private:
		Grid* _grid;
		int _set_at_index;
		uint32_t _data_to_set;
		uint32_t _original_data;
};

#endif
