#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"
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

struct insert_row_operation
{
	insert_row_operation(SpriteC* sprite, unsigned int insert_at_row_index)
	{
		_sprite = sprite;
		_insert_at_row_index = insert_at_row_index;
	}

	void execute()
	{
		// Shift down rows after the new row
		int first_move_pixel_index = _insert_at_row_index * _sprite->width;
		for (int source_index = _sprite->max_index(); source_index >= first_move_pixel_index; source_index -= 1)
		{
			int target_index = source_index + _sprite->width;
			uint32_t to_move = _sprite->get_pixel_data(source_index);
			_sprite->set_pixel_data(target_index, to_move);
		}

		// Clear out pixels in the new row
		int first_new_pixel_index = first_move_pixel_index;
		int first_moved_pixel_index = first_new_pixel_index  + _sprite->width;
		for (int i = first_new_pixel_index ; i < first_moved_pixel_index ; i += 1)
		{
			_sprite->set_pixel_data(i, 0x000000);
		}

		_sprite->height += 1;
	}

	private:
		SpriteC* _sprite = nullptr;
		unsigned int _insert_at_row_index = 0;
};

int InsertRow(Grid& grid)
{
	uint64_t new_size_in_bytes = grid.sprite->size_in_bytes() + (grid.sprite->width * sizeof(uint32_t));
	if (new_size_in_bytes > grid.sprite->pixel_memory.sizeInBytes)
	{
		return -1;
	}

	insert_row_operation insert_operation = insert_row_operation(grid.sprite, grid.cursor.row_index());
	insert_operation.execute();

	grid.cursor.move_up();
	return 0;
}

static void AppendColumnToSpriteC(SpriteC& sprite, int insertAtIndex)
{
	/*
		Current: 2x3 grid of pixels
		oo
		oo
		oo

		oo|oo|oo
		01 23 45

		Insert new column at row index 1:
		ono
		ono
		ono

		ono|ono|ono
		012 345 678

		Pixel index transformations:
		5-->8
		4-->6
		3-->5
		2-->3
		1-->2
		0-->0

		transformation is:
			offsetForRow = rowIndex;
			offsetForColumn = (pixelAfterNewColumnInRow) ? 1 : 0;
			finalPixelOffset = offsetForRow + offsetForColumn

		where pixelAfterNewColumnInRow = (oldPixelIndex % oldWidth) >= insertColumnIndex
	*/

	// shift any columns to the right of the new column
	for (int sourceIndex = sprite.max_index(); sourceIndex >= 0; sourceIndex -= 1)
	{
		int currentRowIndex = sourceIndex / sprite.width;
		bool pixelIsAfterNewColumnInRow = (sourceIndex % sprite.width) >= insertAtIndex;
		int offsetForColumn = (pixelIsAfterNewColumnInRow) ? 1 : 0;
		int offset = currentRowIndex + offsetForColumn;
		int targetIndex = sourceIndex + offset;
		uint32_t to_move = sprite.get_pixel_data(sourceIndex);
		sprite.set_pixel_data(targetIndex, to_move);
	}

	sprite.width += 1;
	// clear pixels in the new column
	for (int i = insertAtIndex; i < sprite.pixel_count(); i += sprite.width)
	{
		sprite.set_pixel_data(i, 0x000000);
	}
}

int InsertColumn(Grid& grid)
{
	uint64_t new_size_in_bytes = grid.sprite->size_in_bytes() + (grid.sprite->height * sizeof(uint32_t));
	if (new_size_in_bytes > grid.sprite->pixel_memory.sizeInBytes)
	{
		return -1;
	}

	int selectedColumnIndex = grid.cursor.column_index();
	int selectedRowIndex = grid.cursor.row_index();
	AppendColumnToSpriteC(*grid.sprite, selectedColumnIndex);
	int new_cursor_index = grid.cursor.index() + 1 + selectedRowIndex;
	grid.cursor.set_index(new_cursor_index);
	return 0;
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
		clipboard.pixel_data.append(to_copy); // TODO: check append result

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
		uint32_t to_copy = clipboard.pixel_data.get_copy(clipboard_index).value;

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
		clipboard.pixel_data.append(to_copy);
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

