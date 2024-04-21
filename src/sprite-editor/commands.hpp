#ifndef SPRITE_EDITOR_COMMANDS_HPP
#define SPRITE_EDITOR_COMMANDS_HPP

#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

void Save(
	const tl::GameMemory& gameMemory,
	const tl::SpriteC& sprite,
	tl::array<char>& displayBuffer,
	char* filePath
);
int InsertRow(Grid& grid, tl::MemorySpace spriteMemory);
int InsertColumn(Grid& grid, tl::MemorySpace spriteMemory);

#endif

