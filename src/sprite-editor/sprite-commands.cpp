#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

// Returns the number of chars
static int SpriteCToCharString(const tl::SpriteC& sprite, tl::MemorySpace memory)
{
	int charCount = 0;
	int pixelCount = sprite.height * sprite.width;
	int dimensionCount = 20; // 10 chars per x & y size

	int charCountPerPixel = 4 * 4; // 3 number chars + 1 space char per R, G, B & A value for each pixel
	int totalCharCount = dimensionCount + (pixelCount * charCountPerPixel);

	if (memory.sizeInBytes < (unsigned long)totalCharCount)
	{
		return charCount;
	}

	char* cursor = (char*)memory.content;

	// Width
	tl::IntToCharString(sprite.width, cursor);
	while (*cursor)
	{
		cursor++;
		charCount += 1;
	}
	*cursor = '\n';
	cursor++;
	charCount += 1;

	// Height
	tl::IntToCharString(sprite.height, cursor);
	while (*cursor)
	{
		cursor++;
		charCount += 1;
	}
	*cursor = '\n';
	cursor++;
	charCount += 1;

	for (int i = 0; i < pixelCount; i += 1)
	{
		tl::Color color = sprite.content[i];
		tl::IntToCharString((int)(color.r * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = ' ';
		cursor++;
		charCount += 1;

		tl::IntToCharString((int)(color.g * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = ' ';
		cursor++;
		charCount += 1;

		tl::IntToCharString((int)(color.b * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = ' ';
		cursor++;
		charCount += 1;

		tl::IntToCharString((int)(color.a * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = '\n';
		cursor++;
		charCount += 1;
	}
	*cursor = '\0';
	return charCount;
}

void Save(
	const tl::GameMemory& gameMemory,
	const tl::SpriteC& sprite,
	tl::array<char>& displayBuffer,
	char* filePath
) {
	// Serialize to string
	int charCount = SpriteCToCharString(sprite, gameMemory.transient);
	tl::MemorySpace toSaveToFile;
	toSaveToFile.content = gameMemory.transient.content;
	toSaveToFile.sizeInBytes = charCount * sizeof(char);
	displayBuffer.clear();
	if (tl::file_interface_write(filePath, toSaveToFile) == tl::Success)
	{
		displayBuffer.append('S');
		displayBuffer.append('A');
		displayBuffer.append('V');
		displayBuffer.append('E');
		displayBuffer.append('D');
		displayBuffer.append('\0');
	}
	else
	{
		displayBuffer.append('E');
		displayBuffer.append('R');
		displayBuffer.append('R');
		displayBuffer.append('O');
		displayBuffer.append('R');
		displayBuffer.append('\0');
	}
}

static int AppendRowToSpriteC(tl::SpriteC& sprite, tl::MemorySpace spriteMemory, int insertAtIndex)
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
	uint64_t currentPixelSpace = currentPixelCount * sizeof(tl::Color);
	uint64_t newPixelSpace = sprite.width * sizeof(tl::Color);
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
		tl::Color pixel;
		pixel.r = 0.0f;
		pixel.g = 0.0f;
		pixel.b = 0.0f;
		pixel.a = 0.0f;
		sprite.content[i] = pixel;
	}

	sprite.height += 1;

	return 0;
}

static int GetSelectedRowIndex(const Grid& grid)
{
	if (grid.sprite->height == 1)
	{
		return 0;
	}

	if (grid.sprite->width == 1)
	{
		return grid.selectedIndex;
	}


	return grid.selectedIndex / grid.sprite->height;
}


int InsertRow(Grid& grid, tl::MemorySpace spriteMemory)
{
	int selectedRowIndex = GetSelectedRowIndex(grid);

	grid.selectedIndex += grid.sprite->width;

	return AppendRowToSpriteC(*grid.sprite, spriteMemory, selectedRowIndex);
}

static int AppendColumnToSpriteC(tl::SpriteC& sprite, tl::MemorySpace spriteMemory, int insertAtIndex)
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
	uint64_t currentPixelSpace = currentPixelCount * sizeof(tl::Color);
	uint64_t newPixelSpace = sprite.height * sizeof(tl::Color);
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

static int GetSelectedColumnIndex(Grid& grid)
{
	if (grid.sprite->width == 1)
	{
		return 0;
	}

	if (grid.sprite->height == 1)
	{
		return grid.selectedIndex;
	}

	return grid.selectedIndex % grid.sprite->width;
}

int InsertColumn(Grid& grid, tl::MemorySpace spriteMemory)
{
	int selectedColumnIndex = GetSelectedColumnIndex(grid);
	int selectedRowIndex = GetSelectedRowIndex(grid);
	grid.selectedIndex += (1 + selectedRowIndex);

	return AppendColumnToSpriteC(*grid.sprite, spriteMemory, selectedColumnIndex);
}

