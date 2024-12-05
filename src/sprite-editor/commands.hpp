#ifndef SPRITE_EDITOR_COMMANDS_HPP
#define SPRITE_EDITOR_COMMANDS_HPP

#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

int SaveBitmap(
	const tl::MemorySpace& tempMemory,
	const SpriteC& sprite,
	char* filePath
);
int InsertRow(Grid& grid);
int InsertColumn(Grid& grid);

void copy_pixels(Grid& grid, int source_index, int source_range);
void cut_pixels(Grid& grid, int source_index, int source_range);

#endif

