#include "./editor.hpp"

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

int GetSelectedColumnIndex(Grid& grid)
{
	return GetColumnIndex(grid, grid.selectedIndex);
}

char* ParseColorFromCharArray(char* content, tl::MemorySpace& space, uint32_t& color)
{
	char* buffer = (char*)space.content;
	char* workingPointer = content;

	/// RBGA values
	int rgbContent[3] = { 0, 0, 0 };

	for (int i = 0; i < 3 && *workingPointer; i += 1)
	{
		workingPointer = tl::GetNextNumberChar(workingPointer);
		if (*workingPointer)
		{
			workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
			rgbContent[i] = tl::CharStringToInt(buffer);
		}
	}

	color = tl::GetColorFromRGB(rgbContent[0], rgbContent[1], rgbContent[2]);
	return workingPointer;
}

