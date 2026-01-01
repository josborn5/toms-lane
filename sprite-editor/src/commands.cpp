#include "../../application/src/tl-application.hpp"
#include "../../src/tl-library.hpp"
#include "./editor.hpp"
#include "./transform.hpp"
#include "./operations.hpp"
#include "./commands.hpp"

int SaveBitmap(
	const tl::MemorySpace& tempMemory,
	const SpriteC& sprite,
	char* filePath
)
{
	tl::bitmap blankBitmap;
	tl::MemorySpace remainder = tempMemory;
	int bitmapWriteResult = InitializeBitmapFromSpriteC(sprite, blankBitmap, remainder);
	if (bitmapWriteResult != 0) return -1;

	tl::CarveMemorySpace(blankBitmap.file_header.fileSizeInBytes, remainder);

	int writeResult = tl::bitmap_interface_write(blankBitmap, remainder);
	if (writeResult != tl::bitmap_write_success) return -2;

	tl::MemorySpace fileData = CarveMemorySpace(blankBitmap.file_header.fileSizeInBytes, remainder);

	return tl::file_interface_write(filePath, fileData);
}

bool can_insert_row(const SpriteC& sprite)
{
	uint64_t new_size_in_bytes = sprite.size_in_bytes() + (sprite.width * sizeof(uint32_t));
	return new_size_in_bytes <= sprite.capacity_in_bytes();
}

bool can_insert_column(const SpriteC& sprite)
{
	uint64_t new_size_in_bytes = sprite.size_in_bytes() + (sprite.height * sizeof(uint32_t));
	return (new_size_in_bytes <= sprite.capacity_in_bytes());
}

static void get_indexes_for_copy(
	const SpriteC& sprite,
	int source_cursor_index,
	int source_range_index,
	clipboard& write_clipboard,
	int& row_hop
)
{
	bool start_at_cursor = (source_cursor_index < source_range_index);
	write_clipboard.start_index = (start_at_cursor) ? source_cursor_index : source_range_index;
	write_clipboard.end_index = (start_at_cursor) ? source_range_index : source_cursor_index;

	int source_cursor_col_index = sprite.column_index(source_cursor_index);
	int source_range_col_index = sprite.column_index(source_range_index);
	bool cursor_left_of_range = (source_cursor_col_index < source_range_col_index);
	int source_start_col_index = (cursor_left_of_range) ? source_cursor_col_index : source_range_col_index;
	int source_end_col_index = (cursor_left_of_range) ? source_range_col_index : source_cursor_col_index;

	write_clipboard.row_stride = source_end_col_index - source_start_col_index;
	row_hop = sprite.width - write_clipboard.row_stride;
}

static void get_indexes_for_paste(
	const Grid& grid,
	const clipboard& clipboard,
	int& target_start_index,
	int& target_end_index,
	int& row_stride,
	int& row_hop
)
{
	target_start_index = grid.cursor.index();
	target_end_index = target_start_index - clipboard.start_index + clipboard.end_index;

	int target_start_col_index = grid.cursor.column_index();
	int target_end_col_index = target_start_col_index + clipboard.row_stride;
	int max_col_index = grid.sprite->max_index_on_row(0);

	row_stride = clipboard.row_stride;

	if (target_end_col_index > max_col_index)
	{
		int columns_out_of_bounds = target_end_col_index - max_col_index;
		row_stride -= columns_out_of_bounds;
		target_end_index -= columns_out_of_bounds;
	}

	int target_end_row_index = grid.sprite->row_index(target_end_index);
	int max_row_index = grid.sprite->height - 1;
	if (target_end_row_index > max_row_index)
	{
		int rows_out_of_bounds = target_end_row_index - max_row_index;
		target_end_index -= (grid.sprite->width * rows_out_of_bounds);
	}

	row_hop = grid.sprite->width - row_stride;
}

int copy_to_clipboard(
	const SpriteC& sprite,
	int source_cursor_index,
	int source_range_index,
	clipboard& clipboard
)
{
	clipboard.clear();
	int row_hop;
	get_indexes_for_copy(
		sprite,
		source_cursor_index,
		source_range_index,
		clipboard,
		row_hop
	);

	int row_stride_counter = 0;
	int source_index = clipboard.start_index;
	while (source_index <= clipboard.end_index)
	{
		uint32_t to_copy = sprite.get_pixel_data(source_index);
		clipboard.append(to_copy); // TODO: check append result

		if (row_stride_counter < clipboard.row_stride)
		{
			source_index += 1;
			row_stride_counter += 1;
		}
		else
		{
			source_index += row_hop;
			row_stride_counter = 0;
		}
	}

	return 0;
}

int paste_from_clipboard_operation(const Grid& grid, const clipboard& clipboard, paste_pixel_data_operation& operation)
{
	int row_stride, row_hop, target_start_index, target_end_index;
	get_indexes_for_paste(
		grid,
		clipboard,
		target_start_index,
		target_end_index,
		row_stride,
		row_hop
	);

	int clipboard_index = 0;
	int row_stride_counter = 0;
	int target_index = target_start_index;
	int clipboard_row_hop = 1 + clipboard.row_stride - row_stride;

	while (target_index <= target_end_index)
	{
		uint32_t to_copy = clipboard.read(clipboard_index);

		operation.add_pixel(target_index, to_copy);

		if (row_stride_counter < row_stride)
		{
			clipboard_index += 1;
			target_index += 1;
			row_stride_counter += 1;
		}
		else
		{
			clipboard_index += clipboard_row_hop;
			target_index += row_hop;
			row_stride_counter = 0;
		}
	}

	return 0;
}

int cut_to_clipboard_operation(
	const SpriteC& sprite,
	int source_cursor_index,
	int source_range_index,
	paste_pixel_data_operation& clear_source_operation,
	clipboard& clipboard
)
{
	clipboard.clear();
	int row_hop;
	get_indexes_for_copy(
		sprite,
		source_cursor_index,
		source_range_index,
		clipboard,
		row_hop
	);

	int source_index;
	int row_stride_counter = 0;
	source_index = clipboard.start_index;
	while (source_index <= clipboard.end_index)
	{
		uint32_t to_copy = sprite.get_pixel_data(source_index);
		clipboard.append(to_copy);
		clear_source_operation.add_pixel(source_index, 0);

		if (row_stride_counter < clipboard.row_stride)
		{
			source_index += 1;
			row_stride_counter += 1;
		}
		else
		{
			source_index += row_hop;
			row_stride_counter = 0;
		}
	}

	return 0;
}

bool can_delete_row(const SpriteC& sprite)
{
	return sprite.height > 1;
}
bool can_delete_column(const SpriteC& sprite)
{
	return sprite.width > 1;
}

