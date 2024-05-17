#include "./editor.hpp"

int GetSelectedRowIndex(const Grid& grid)
{
	if (grid.sprite->height == 1)
	{
		return 0;
	}

	if (grid.sprite->width == 1)
	{
		return grid.selectedIndex;
	}


	return grid.selectedIndex / grid.sprite->width;
}

int GetSelectedColumnIndex(Grid& grid)
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

