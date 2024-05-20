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

