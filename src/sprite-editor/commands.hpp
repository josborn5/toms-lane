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
int InsertRow(Grid& grid);
int InsertColumn(Grid& grid);

void copy_pixels(Grid& grid, int source_index, int source_range);
void cut_pixels(Grid& grid, int source_index, int source_range);

struct clipboard
{
	int start_index = 0;
	int end_index = 0;
	int row_stride = 0;
	tl::stack_array<uint32_t, 256> pixel_data;
};

int copy_to_clipboard(const SpriteC& sprite, int source_cursor_index, int source_range_index, clipboard& clipboard);
int paste_from_clipboard_operation(const Grid& grid, const clipboard& clipboard, paste_pixel_data_operation& operation);

#endif

