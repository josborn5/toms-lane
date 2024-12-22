#ifndef SPRITE_EDITOR_OPERATIONS_HPP
#define SPRITE_EDITOR_OPERATIONS_HPP

#include "./editor.hpp"

struct set_pixel_data_operation
{
	public:
		set_pixel_data_operation()
		{
		}

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

#endif
