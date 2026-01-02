#ifndef SPRITE_EDITOR_PALETTES_HPP
#define SPRITE_EDITOR_PALETTES_HPP

#include "../../platform/src/tl-library.hpp"
#include "./editor.hpp"

void InitializePalettes(tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state);
void SwitchPalette(EditorState& state);

#endif
