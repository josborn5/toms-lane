#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./palettes.hpp"
#include "./commands.hpp"

static const int commandBufferSize = 256;
static const int filePathBufferSize = 256;
static const int modeBufferSize = 2;


static bool hasCopied = false;
static tl::MemorySpace spriteMemory;
static tl::MemorySpace paletteMemory;
static tl::Color currentColor;
static tl::Color copiedColor;
static tl::SpriteC currentSprite;

tl::GameMemory appMemory;

EditorState state;
static char commandBuffer[commandBufferSize];
static char filePathBuffer[filePathBufferSize] = {0};
static char* filePath = &filePathBuffer[0];
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

static void ClearCommandBuffer()
{
	for (int i = 0; i < commands.capacity(); i += 1)
	{
		commands.access(i) = '\0';
		commands.clear();
	}
}


static int Initialize(const tl::GameMemory& gameMemory)
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
	if (*filePath)
	{
		uint64_t fileSize = 0;
		int getSizeResult = tl::file_interface_size_get(filePath, fileSize);
		if (getSizeResult != tl::Success)
		{
			// If the file does not exist, try to create it
			if (getSizeResult == tl::FileDoesNotExist)
			{
				tl::MemorySpace empty;
				empty.content = (void*)"";
				empty.sizeInBytes = 0;
				int createEmptyFileResult = tl::file_interface_write(filePath, empty);
				if (createEmptyFileResult != tl::Success)
				{
					return 1;
				}

				// Initialize default sprite
				fileReadMemory.content = "2\n2\n0 0 0 0\n0 0 0 0\n0 0 0 0\n0 0 0 0";
			}
			else
			{
				return 1;
			}
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

static void CopyString(char* source, char* target)
{
	while (*source)
	{
		*target = *source;
		target++;
		source++;
	}
}

int InitializeState(char* commandLine)
{
	if (*commandLine)
	{
		CopyString(commandLine, filePath);
	}

	tl::InitializeMemory(
		2,
		2,
		appMemory
	);

	return Initialize(appMemory);
}

static void ExecuteCurrentCommand()
{
	if (commands.get(0) != ':')
	{
		return;
	}
	switch (commands.get(1))
	{
		case 'S': // save
		{
			if (commands.get(2) == '\0') // save to current filePath
			{
				Save(appMemory, *state.pixels.sprite, commands, filePath);
				return;
			}
			else if (commands.get(2) == ' ' && commands.get(3)) // save to new filePath
			{
				filePath = &commands.access(3);
				Save(appMemory, *state.pixels.sprite, commands, filePath);
				return;
			}
			break;
		}
		case 'R': // append row
		{
			if (commands.get(2) == '\0')
			{
				InsertRow(state.pixels, spriteMemory);
				SizeGrid(state.pixels);
				ClearCommandBuffer();
				return;
			}
			break;
		}
		case 'C': // append column
		{
			if (commands.get(2) == '\0')
			{
				InsertColumn(state.pixels, spriteMemory);
				SizeGrid(state.pixels);
				ClearCommandBuffer();
				return;
			}
			break;
		}
		case 'E': // edit color of selected pixel
		{
			if (commands.get(2) == 'D'
				&& commands.get(3) == 'I'
				&& commands.get(4) == 'T'
				&& commands.get(5) == ' '
				&& commands.get(6) != '\0')
			{
				InitializeState(&commands.access(6));
				return;
			}
			else
			{
				char* pointer = tl::GetNextNumberChar(&commands.access(1));
				tl::MemorySpace transient = appMemory.transient;
				ParseColorFromCharArray(pointer, transient, state.pixels.sprite->content[state.pixels.selectedIndex]);
				ClearCommandBuffer();
				return;
			}
			break;
		}
		case 'P': // switch palette
		{
			if (commands.get(2) == '\0')
			{
				SwitchPalette(state);
				ClearCommandBuffer();
				return;
			}
			break;
		}
		case 'D': // Delete
		{
			if (commands.get(2) == 'R' && commands.get(3) == '\0' && state.pixels.sprite->height > 1)
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
				ClearCommandBuffer();
				return;
			}
			else if (commands.get(2) == 'C' && commands.get(3) == '\0' && state.pixels.sprite->width > 1)
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
				ClearCommandBuffer();
				return;
			}
			break;
		}
		case 'W':
		{
			if (*filePath && commands.get(2) == '\0')
			{
				Save(appMemory, *state.pixels.sprite, commands, filePath);
				return;
			}
		}
		break;
	}
	ClearCommandBuffer();
	commands.append('N');
	commands.append('O');
	commands.append('T');
	commands.append(' ');
	commands.append('A');
	commands.append(' ');
	commands.append('C');
	commands.append('O');
	commands.append('M');
	commands.append('M');
	commands.append('A');
	commands.append('N');
	commands.append('D');
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
	if (commands.length() < commands.capacity() && input.character != 0)
	{
		// Capitalize
		char toAppend = (input.character >= 'a' && input.character <= 'z')
			? input.character + ('A' - 'a')
			: input.character;
		commands.append(toAppend);
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
			commands.append(':');
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

const EditorState& GetLatestState(const tl::Input& input)
{
	if (state.mode != Command)
	{
		ProcessCursorMovementInput(input);
	}

	ProcessImmediateActionKeys(input);
	return state;
}
