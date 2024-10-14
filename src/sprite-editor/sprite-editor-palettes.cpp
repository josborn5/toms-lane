#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./state.hpp"

static const int PALETTE_COUNT = 5;
static int availablePaletteCount = 0;

static uint64_t GetSpriteSpaceInBytes(const SpriteC& sprite)
{
	int pixelCount = sprite.width * sprite.height;
	return sizeof(Color) * pixelCount;
}

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

char* fantasyConsolePaletteContent = "\
2\n\
8\n\
  0   0   0 255\n\
255 252 255 255\n\
230 255 242 255\n\
178 183 225 255\n\
 89  91 125 255\n\
123 138 198 255\n\
255 217 244 255\n\
225 173 195 255\n\
173 128 166 255\n\
251 162 215 255\n\
250 224 199 255\n\
240 171 171 255\n\
151 196 170 255\n\
191 237 245 255\n\
115 201 235 255\n\
202 175 245 255";

char* defaultPaletteContent = "\
1\n\
2\n\
  0   0   0 255\n\
255 255 255 255";


char* paletteContents[PALETTE_COUNT] = {
	rgrPaletteContent,
	pollenPaletteContent,
	sunsetCloudsPaletteContent,
	fantasyConsolePaletteContent,
	defaultPaletteContent
};

static SpriteC palettes[PALETTE_COUNT];
static SpriteC rgrPalette;
static int selectedPaletteIndex = 0;

static void SelectPalette(EditorState& state)
{
	state.palette_.sprite = &palettes[selectedPaletteIndex];
	state.palette_.selectedIndex = 0;
	SizeGrid(state.palette_);
}

void InitializePalettes(tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state)
{
	int maxColorsPerPalette = state.pixels.sprite->bitsPerPixel != 1 ? -1 : 2;

	for (int i = 0; i < PALETTE_COUNT; i += 1)
	{
		palettes[i].content = (Color*)paletteMemory.content;
		LoadSpriteC(paletteContents[i], tempMemory, palettes[i]);
		uint64_t paletteSize = GetSpriteSpaceInBytes(palettes[i]);
		tl::CarveMemorySpace(paletteSize, paletteMemory);

		int palettePixelCount = palettes[i].width * palettes[i].height;
		if (palettePixelCount <= maxColorsPerPalette)
		{
			availablePaletteCount += 1;
		}
	}
	SelectPalette(state);
}

void SwitchPalette(EditorState& state)
{
	int nextPaletteIndex = selectedPaletteIndex += 1;
	selectedPaletteIndex = (nextPaletteIndex >= PALETTE_COUNT) ? 0 : nextPaletteIndex;
	SelectPalette(state);
}
