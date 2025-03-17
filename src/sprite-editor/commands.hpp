#ifndef SPRITE_EDITOR_COMMANDS_HPP
#define SPRITE_EDITOR_COMMANDS_HPP

#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./operations.hpp"

int SaveBitmap(
	const tl::MemorySpace& tempMemory,
	const SpriteC& sprite,
	char* filePath
);

bool can_insert_row(const SpriteC& sprite);
bool can_insert_column(const SpriteC& sprite);

bool can_delete_row(const SpriteC& sprite);
bool can_delete_column(const SpriteC& sprite);

struct clipboard
{
	int start_index = 0;
	int end_index = 0;
	int row_stride = -1;

	void append(uint32_t value)
	{
		pixel_data.append(value);
	}

	uint32_t read(unsigned int index) const
	{
		return pixel_data.get_copy(index).value;
	}

	void clear()
	{
		start_index = 0;
		end_index = 0;
		row_stride = -1;
		pixel_data.clear();
	}

	bool is_set() const
	{
		return row_stride != -1;
	}

	private:
		tl::stack_array<uint32_t, 256> pixel_data;
};

int copy_to_clipboard(
	const SpriteC& sprite,
	int source_cursor_index,
	int source_range_index,
	clipboard& clipboard
);
int cut_to_clipboard_operation(
	const SpriteC& sprite,
	int source_cursor_index,
	int source_range_index,
	paste_pixel_data_operation& cut_operation,
	clipboard& clipboard
);
int paste_from_clipboard_operation(
	const Grid& grid,
	const clipboard& clipboard,
	paste_pixel_data_operation& paste_operation
);

#endif

