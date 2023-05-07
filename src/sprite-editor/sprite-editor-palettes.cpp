#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

char* rgrPaletteContent = "\
2\n\
8\n\
255 249 179 255\n\
185 197 204 255\n\
71 116 179 255 \n\
20 75 102 255\n\
143 179 71 255\n\
46 153 78 255\n\
242 144 102 255\n\
230 80 80 255\n\
112 125 124 255\n\
41 60 64 255\n\
23 11 26 255\n\
10 1 13 255\n\
87 9 50 255\n\
135 30 46 355\n\
255 191 64 255\n\
204 20 36 255";

tl::SpriteC rgrPalette;

void InitializePalettes(const tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state)
{
	rgrPalette.content = (tl::Color*)paletteMemory.content;
	tl::LoadSpriteC(rgrPaletteContent, tempMemory, rgrPalette);
	state.palette = &rgrPalette;
}