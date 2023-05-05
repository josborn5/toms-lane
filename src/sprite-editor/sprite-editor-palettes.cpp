#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

char* rgrPaletteContent = "\
1\n\
4\n\
255 249 179 255\n\
185 197 204 255\n\
71 116 179 255 \n\
20 75 102 255";

tl::SpriteC rgrPalette;

void InitializePalettes(const tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state)
{
	rgrPalette.content = (tl::Color*)paletteMemory.content;
	tl::LoadSpriteC(rgrPaletteContent, tempMemory, rgrPalette);
	state.palette = rgrPalette;
}