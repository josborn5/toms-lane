#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./palettes.hpp"
#include "./commands.hpp"
#include "./utilities.hpp"
#include "./transform.hpp"

static const int commandBufferSize = 256;
static const int filePathBufferSize = 256;
static const int modeBufferSize = 2;


static bool hasCopied = false;
static tl::MemorySpace fontMemory;
static tl::MemorySpace spriteMemory;
static tl::MemorySpace fileReadMemory;
static tl::MemorySpace paletteMemory;
static Color currentColor;
static Color copiedColor;
static SpriteC currentSprite;
static tl::bitmap currentBitmap;

tl::GameMemory appMemory;

EditorState state;
static char commandBuffer[commandBufferSize];
static char filePathBuffer[filePathBufferSize] = {0};
static char* filePath = &filePathBuffer[0];
static tl::array<char> commands = tl::array<char>(commandBuffer, commandBufferSize);

static char* ParseColorFromCharArray(char* content, tl::MemorySpace& space, Color& color)
{
	char* buffer = (char*)space.content;
	char* workingPointer = content;

	/// RBGA values
	int rgbaContent[4] = { 0, 0, 0, 255 }; // Default alpha to 100%

	for (int i = 0; i < 4 && *workingPointer; i += 1)
	{
		workingPointer = tl::GetNextNumberChar(workingPointer);
		if (*workingPointer)
		{
			workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
			rgbaContent[i] = tl::CharStringToInt(buffer);
		}
	}

	color.r = (float)rgbaContent[0] / 255.0f;
	color.g = (float)rgbaContent[1] / 255.0f;
	color.b = (float)rgbaContent[2] / 255.0f;
	color.a = (float)rgbaContent[3] / 255.0f;

	return workingPointer;
}

/*
* Assumed char* format is:
* width<int>\n
* height<int>\n
* RValue<char>, GValue<char>, BValue<char>, AValue<char>\n // 1st pixel
* :
* RValue<char>, GValue<char>, BValue<char>, AValue<char>\n // Nth pixel
*/
void LoadSpriteC(char* content, tl::MemorySpace& space, SpriteC& sprite)
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

	for (int i = 0; i < contentCount && *workingPointer; i += 1)
	{
		workingPointer = ParseColorFromCharArray(workingPointer, space, sprite.content[i]);
	}
}


static int CompareColor(const Color& color1, const Color& color2)
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

static int GetCursorIndexForNextColor(Grid& grid, int step, int inclusiveMinPixelIndex, int inclusiveMaxPixelIndex, int prevIndex)
{
	Color activeColor = grid.sprite->content[grid.selectedIndex];
	int pixelIndex = prevIndex;
	int provisionalIndex = pixelIndex + step;
	bool sameColor = true;
	while (provisionalIndex >= inclusiveMinPixelIndex && provisionalIndex <= inclusiveMaxPixelIndex && sameColor)
	{
		pixelIndex = provisionalIndex;
		sameColor = (CompareColor(activeColor, grid.sprite->content[pixelIndex]) == 0);
		provisionalIndex += step;
	}
	return pixelIndex;
}

static int GetCursorIndex(const tl::Input &input, Grid& grid, int prevIndex)
{
	int maxPixelIndex = (grid.sprite->width * grid.sprite->height) - 1;
	int currentRowIndex = GetRowIndex(grid, prevIndex);
	if (input.buttons[tl::KEY_CTRL].isDown)
	{
		if (input.buttons[tl::KEY_HOME].keyDown) return 0;

		if (input.buttons[tl::KEY_END].keyDown) return maxPixelIndex;

		if (input.buttons[tl::KEY_LEFT].keyDown)
		{
			int minIndexForRow = grid.sprite->width * currentRowIndex;
			int newSelectedIndex = GetCursorIndexForNextColor(grid, -1, minIndexForRow , maxPixelIndex, prevIndex);
			return newSelectedIndex;
		}

		if (input.buttons[tl::KEY_RIGHT].keyDown)
		{
			int maxIndexForRow = (grid.sprite->width * (currentRowIndex + 1)) - 1;
			int newSelectedIndex = GetCursorIndexForNextColor(grid, 1, 0, maxIndexForRow, prevIndex);
			return newSelectedIndex;
		}

		if (input.buttons[tl::KEY_UP].keyDown)
		{
			return GetCursorIndexForNextColor(grid, -grid.sprite->width, 0, maxPixelIndex, prevIndex);
		}

		if (input.buttons[tl::KEY_DOWN].keyDown)
		{
			return GetCursorIndexForNextColor(grid, grid.sprite->width, 0, maxPixelIndex, prevIndex);
		}
		return prevIndex;
	}

	if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		int maxIndexForRow = (grid.sprite->width * (currentRowIndex + 1)) - 1;
		if (prevIndex < maxIndexForRow) return prevIndex + 1;
	}
	else if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		int minIndexForRow = grid.sprite->width * currentRowIndex;
		if (prevIndex > minIndexForRow) return prevIndex - 1;
	}
	else if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		int provisionalSelectedPixelIndex = prevIndex + grid.sprite->width;
		if (provisionalSelectedPixelIndex <= maxPixelIndex) return provisionalSelectedPixelIndex;
	}
	else if (input.buttons[tl::KEY_UP].keyDown)
	{
		int provisionalSelectedPixelIndex = prevIndex - grid.sprite->width;
		if (provisionalSelectedPixelIndex >= 0) return provisionalSelectedPixelIndex;
	}
	return prevIndex;
}

static bool ApplyCursorMovementToState(const tl::Input& input)
{
	if (input.buttons[tl::KEY_CTRL].isDown) return false;

	Grid& activeGrid = (state.activeControl == SpriteGrid) ? state.pixels : state.palette_;
	int newCursorIndex = GetCursorIndex(input, activeGrid, activeGrid.selectedIndex);
	bool handledInput = activeGrid.selectedIndex != newCursorIndex;
	activeGrid.selectedIndex = newCursorIndex;

	currentColor = state.palette_.sprite->content[state.palette_.selectedIndex];

	return handledInput;
}

static bool ApplyCameraMovementToState(const tl::Input& input)
{
	if (state.activeControl != SpriteGrid || !input.buttons[tl::KEY_CTRL].isDown)
	{
		return false;
	}

	if (input.buttons[tl::KEY_I].keyDown)
	{
		state.pixels.camera.halfSize.x *= 0.75f;
		state.pixels.camera.halfSize.y *= 0.75f;
		return true;
	}
	else if (input.buttons[tl::KEY_U].keyDown)
	{
		state.pixels.camera.halfSize.x /= 0.75f;
		state.pixels.camera.halfSize.y /= 0.75f;
		return true;
	}
	else if (input.buttons[tl::KEY_UP].keyDown)
	{
		state.pixels.camera.position.y += (state.pixels.camera.halfSize.y * 0.2f);
		return true;
	}
	else if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		state.pixels.camera.position.y -= (state.pixels.camera.halfSize.y * 0.2f);
		return true;
	}
	else if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		state.pixels.camera.position.x -= (state.pixels.camera.halfSize.x * 0.2f);
		return true;
	}
	else if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		state.pixels.camera.position.x += (state.pixels.camera.halfSize.x * 0.2f);
		return true;
	}

	return false;
}

static bool ApplySelectedRangeMovementToState(const tl::Input& input)
{
	Grid& activeGrid = (state.activeControl == SpriteGrid) ? state.pixels : state.palette_;
	int newCursorIndex = GetCursorIndex(input, activeGrid, activeGrid.selectedRangeIndex);
	bool handledInput = activeGrid.selectedRangeIndex != newCursorIndex;
	activeGrid.selectedRangeIndex = newCursorIndex;

	return handledInput;
}

static void ClearCommandBuffer()
{
	for (int i = 0; i < commands.capacity(); i += 1)
	{
		commands.access(i) = '\0';
		commands.clear();
	}
}

static void WriteStringToCommandBuffer(char* character)
{
	ClearCommandBuffer();
	int counter = 1;
	while(*character && counter < commandBufferSize)
	{
		commands.append(*character);
		character++;
		counter += 1;
	}
}

static int Initialize(const tl::GameMemory& gameMemory, int clientX, int clientY)
{
	state.mode = View;
	state.commandBuffer = &commandBuffer[0];
	state.windowWidth = clientX;
	state.windowHeight = clientY;

	// Define memory slices
	tl::MemorySpace perm = gameMemory.permanent;
	const uint64_t oneKiloByteInBytes = 1024;
	const uint64_t oneMegaByteInBytes = oneKiloByteInBytes * 1024;

	tl::MemorySpace working;
	tl::font_interface_initialize_from_file("font-mono.tlsf", perm, working);

	paletteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	spriteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	fileReadMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	tl::MemorySpace temp = gameMemory.transient;
	tl::MemorySpace tempMemory = tl::CarveMemorySpace(oneMegaByteInBytes, temp);

	currentSprite.content = (Color*)spriteMemory.content;
	state.pixels.sprite = &currentSprite;

	ClearCommandBuffer();
	InitializeLayout(state);
	InitializePalettes(paletteMemory, tempMemory, state);

	// Load file
	int fileReadResult = tl::file_interface_read(filePath, fileReadMemory);
	if (fileReadResult != tl::Success)
	{
		LoadSpriteC("2\n2\n0 0 0 0\n0 0 0 0\n0 0 0 0\n0 0 0 0", tempMemory, currentSprite);

		switch (fileReadResult)
		{
			case tl::InvalidFilePath:
			case tl::FileDoesNotExist:
				WriteStringToCommandBuffer("File not found");
				break;
			default:
				WriteStringToCommandBuffer("BAD FILE READ");
		}
	}
	else
	{
		tl::bitmap_interface_initialize(currentBitmap, fileReadMemory);
		InitializeSpriteCFromBitmap(
			currentSprite,
			currentBitmap,
			spriteMemory);
	}

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

int InitializeState(char* commandLine, int clientX, int clientY)
{
	if (*commandLine)
	{
		CopyString(commandLine, filePath);
	}

	tl::InitializeMemory(
		4,
		1,
		appMemory
	);

	return Initialize(appMemory, clientX, clientY);
}

static bool CommandHas(char* compare, int& cursor)
{
	cursor = 1;
	bool match = true;
	while (*compare && match && cursor < commandBufferSize)
	{
		match = (commands.get(cursor) == *compare);
		cursor += 1;
		compare++;
	}
	return match;
}

static bool CommandStartsWith(char* prefix)
{
	int cursor;
	return CommandHas(prefix, cursor) & (commands.get(cursor) != '\0');
}

static bool CommandIs(char* command)
{
	int cursor;
	return CommandHas(command, cursor) & (commands.get(cursor) == '\0');
}

static void ExecuteCurrentCommand()
{
	if (commands.get(0) != ':')
	{
		return;
	}

	if (CommandIs("R")) // append row
	{
		InsertRow(state.pixels, spriteMemory);
		SizeGrid(state.pixels);
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("C")) // append column
	{
		InsertColumn(state.pixels, spriteMemory);
		SizeGrid(state.pixels);
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("P")) // switch palette
	{
		SwitchPalette(state);
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("DR") && state.pixels.sprite->height > 1) // delete row
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
	else if (CommandIs("DC") && state.pixels.sprite->width > 1) // delete column
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
	else if (CommandIs("W") || CommandStartsWith("W ")) // write to file
	{
		char* targetFilePath = CommandIs("W") ? filePath : &commands.access(3);

		int saveResult = SaveBitmap(appMemory, *state.pixels.sprite, targetFilePath);
		if (saveResult == tl::Success)
		{
			WriteStringToCommandBuffer("Saved");
		}
		else
		{
			WriteStringToCommandBuffer("Save error");
		}
		return;
	}
	else if (CommandStartsWith("EDIT ")) // edit new file
	{
		InitializeState(&commands.access(6), state.windowWidth, state.windowHeight);
		return;
	}
	else if (CommandStartsWith("E ")) // edit selected pixel
	{
		char* pointer = tl::GetNextNumberChar(&commands.access(1));
		tl::MemorySpace transient = appMemory.transient;
		ParseColorFromCharArray(pointer, transient, state.pixels.sprite->content[state.pixels.selectedIndex]);
		ClearCommandBuffer();
		return;
	}
	else if (CommandStartsWith("BPP ")) // set bits per pixel
	{
		int newBitValue = tl::CharStringToInt(&commands.access(5));
		if (newBitValue == 24)
		{
			state.pixels.sprite->bitsPerPixel = newBitValue;
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("Invalid bits per pixel");
		}
		return;
	}

	ClearCommandBuffer();
	WriteStringToCommandBuffer("NOT A COMMAND");
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
	// Backspace
	const char asciiBackspace = 8;
	if (input.character == asciiBackspace)
	{
		if (commands.length() > 0)
		{
			// hack to replace the removed char with a null
			*(commands.getTailPointer()) = '\0';
			commands.pop();
		}
		return;
	}

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

static void ApplyViewModeInputToState(const tl::Input& input)
{
	if (ApplyCursorMovementToState(input)) return;
	if (ApplyCameraMovementToState(input)) return;
	if (CheckForCopy(input)) return;
	if (CheckForPaste(input)) return;

	if (input.buttons[tl::KEY_V].keyDown)
	{
		state.mode = Visual;
		state.pixels.selectedRangeIndex = state.pixels.selectedIndex;
		WriteStringToCommandBuffer("VISUAL");
		return;
	}

	if (input.buttons[tl::KEY_I].keyDown)
	{
		state.mode = Insert;
		WriteStringToCommandBuffer("INSERT");
		return;
	}

	if (input.character == ':')
	{
		state.mode = Command;
		WriteStringToCommandBuffer(":");
		return;
	}
}

static void ApplyInsertModeInputToState(const tl::Input& input)
{
	if (ApplyCursorMovementToState(input)) return;
	if (input.buttons[tl::KEY_ENTER].keyDown && state.activeControl == SpriteGrid)
	{
		state.pixels.sprite->content[state.pixels.selectedIndex] = currentColor;
		return;
	}
}

static void ApplyCommandModeInputToState(const tl::Input& input)
{
	if (input.buttons[tl::KEY_ENTER].keyDown)
	{
		ExecuteCurrentCommand();
		state.mode = View;
		return;
	}
	ProcessCommandInput(input);
}

const EditorState& GetLatestState(const tl::Input& input)
{
	if (input.buttons[tl::KEY_ESCAPE].keyDown)
	{
		ClearCommandBuffer();
		state.mode = View;
		return state;
	}

	if (input.buttons[tl::KEY_TAB].keyDown)
	{
		int nextActiveControlIndex = state.activeControl + 1;
		state.activeControl = (nextActiveControlIndex < EditorControlCount) ? (EditorControl)nextActiveControlIndex : SpriteGrid;
		return state;
	}

	switch (state.mode)
	{
		case View:
			ApplyViewModeInputToState(input);
			break;
		case Insert:
			ApplyInsertModeInputToState(input);
			break;
		case Visual:
			if (ApplySelectedRangeMovementToState(input)) return state;
			if (CheckForCopy(input)) return state;
			if (CheckForPaste(input)) return state;
			break;
		case Command:
			ApplyCommandModeInputToState(input);
			break;
	}

	return state;
}

