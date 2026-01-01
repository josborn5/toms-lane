#ifndef SPRITE_EDITOR_PALETTES_HPP
#define SPRITE_EDITOR_PALETTES_HPP

#include "../../application/src/tl-application.hpp"
#include "./editor.hpp"

void InitializePalettes(tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state);
void SwitchPalette(EditorState& state);

#endif
