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
int InsertRow(EditorState& state, tl::MemorySpace spriteMemory);
int AppendColumnToSpriteC(tl::SpriteC& sprite, tl::MemorySpace spriteMemory, int insertAtIndex);

#endif

