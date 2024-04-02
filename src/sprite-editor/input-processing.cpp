#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"

#define COMMAND_BUFFER_SIZE 15
#define DISPLAY_BUFFER_SIZE 15

static bool hasCopied = false;
static tl::MemorySpace spriteMemory;
static tl::MemorySpace paletteMemory;
static tl::Color currentColor;
static tl::Color copiedColor;

tl::GameMemory appMemory;

EditorState state;
static char commandBuffer[COMMAND_BUFFER_SIZE];
static char displayBuffer[DISPLAY_BUFFER_SIZE];
tl::array<char> commands = tl::array<char>(commandBuffer, COMMAND_BUFFER_SIZE);
tl::array<char> display = tl::array<char>(displayBuffer, DISPLAY_BUFFER_SIZE);


static void MoveCursorForSprite(const tl::Input &input, const tl::SpriteC& sprite, int& selectedPixelIndex)
{
	int maxPixelIndex = (sprite.width * sprite.height) - 1;
	if (!input.buttons[tl::KEY_CTRL].isDown)
	{
		if (tl::IsReleased(input, tl::KEY_RIGHT))
		{
			if (selectedPixelIndex < maxPixelIndex)
			{
				selectedPixelIndex += 1;
			}
		}
		else if (tl::IsReleased(input, tl::KEY_LEFT))
		{
			if (selectedPixelIndex > 0)
			{
				selectedPixelIndex -= 1;
			}
		}
		else if (tl::IsReleased(input, tl::KEY_DOWN))
		{
			int provisionalSelectedPixelIndex = selectedPixelIndex + sprite.width;
			if (provisionalSelectedPixelIndex <= maxPixelIndex)
			{
				selectedPixelIndex = provisionalSelectedPixelIndex;
			}
		}
		else if (tl::IsReleased(input, tl::KEY_UP))
		{
			int provisionalSelectedPixelIndex = selectedPixelIndex - sprite.width;
			if (provisionalSelectedPixelIndex >= 0)
			{
				selectedPixelIndex = provisionalSelectedPixelIndex;
			}
		}
	}
}

void ProcessCursorMovementInput(const tl::Input &input)
{
	tl::SpriteC& activeSprite = (state.activeControl == SpriteGrid) ? state.sprite : *state.palette;
	int& activeIndex = (state.activeControl == SpriteGrid) ? state.selectedPixelIndex : state.selectedPalettePixelIndex;
	MoveCursorForSprite(input, activeSprite, activeIndex);
	currentColor = state.palette->content[state.selectedPalettePixelIndex];
}

void ProcessActiveControl(const tl::Input &input)
{
	if (!input.buttons[tl::KEY_CTRL].isDown)
	{
		if (tl::IsReleased(input, tl::KEY_TAB))
		{
			int nextActiveControlIndex = state.activeControl + 1;
			state.activeControl = (nextActiveControlIndex < EditorControlCount) ? (EditorControl)nextActiveControlIndex : SpriteGrid;
		}
	}
}


static char alphaKeyMap[26] = {
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z'
};

static char digitKeyMap[10] = {
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9'
};

static char GetCharForAlphaKey(int key)
{
	if (key < tl::KEY_A || key > tl::KEY_Z)
	{
		return '\0';
	}
	int relativeIndex = key - tl::KEY_A;
	return alphaKeyMap[relativeIndex];
}

static char GetCharForDigitKey(int key)
{
	if (key < tl::KEY_0 || key > tl::KEY_9)
	{
		return '\0';
	}
	int relativeIndex = key - tl::KEY_0;
	return digitKeyMap[relativeIndex];
}

void ClearCommandBuffer()
{
	for (int i = 0; i < commands.capacity(); i += 1)
	{
		commands.access(i) = '\0';
		commands.clear();
	}
}

void ClearDisplayBuffer()
{
	for (int i = 0; i < display.capacity(); i += 1)
	{
		display.access(i) = '\0';
		display.clear();
	}
}

int Initialize(const tl::GameMemory& gameMemory)
{
	state.commandBuffer = &commands;
	state.displayBuffer = &display;
	state.windowWidth = 800;
	state.windowHeight = 600;

	// Define memory slices
	tl::MemorySpace perm = gameMemory.permanent;
	const uint64_t oneKiloByteInBytes = 1024;
	const uint64_t oneMegaByteInBytes = oneKiloByteInBytes * 1024;
	paletteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, perm);
	spriteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, perm);
	tl::MemorySpace temp = gameMemory.transient;
	tl::MemorySpace fileReadMemory = tl::CarveMemorySpace(oneMegaByteInBytes, temp);
	tl::MemorySpace tempMemory = tl::CarveMemorySpace(oneMegaByteInBytes, temp);

	// Load file
	if (state.filePath)
	{
		uint64_t fileSize = 0;
		if (tl::file_interface_size_get(state.filePath, fileSize) != tl::Success)
		{
			return 1;
		}

		if (tl::file_interface_read(state.filePath, fileReadMemory) != tl::Success)
		{
			return 1;
		}
	}
	else
	{
		// Initialize default sprite
		fileReadMemory.content = "2\n2\n0 0 0 0\n0 0 0 0\n0 0 0 0\n0 0 0 0";
	}

	ClearCommandBuffer();
	ClearDisplayBuffer();

	InitializeLayout(state);
	InitializePalettes(paletteMemory, tempMemory, state);

	char* spriteCharArray = (char*)fileReadMemory.content;
	state.sprite.content = (tl::Color*)spriteMemory.content;
	tl::LoadSpriteC(spriteCharArray, tempMemory, state.sprite);

	SizeGridForSprite(state.sprite);
	return 0;
}


void ProcessKeyboardInput(const tl::Input& input)
{
	if (!input.buttons[tl::KEY_CTRL].isDown)
	{
		// Update command buffer from input
		if (commands.length() < commands.capacity())
		{
			for (int key = tl::KEY_A; key <= tl::KEY_Z; key += 1)
			{
				if (tl::IsReleased(input, key))
				{
					char commandChar = GetCharForAlphaKey(key);
					commands.append(commandChar);
				}
			}

			for (int key = tl::KEY_0; key <= tl::KEY_9; key += 1)
			{
				if (tl::IsReleased(input, key))
				{
					char commandChar = GetCharForDigitKey(key);
					commands.append(commandChar);
				}
			}

			if (tl::IsReleased(input, tl::KEY_SPACE))
			{
				commands.append(' ');
			}
		}
		if (tl::IsReleased(input, tl::KEY_ESCAPE))
		{
			ClearCommandBuffer();
		}
		else if (tl::IsReleased(input, tl::KEY_ENTER))
		{
			switch (commands.get(0))
			{
				case 'S': // save
				{
					char* displayString = &display.access(0);
					if (commands.get(1) == '\0') // save to current filePath
					{
						Save(appMemory, state.sprite, displayString, state);
					}
					else if (commands.get(1) == ' ' && commands.get(2)) // save to new filePath
					{
						state.filePath = &commands.access(2);
						Save(appMemory, state.sprite, displayString, state);
					}
					break;
				}
				case 'R': // append row
				{
					if (commands.get(1) == '\0')
					{
						AppendRowToSpriteC(state.sprite, spriteMemory); // TODO: make the MemorySpace a field of the SpriteC struct. The pointer to the sprite content is shared between the two - make it a single pointer owner!
						SizeGridForSprite(state.sprite);
					}
					break;
				}
				case 'C': // append column
				{
					if (commands.get(1) == '\0')
					{
						AppendColumnToSpriteC(state.sprite, spriteMemory);
						SizeGridForSprite(state.sprite);
					}
					break;
				}
				case 'E': // edit color of selected pixel
				{
					char* pointer = tl::GetNextNumberChar(&commands.access(1));
					tl::MemorySpace transient = appMemory.transient;
					ParseColorFromCharArray(pointer, transient, state.sprite.content[state.selectedPixelIndex]);
					ClearCommandBuffer();
					break;
				}
				case 'I': // inspect color of selected pixel
				{
					if (commands.get(1) == '\0')
					{
						ClearDisplayBuffer();
						tl::Color selectedColor = state.sprite.content[state.selectedPixelIndex];

						int color = (int)(selectedColor.r * 255.0f);
						char* cursor = &display.access(0);
						tl::IntToCharString(color, cursor);

						while (*cursor) cursor++;
						*cursor = ' ';
						cursor++;

						color = (int)(selectedColor.g * 255.0f);
						tl::IntToCharString(color, cursor);

						while (*cursor) cursor++;
						*cursor = ' ';
						cursor++;

						color = (int)(selectedColor.b * 255.0f);
						tl::IntToCharString(color, cursor);
					}
					break;
				}
				case 'P': // switch palette
				{
					if (commands.get(1) == '\0')
					{
						SwitchPalette(state);
					}
					break;
				}
				case '\0': // Apply active color from palette
				{
					if (state.activeControl == SpriteGrid)
					{
						state.sprite.content[state.selectedPixelIndex] = currentColor;
					}
					break;
				}
				case 'D': // Delete
				{
					if (commands.get(1) == 'R' && commands.get(2) == '\0' && state.sprite.height > 1)
					{
						// Get start and end index of row
						int rowIndex = (int)(state.selectedPixelIndex / state.sprite.width);
						unsigned int startDeleteIndex = rowIndex * state.sprite.width;
						unsigned int endDeleteIndex = rowIndex + state.sprite.width - 1;
						unsigned int spriteLength = state.sprite.width * state.sprite.height;

						// Call tl::DeleteFromArray with the sprite content
						tl::DeleteFromArray(state.sprite.content, startDeleteIndex, endDeleteIndex, spriteLength);

						// Subtract 1 from the sprite height
						state.sprite.height -= 1;

						SizeGridForSprite(state.sprite);
					}
					else if (commands.get(1) == 'C' && commands.get(2) == '\0' && state.sprite.width > 1)
					{
						// get the column index
						unsigned int columnIndex = state.selectedPixelIndex % state.sprite.width;
						unsigned int spriteLength = state.sprite.width * state.sprite.height;
						for (int i = state.sprite.height - 1; i >= 0; i -= 1)
						{
							unsigned int deleteIndex = (i * state.sprite.width) + columnIndex;
							tl::DeleteFromArray(state.sprite.content, deleteIndex, deleteIndex, spriteLength);
							spriteLength -= 1;
						}
						state.sprite.width -= 1;

						SizeGridForSprite(state.sprite);
					}
					break;
				}
			}
			ClearCommandBuffer();
		}
	}
	else
	{
		if (tl::IsReleased(input, tl::KEY_C))
		{
			hasCopied = true;
			copiedColor = state.sprite.content[state.selectedPixelIndex];
			ClearDisplayBuffer();
			display.append('C');
			display.append('O');
			display.append('P');
			display.append('Y');
		}
		else if (hasCopied && tl::IsReleased(input, tl::KEY_V))
		{
			state.sprite.content[state.selectedPixelIndex] = copiedColor;
			ClearDisplayBuffer();
			display.append('P');
			display.append('A');
			display.append('S');
			display.append('T');
			display.append('E');
		}
	}
}

int InitializeState(char* commandLine)
{
	if (*commandLine)
	{
		state.filePath = commandLine;
	}

	tl::InitializeMemory(
		2,
		2,
		appMemory
	);

	return Initialize(appMemory);
}

const EditorState& GetLatestState(const tl::Input& input)
{
	ProcessActiveControl(input);
	ProcessCursorMovementInput(input);

	state.mouse.x = input.mouse.x;
	state.mouse.y = input.mouse.y;

	ProcessKeyboardInput(input);

	return state;
}
