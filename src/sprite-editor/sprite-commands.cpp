#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"
#include "./transform.hpp"
#include "./operations.hpp"

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

static int AppendRowToSpriteC(SpriteC& sprite, int insertAtIndex)
{
	/*
		Current: 3x2 grid of pixels
		ooo
		ooo

		ooo|ooo
		012 345

		Insert new row at row index 1:
		ooo
		nnn
		ooo

		ooo|nnn|ooo
		012 345 678

		Pixel index transformations:
		5-->8
		4-->7
		3-->6
		2-->2
		1-->1
		0-->0
	*/

	// Check there is space to add a final row
	int currentPixelCount = sprite.width * sprite.height;
	uint64_t currentPixelSpace = currentPixelCount * sizeof(uint32_t);
	uint64_t newPixelSpace = sprite.width * sizeof(uint32_t);
	uint64_t availableSpace = sprite.pixel_memory.sizeInBytes - currentPixelSpace;

	if (availableSpace < newPixelSpace)
	{
		return -1;
	}

	// Shift down rows after the new row
	int lastPixelIndex = currentPixelCount - 1;
	int firstMovePixelIndex = insertAtIndex * sprite.width;
	for (int sourceIndex = lastPixelIndex; sourceIndex >= firstMovePixelIndex; sourceIndex -= 1)
	{
		int targetIndex = sourceIndex + sprite.width;
		uint32_t to_move = sprite.get_pixel_data(sourceIndex);
		sprite.set_pixel_data(targetIndex, to_move);
	}

	// Clear out pixels in the new row
	int firstNewPixelIndex = firstMovePixelIndex;
	int firstMovedPixelIndex = firstNewPixelIndex + sprite.width;
	for (int i = firstNewPixelIndex; i < firstMovedPixelIndex; i += 1)
	{
		sprite.set_pixel_data(i, 0x000000);
	}

	sprite.height += 1;

	return 0;
}

int InsertRow(Grid& grid)
{
	int append_result = AppendRowToSpriteC(*grid.sprite, grid.cursor.row_index());
	grid.cursor.move_up();
	return append_result;
}

static int AppendColumnToSpriteC(SpriteC& sprite, int insertAtIndex)
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

	// Check there is space to add a final column
	int currentPixelCount = sprite.width * sprite.height;
	uint64_t currentPixelSpace = currentPixelCount * sizeof(uint32_t);
	uint64_t newPixelSpace = sprite.height * sizeof(uint32_t);
	uint64_t availableSpace = sprite.pixel_memory.sizeInBytes - currentPixelSpace;

	if (availableSpace < newPixelSpace)
	{
		return -1;
	}

	// shift any columns to the right of the new column
	int lastPixelIndex = currentPixelCount - 1;
	for (int sourceIndex = lastPixelIndex; sourceIndex >= 0; sourceIndex -= 1)
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
	int newPixelCount = currentPixelCount + sprite.height;
	// clear pixels in the new column
	for (int i = insertAtIndex; i < newPixelCount; i += sprite.width)
	{
		sprite.set_pixel_data(i, 0x000000);
	}

	return 0;
}

int InsertColumn(Grid& grid)
{
	int selectedColumnIndex = grid.cursor.column_index();
	int selectedRowIndex = grid.cursor.row_index();
	int append_result = AppendColumnToSpriteC(*grid.sprite, selectedColumnIndex);
	int new_cursor_index = grid.cursor.index() + 1 + selectedRowIndex;
	grid.cursor.set_index(new_cursor_index);
	return append_result;
}

static void get_indexes_for_copy(
	const Grid& grid,
	int source_cursor_index,
	int source_range_index,
	int& source_to_target,
	int& source_start_index,
	int& source_end_index,
	int& row_stride,
	int& row_hop
)
{
	bool start_at_cursor = (source_cursor_index < source_range_index);
	source_start_index = (start_at_cursor) ? source_cursor_index : source_range_index;
	source_end_index = (start_at_cursor) ? source_range_index : source_cursor_index;
	int target_start_index = grid.cursor.index();

	source_to_target = target_start_index - source_start_index;

	int source_cursor_col_index = grid.sprite->column_index(source_cursor_index);
	int source_range_col_index = grid.sprite->column_index(source_range_index);
	bool cursor_left_of_range = (source_cursor_col_index < source_range_col_index);
	int source_start_col_index = (cursor_left_of_range) ? source_cursor_col_index : source_range_col_index;
	int source_end_col_index = (cursor_left_of_range) ? source_range_col_index : source_cursor_col_index;

	int target_start_col_index = grid.cursor.column_index();
	int target_end_col_index = target_start_col_index - source_start_col_index + source_end_col_index;
	int max_col_index = grid.sprite->width - 1;

	if (target_end_col_index > max_col_index)
	{
		int columns_out_of_bounds = target_end_col_index - max_col_index;
		source_end_col_index -= columns_out_of_bounds;
		source_end_index -= columns_out_of_bounds;
	}

	int target_end_row_index = grid.sprite->row_index(source_to_target + source_end_index);
	int max_row_index = grid.sprite->height - 1;
	if (target_end_row_index > max_row_index)
	{
		int rows_out_of_bounds = target_end_row_index - max_row_index;
		source_end_index -= (grid.sprite->width * rows_out_of_bounds);
	}

	row_stride = source_end_col_index - source_start_col_index;
	row_hop = source_start_col_index + grid.sprite->width - source_end_col_index;
}

struct clipboard
{
	int start_index;
	int end_index;
	tl::stack_array<uint32_t, 256> pixel_data;
};

static clipboard the_clipboard;

static int copy(const Grid& grid, int source_cursor_index, int source_range_index)
{
	int source_to_target, row_stride, row_hop;
	get_indexes_for_copy(
		grid,
		source_cursor_index,
		source_range_index,
		source_to_target,
		the_clipboard.start_index,
		the_clipboard.end_index,
		row_stride,
		row_hop
	);

	int row_stride_counter = 0;
	int source_index = the_clipboard.start_index;
	// copy
	while (source_index <= the_clipboard.end_index)
	{
		uint32_t to_copy = grid.sprite->get_pixel_data(source_index);
		the_clipboard.pixel_data.append(to_copy); // TODO: check append result

		if (row_stride_counter < row_stride)
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

static int paste(Grid& grid, int source_cursor_index, int source_range_index)
{
	int source_to_target, row_stride, row_hop;
	get_indexes_for_copy(
		grid,
		source_cursor_index,
		source_range_index,
		source_to_target,
		the_clipboard.start_index,
		the_clipboard.end_index,
		row_stride,
		row_hop
	);

	// paste
	int counter = 0;
	int row_stride_counter = 0;
	int source_index = the_clipboard.start_index;

	paste_pixel_data_operation operation = paste_pixel_data_operation(grid.sprite);

	while (source_index <= the_clipboard.end_index)
	{
		uint32_t to_copy = the_clipboard.pixel_data.get_copy(counter).value;
		int target_index = source_index + source_to_target;

		operation.add_pixel(target_index, to_copy);
		counter += 1;

		if (row_stride_counter < row_stride)
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
	the_clipboard.pixel_data.clear();

	operation.execute();

	return 0;
}

void copy_pixels(Grid& grid, int source_cursor_index, int source_range_index)
{
	copy(grid, source_cursor_index, source_range_index);

	paste(grid, source_cursor_index, source_range_index);
}

void cut_pixels(Grid& grid, int source_cursor_index, int source_range_index)
{
	int source_to_target, row_stride, row_hop;
	get_indexes_for_copy(
		grid,
		source_cursor_index,
		source_range_index,
		source_to_target,
		the_clipboard.start_index,
		the_clipboard.end_index,
		row_stride,
		row_hop
	);

	int source_index;
	int row_stride_counter = 0;
	paste_pixel_data_operation clear_source_operation = paste_pixel_data_operation(grid.sprite);
	source_index = the_clipboard.start_index;
	while (source_index <= the_clipboard.end_index)
	{
		uint32_t to_copy = grid.sprite->get_pixel_data(source_index);
		the_clipboard.pixel_data.append(to_copy);
		clear_source_operation.add_pixel(source_index, 0);

		if (row_stride_counter < row_stride)
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
	clear_source_operation.execute();

	paste(grid, source_cursor_index, source_range_index);
}
