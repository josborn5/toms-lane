#include "./editor.hpp"

int GetRowIndex(const Grid& grid, int index)
{
	if (grid.sprite->height == 1)
	{
		return 0;
	}

	if (grid.sprite->width == 1)
	{
		return index;
	}

	return index / grid.sprite->width;
}

int GetColumnIndex(const Grid& grid, int index)
{
	if (grid.sprite->width == 1)
	{
		return 0;
	}

	if (grid.sprite->height == 1)
	{
		return index;
	}

	return index % grid.sprite->width;

}

int GetSelectedRowIndex(const Grid& grid)
{
	return GetRowIndex(grid, grid.selectedIndex);
}

int GetSelectedColumnIndex(Grid& grid)
{
	return GetColumnIndex(grid, grid.selectedIndex);
}

char* ParseColorFromCharArray(char* content, tl::MemorySpace& space, Color& color)
{
	char* buffer = (char*)space.content;
	char* workingPointer = content;

	/// RBGA values
	int rgbaContent[4] = { 0, 0, 0, 255 }; // Default alpha to 100%

	for (int i = 0; i < 4 && *workingPointer; i += 1)
	{
		workingPointer = tl::GetNextNumberChar(workingPointer);
		if (*workingPointer)
		{
			workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
			rgbaContent[i] = tl::CharStringToInt(buffer);
		}
	}

	color.r = (float)rgbaContent[0] / 255.0f;
	color.g = (float)rgbaContent[1] / 255.0f;
	color.b = (float)rgbaContent[2] / 255.0f;
	color.a = (float)rgbaContent[3] / 255.0f;

	return workingPointer;
}



