#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

#define PALETTE_COUNT 2

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

char* pollenPaletteContent = "\
2\n\
4\n\
115 70 76 255\n\
171 86 117 255\n\
238 106 124 255\n\
255 167 165 255\n\
255 224 126 255\n\
255 231 214 255\n\
114 220 187 255\n\
52 172 186 255";

char* paletteContents[PALETTE_COUNT] = {
	rgrPaletteContent,
	pollenPaletteContent
};

tl::SpriteC palettes[PALETTE_COUNT];
tl::SpriteC rgrPalette;

void InitializePalettes(tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state)
{
	int totalPaletteMemorySize = 512;
	int perPalleteMemorySize = totalPaletteMemorySize / PALETTE_COUNT;
	for (int i = 0; i < PALETTE_COUNT; i += 1)
	{
		tl::MemorySpace paletteSpace = tl::CarveMemorySpace(perPalleteMemorySize, paletteMemory);
		palettes[i].content = (tl::Color*)paletteSpace.content;
		tl::LoadSpriteC(paletteContents[i], tempMemory, palettes[i]);
	}
	state.palette = &palettes[1];
}