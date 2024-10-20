#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"
#include "./render.hpp"

static const int PALETTE_COUNT = 5;

char* rgrPaletteContent = "\
2\n\
8\n\
255 249 179\n\
185 197 204\n\
 71 116 179\n\
 20  75 102\n\
143 179  71\n\
 46 153  78\n\
242 144 102\n\
230  80  80\n\
112 125 124\n\
 41  60  64\n\
 23  11  26\n\
 10   1  13\n\
 87   9  50\n\
135  30  46\n\
255 191  64\n\
204  20  36";

char* pollenPaletteContent = "\
1\n\
8\n\
115  70  76\n\
171  86 117\n\
238 106 124\n\
255 167 165\n\
255 224 126\n\
255 231 214\n\
114 220 187\n\
 52 172 186";

char* sunsetCloudsPaletteContent = "\
1\n\
8\n\
252 176 140\n\
239 157 127\n\
214 147 138\n\
180 141 146\n\
165 151 161\n\
143 169 191\n\
154 171 201\n\
165 183 212";

char* fantasyConsolePaletteContent = "\
2\n\
8\n\
  0   0   0\n\
255 252 255\n\
230 255 242\n\
178 183 225\n\
 89  91 125\n\
123 138 198\n\
255 217 244\n\
225 173 195\n\
173 128 166\n\
251 162 215\n\
250 224 199\n\
240 171 171\n\
151 196 170\n\
191 237 245\n\
115 201 235\n\
202 175 245";

char* defaultPaletteContent = "\
1\n\
2\n\
  0   0   0\n\
255 255 255";


char* paletteContents[PALETTE_COUNT] = {
	rgrPaletteContent,
	pollenPaletteContent,
	sunsetCloudsPaletteContent,
	fantasyConsolePaletteContent,
	defaultPaletteContent
};

static SpriteC palettes[PALETTE_COUNT];
static tl::stack_array<SpriteC*, PALETTE_COUNT> available_palettes;
static SpriteC rgrPalette;
static int selectedPaletteIndex = 0;

/*
* Assumed char* format is:
* width<int>\n
* height<int>\n
* RValue<char> GValue<char> BValue<char>\n // 1st pixel
* :
* RValue<char> GValue<char> BValue<char>\n // Nth pixel
*/
static void sprite_from_string_read_dimensions(char* content, tl::MemorySpace& space, SpriteC& sprite)
{
	char* buffer = (char*)space.content;
	// Width
	char* workingPointer = tl::GetNextNumberChar(content);
	workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
	int width = tl::CharStringToInt(buffer);

	// Height
	workingPointer = tl::GetNextNumberChar(workingPointer);
	workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
	int height = tl::CharStringToInt(buffer);

	sprite.width = width;
	sprite.height = height;
}

static void LoadSpriteC(char* content, tl::MemorySpace& space, SpriteC& sprite)
{
	char* buffer = (char*)space.content;
	// Width
	char* workingPointer = tl::GetNextNumberChar(content);
	workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
	int width = tl::CharStringToInt(buffer);

	// Height
	workingPointer = tl::GetNextNumberChar(workingPointer);
	workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
	int height = tl::CharStringToInt(buffer);

	// Content
	int contentCount = height * width;

	sprite.width = width;
	sprite.height = height;
	sprite.bitsPerPixel = 24;

	for (int i = 0; i < contentCount && *workingPointer; i += 1)
	{
		workingPointer = ParseColorFromCharArray(workingPointer, space, sprite.pixels()[i]);
	}
}




static void SelectPalette(EditorState& state)
{
	state.palette_.sprite = available_palettes.get_copy(selectedPaletteIndex);
	state.palette_.selectedIndex = 0;
	SizeGrid(state.palette_);
}

void InitializePalettes(tl::MemorySpace& paletteMemory, tl::MemorySpace& tempMemory, EditorState& state)
{
	selectedPaletteIndex = 0;
	bool paletteSizeLimit = (state.pixels.sprite->bitsPerPixel == 1);
	int maxColorsPerPalette = 2;
	available_palettes.clear();

	for (int i = 0; i < PALETTE_COUNT; i += 1)
	{
		sprite_from_string_read_dimensions(paletteContents[i], tempMemory, palettes[i]);
		int palette_pixel_count = palettes[i].height * palettes[i].width;
		uint64_t pixel_size_in_bytes = sizeof(uint32_t) * palette_pixel_count;
		uint64_t color_size_in_bytes = sizeof(Color) * palette_pixel_count;
		uint64_t palette_size_in_bytes = pixel_size_in_bytes + color_size_in_bytes;
		tl::MemorySpace palette_allocation = tl::CarveMemorySpace(palette_size_in_bytes, paletteMemory);

		palettes[i].pixel_memory = palette_allocation;

		LoadSpriteC(paletteContents[i], tempMemory, palettes[i]);

		if (!paletteSizeLimit || palette_pixel_count <= maxColorsPerPalette)
		{
			available_palettes.append(&palettes[i]);
		}
	}
	SelectPalette(state);
}

void SwitchPalette(EditorState& state)
{
	int nextPaletteIndex = selectedPaletteIndex += 1;
	selectedPaletteIndex = (nextPaletteIndex >= available_palettes.length()) ? 0 : nextPaletteIndex;
	SelectPalette(state);
}
