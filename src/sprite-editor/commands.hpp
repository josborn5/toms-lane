#ifndef SPRITE_EDITOR_COMMANDS_HPP
#define SPRITE_EDITOR_COMMANDS_HPP

#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

int SaveBitmap(
	const tl::GameMemory& gameMemory,
	const SpriteC& sprite,
	char* filePath
);
int InsertRow(Grid& grid, tl::MemorySpace spriteMemory);
int InsertColumn(Grid& grid, tl::MemorySpace spriteMemory);

#endif

