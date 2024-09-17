#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"
#include "./transform.hpp"


int SaveBitmap(
	const tl::GameMemory& gameMemory,
	const SpriteC& sprite,
	char* filePath
)
{
	tl::bitmap blankBitmap;
	tl::MemorySpace remainder;
	remainder.content = gameMemory.transient.content;
	remainder.sizeInBytes = gameMemory.transient.sizeInBytes;
	int bitmapWriteResult = InitializeBitmapFromSpriteC(sprite, blankBitmap, remainder);
	if (bitmapWriteResult != 0) return -1;

	tl::CarveMemorySpace(blankBitmap.file_header.fileSizeInBytes, remainder);

	int writeResult = tl::bitmap_interface_write(blankBitmap, remainder);
	if (writeResult != tl::bitmap_write_success) return -2;

	tl::MemorySpace fileData = CarveMemorySpace(blankBitmap.file_header.fileSizeInBytes, remainder);

	return tl::file_interface_write(filePath, fileData);
}

static int AppendRowToSpriteC(SpriteC& sprite, tl::MemorySpace spriteMemory, int insertAtIndex)
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
	uint64_t currentPixelSpace = currentPixelCount * sizeof(Color);
	uint64_t newPixelSpace = sprite.width * sizeof(Color);
	uint64_t availableSpace = spriteMemory.sizeInBytes - currentPixelSpace;

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
		sprite.content[targetIndex] = sprite.content[sourceIndex];
	}

	// Clear out pixels in the new row
	int firstNewPixelIndex = firstMovePixelIndex;
	int firstMovedPixelIndex = firstNewPixelIndex + sprite.width;
	for (int i = firstNewPixelIndex; i < firstMovedPixelIndex; i += 1)
	{
		Color pixel;
		pixel.r = 0.0f;
		pixel.g = 0.0f;
		pixel.b = 0.0f;
		pixel.a = 0.0f;
		sprite.content[i] = pixel;
	}

	sprite.height += 1;

	return 0;
}

int InsertRow(Grid& grid, tl::MemorySpace spriteMemory)
{
	int selectedRowIndex = GetSelectedRowIndex(grid);

	grid.selectedIndex += grid.sprite->width;

	return AppendRowToSpriteC(*grid.sprite, spriteMemory, selectedRowIndex);
}

static int AppendColumnToSpriteC(SpriteC& sprite, tl::MemorySpace spriteMemory, int insertAtIndex)
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
	uint64_t currentPixelSpace = currentPixelCount * sizeof(Color);
	uint64_t newPixelSpace = sprite.height * sizeof(Color);
	uint64_t availableSpace = spriteMemory.sizeInBytes - currentPixelSpace;

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
		sprite.content[targetIndex] = sprite.content[sourceIndex];
	}

	sprite.width += 1;
	int newPixelCount = currentPixelCount + sprite.height;
	// clear pixels in the new column
	for (int i = insertAtIndex; i < newPixelCount; i += sprite.width)
	{
		sprite.content[i].r = 0.0f;
		sprite.content[i].g = 0.0f;
		sprite.content[i].b = 0.0f;
		sprite.content[i].a = 0.0f;
	}

	return 0;
}

int InsertColumn(Grid& grid, tl::MemorySpace spriteMemory)
{
	int selectedColumnIndex = GetSelectedColumnIndex(grid);
	int selectedRowIndex = GetSelectedRowIndex(grid);
	grid.selectedIndex += (1 + selectedRowIndex);

	return AppendColumnToSpriteC(*grid.sprite, spriteMemory, selectedColumnIndex);
}

