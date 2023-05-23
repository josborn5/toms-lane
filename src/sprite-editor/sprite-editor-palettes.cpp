#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

#define PALETTE_COUNT 3

char* rgrPaletteContent = "\
2\n\
8\n\
255 249 179 255\n\
185 197 204 255\n\
71 116 179 255\n\
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
1\n\
8\n\
115 70 76 255\n\
171 86 117 255\n\
238 106 124 255\n\
255 167 165 255\n\
255 224 126 255\n\
255 231 214 255\n\
114 220 187 255\n\
52 172 186 255";

char* sunsetCloudsPaletteContent = "\
1\n\
8\n\
252 176 140 255\n\
239 157 127 255\n\
214 147 138 255\n\
180 141 146 255\n\
165 151 161 255\n\
143 169 191 255\n\
154 171 201 255\n\
165 183 212 255";

char* paletteContents[PALETTE_COUNT] = {
	rgrPaletteContent,
	pollenPaletteContent,
	sunsetCloudsPaletteContent
};

tl::SpriteC palettes[PALETTE_COUNT];
tl::SpriteC rgrPalette;

static void SelectPalette(EditorState& state)
{
	state.palette = &palettes[state.selectedPaletteIndex];
	state.selectedPalettePixelIndex = 0;
	SizePalette(*state.palette);
}

void InitializePalettes(tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state)
{
	for (int i = 0; i < PALETTE_COUNT; i += 1)
	{
		palettes[i].content = (tl::Color*)paletteMemory.content;
		tl::LoadSpriteC(paletteContents[i], tempMemory, palettes[i]);
		uint64_t paletteSize = tl::GetSpriteSpaceInBytes(palettes[i]);
		tl::CarveMemorySpace(paletteSize, paletteMemory);
	}
	SelectPalette(state);
}

void SwitchPalette(EditorState& state)
{
	int nextPaletteIndex = state.selectedPaletteIndex += 1;
	state.selectedPaletteIndex = (nextPaletteIndex >= PALETTE_COUNT) ? 0 : nextPaletteIndex;
	SelectPalette(state);
}