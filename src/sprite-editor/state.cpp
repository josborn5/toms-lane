#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./palettes.hpp"
#include "./commands.hpp"

static const int commandBufferSize = 256;
static const int modeBufferSize = 2;


static bool hasCopied = false;
static tl::MemorySpace spriteMemory;
static tl::MemorySpace paletteMemory;
static tl::Color currentColor;
static tl::Color copiedColor;
static char* filePath;
static tl::SpriteC currentSprite;

tl::GameMemory appMemory;

EditorState state;
static char commandBuffer[commandBufferSize];
static tl::array<char> commands = tl::array<char>(commandBuffer, commandBufferSize);

static int CompareColor(const tl::Color& color1, const tl::Color& color2)
{
	if (color1.r == color2.r &&
		color1.g == color2.g &&
		color1.b == color2.b &&
		color1.a == color2.a)
	{
		return 0;
	}
	return -1;
}

static void MoveCursorForSprite(const tl::Input &input, Grid& grid)
{
	int maxPixelIndex = (grid.sprite->width * grid.sprite->height) - 1;
	if (input.buttons[tl::KEY_CTRL].isDown)
	{
		if (input.buttons[tl::KEY_HOME].keyDown)
		{
			grid.selectedIndex = 0;
			return;
		}

		if (input.buttons[tl::KEY_END].keyDown)
		{
			grid.selectedIndex = (grid.sprite->height * grid.sprite->width) - 1;
			return;
		}

		if (input.buttons[tl::KEY_LEFT].keyDown)
		{
			tl::Color activeColor = grid.sprite->content[grid.selectedIndex];
			int pixelIndex = grid.selectedIndex;
			bool sameColor = true;
			while (pixelIndex > 0 && sameColor)
			{
				pixelIndex -= 1;
				sameColor = (CompareColor(activeColor, grid.sprite->content[pixelIndex]) == 0);
			}
			grid.selectedIndex = pixelIndex;
		}
		return;
	}

	if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		if (grid.selectedIndex < maxPixelIndex)
		{
			grid.selectedIndex += 1;
		}
	}
	else if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		if (grid.selectedIndex > 0)
		{
			grid.selectedIndex -= 1;
		}
	}
	else if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		int provisionalSelectedPixelIndex = grid.selectedIndex + grid.sprite->width;
		if (provisionalSelectedPixelIndex <= maxPixelIndex)
		{
			grid.selectedIndex = provisionalSelectedPixelIndex;
		}
	}
	else if (input.buttons[tl::KEY_UP].keyDown)
	{
		int provisionalSelectedPixelIndex = grid.selectedIndex - grid.sprite->width;
		if (provisionalSelectedPixelIndex >= 0)
		{
			grid.selectedIndex = provisionalSelectedPixelIndex;
		}
	}
}

static void ProcessCursorMovementInput(const tl::Input &input)
{
	Grid& activeGrid = (state.activeControl == SpriteGrid) ? state.pixels : state.palette_;
	MoveCursorForSprite(input, activeGrid);

	currentColor = state.palette_.sprite->content[state.palette_.selectedIndex];
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

static void ClearCommandBuffer()
{
	for (int i = 0; i < commands.capacity(); i += 1)
	{
		commands.access(i) = '\0';
		commands.clear();
	}
}


int Initialize(const tl::GameMemory& gameMemory)
{
	state.mode = View;
	state.commandBuffer = &commandBuffer[0];
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
	if (filePath)
	{
		uint64_t fileSize = 0;
		if (tl::file_interface_size_get(filePath, fileSize) != tl::Success)
		{
			return 1;
		}

		if (tl::file_interface_read(filePath, fileReadMemory) != tl::Success)
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

	InitializeLayout(state);
	InitializePalettes(paletteMemory, tempMemory, state);

	char* spriteCharArray = (char*)fileReadMemory.content;
	currentSprite.content = (tl::Color*)spriteMemory.content;
	tl::LoadSpriteC(spriteCharArray, tempMemory, currentSprite);

	state.pixels.sprite = &currentSprite;

	SizeGrid(state.pixels);
	return 0;
}


static void ExecuteCurrentCommand()
{
	switch (commands.get(0))
	{
		case 'S': // save
		{
			if (commands.get(1) == '\0') // save to current filePath
			{
				Save(appMemory, *state.pixels.sprite, commands, filePath);
			}
			else if (commands.get(1) == ' ' && commands.get(2)) // save to new filePath
			{
				filePath = &commands.access(2);
				Save(appMemory, *state.pixels.sprite, commands, filePath);
			}
			break;
		}
		case 'R': // append row
		{
			if (commands.get(1) == '\0')
			{
				InsertRow(state.pixels, spriteMemory);
				SizeGrid(state.pixels);
			}
			break;
		}
		case 'C': // append column
		{
			if (commands.get(1) == '\0')
			{
				InsertColumn(state.pixels, spriteMemory);
				SizeGrid(state.pixels);
			}
			break;
		}
		case 'E': // edit color of selected pixel
		{
			char* pointer = tl::GetNextNumberChar(&commands.access(1));
			tl::MemorySpace transient = appMemory.transient;
			ParseColorFromCharArray(pointer, transient, state.pixels.sprite->content[state.pixels.selectedIndex]);
			ClearCommandBuffer();
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
		case 'D': // Delete
		{
			if (commands.get(1) == 'R' && commands.get(2) == '\0' && state.pixels.sprite->height > 1)
			{
				// Get start and end index of row
				int rowIndex = (int)(state.pixels.selectedIndex / state.pixels.sprite->width);
				unsigned int startDeleteIndex = rowIndex * state.pixels.sprite->width;
				unsigned int endDeleteIndex = rowIndex + state.pixels.sprite->width - 1;
				unsigned int spriteLength = state.pixels.sprite->width * state.pixels.sprite->height;

				// Call tl::DeleteFromArray with the sprite content
				tl::DeleteFromArray(state.pixels.sprite->content, startDeleteIndex, endDeleteIndex, spriteLength);

				// Subtract 1 from the sprite height
				state.pixels.sprite->height -= 1;

				SizeGrid(state.pixels);
			}
			else if (commands.get(1) == 'C' && commands.get(2) == '\0' && state.pixels.sprite->width > 1)
			{
				// get the column index
				unsigned int columnIndex = state.pixels.selectedIndex % state.pixels.sprite->width;
				unsigned int spriteLength = state.pixels.sprite->width * state.pixels.sprite->height;
				for (int i = state.pixels.sprite->height - 1; i >= 0; i -= 1)
				{
					unsigned int deleteIndex = (i * state.pixels.sprite->width) + columnIndex;
					tl::DeleteFromArray(state.pixels.sprite->content, deleteIndex, deleteIndex, spriteLength);
					spriteLength -= 1;
				}
				state.pixels.sprite->width -= 1;

				SizeGrid(state.pixels);
			}
			break;
		}
	}
	ClearCommandBuffer();
}

static bool CheckForCopy(const tl::Input& input)
{
	if (input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_C].keyDown && state.activeControl == SpriteGrid)
	{
		hasCopied = true;
		copiedColor = state.pixels.sprite->content[state.pixels.selectedIndex];
		return true;
	}
	return false;
}

static bool CheckForPaste(const tl::Input& input)
{
	if (hasCopied && input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_V].keyDown && state.activeControl == SpriteGrid)
	{
		state.pixels.sprite->content[state.pixels.selectedIndex] = copiedColor;
		return true;
	}
	return false;
}

static void ProcessCommandInput(const tl::Input& input)
{
	// Update command buffer from input
	if (commands.length() < commands.capacity())
	{
		for (int key = tl::KEY_A; key <= tl::KEY_Z; key += 1)
		{
			if (input.buttons[key].keyDown)
			{
				char commandChar = GetCharForAlphaKey(key);
				commands.append(commandChar);
			}
		}

		for (int key = tl::KEY_0; key <= tl::KEY_9; key += 1)
		{
			if (input.buttons[key].keyDown)
			{
				char commandChar = GetCharForDigitKey(key);
				commands.append(commandChar);
			}
		}

		if (input.buttons[tl::KEY_SPACE].keyDown)
		{
			commands.append(' ');
		}
	}
}

static void ProcessImmediateActionKeys(const tl::Input& input)
{
	if (input.buttons[tl::KEY_ESCAPE].keyDown)
	{
		ClearCommandBuffer();
		state.mode = View;
		return;
	}

	if (input.buttons[tl::KEY_TAB].keyDown)
	{
		int nextActiveControlIndex = state.activeControl + 1;
		state.activeControl = (nextActiveControlIndex < EditorControlCount) ? (EditorControl)nextActiveControlIndex : SpriteGrid;
		return;
	}

	if (state.mode == View)
	{
		if (CheckForCopy(input)) return;
		if (CheckForPaste(input)) return;

		if (input.buttons[tl::KEY_V].keyDown)
		{
			ClearCommandBuffer();
			state.mode = Visual;
			commands.append('V');
			commands.append('I');
			commands.append('S');
			commands.append('U');
			commands.append('A');
			commands.append('L');
			commands.append('\0');

			return;
		}

		if (input.buttons[tl::KEY_I].keyDown)
		{
			ClearCommandBuffer();
			state.mode = Insert;
			commands.append('I');
			commands.append('N');
			commands.append('S');
			commands.append('E');
			commands.append('R');
			commands.append('T');
			commands.append('\0');

			return;
		}

		if (input.character == ':')
		{
			ClearCommandBuffer();
			state.mode = Command;
			return;
		}
		return;
	}

	if (state.mode == Insert)
	{
		if (input.buttons[tl::KEY_ENTER].keyDown && state.activeControl == SpriteGrid)
		{
			state.pixels.sprite->content[state.pixels.selectedIndex] = currentColor;
			return;
		}
		return;
	}

	if (state.mode == Visual)
	{
		if (CheckForCopy(input)) return;
		if (CheckForPaste(input)) return;
		return;
	}

	if (state.mode == Command)
	{
		if (input.buttons[tl::KEY_ENTER].keyDown)
		{
			ExecuteCurrentCommand();
			state.mode = View;
			return;
		}
		ProcessCommandInput(input);
	}
}

int InitializeState(char* commandLine)
{
	if (*commandLine)
	{
		filePath = commandLine;
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
	if (state.mode != Command)
	{
		ProcessCursorMovementInput(input);
	}

	ProcessImmediateActionKeys(input);
	return state;
}
