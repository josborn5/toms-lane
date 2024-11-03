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
static const int skipModifierKey = tl::KEY_CTRL;
static const int cameraModifierKey = tl::KEY_SHIFT;

static bool hasCopied = false;
static tl::MemorySpace fontMemory;
static tl::MemorySpace spritePixelMemory;
static tl::MemorySpace sprite_color_table_memory;
static tl::MemorySpace fileReadMemory;
static tl::MemorySpace paletteMemory;
static tl::MemorySpace tempMemory;
static uint32_t currentColor;
static uint32_t copiedColor;
static tl::bitmap currentBitmap;

EditorState state;
static char commandBuffer[commandBufferSize];
static char filePathBuffer[filePathBufferSize] = {0};
static constexpr char* filePath = &filePathBuffer[0];
static tl::array<char> commands = tl::array<char>(commandBuffer, commandBufferSize);

static int GetCursorIndexForNextColor(Grid& grid, int step, int inclusiveMinPixelIndex, int inclusiveMaxPixelIndex, int prevIndex)
{
	uint32_t active_color = grid.selected_color();
	int pixelIndex = prevIndex;
	int provisionalIndex = pixelIndex + step;
	bool sameColor = true;
	while (provisionalIndex >= inclusiveMinPixelIndex && provisionalIndex <= inclusiveMaxPixelIndex && sameColor)
	{
		pixelIndex = provisionalIndex;
		sameColor = (active_color == grid.sprite->get_pixel_data(pixelIndex));
		provisionalIndex += step;
	}
	return pixelIndex;
}

static int GetCursorIndex(const tl::Input &input, Grid& grid, int prevIndex)
{
	int maxPixelIndex = (grid.sprite->width * grid.sprite->height) - 1;
	int currentRowIndex = grid.row_index(prevIndex);
	if (input.buttons[skipModifierKey].isDown)
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
			return GetCursorIndexForNextColor(grid, grid.sprite->width, 0, maxPixelIndex, prevIndex);
		}

		if (input.buttons[tl::KEY_DOWN].keyDown)
		{
			return GetCursorIndexForNextColor(grid, -grid.sprite->width, 0, maxPixelIndex, prevIndex);
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
		int provisionalSelectedPixelIndex = prevIndex - grid.sprite->width;
		if (provisionalSelectedPixelIndex >= 0) return provisionalSelectedPixelIndex;
	}
	else if (input.buttons[tl::KEY_UP].keyDown)
	{
		int provisionalSelectedPixelIndex = prevIndex + grid.sprite->width;
		if (provisionalSelectedPixelIndex <= maxPixelIndex) return provisionalSelectedPixelIndex;
	}
	return prevIndex;
}

static bool ApplyCursorMovementToState(const tl::Input& input)
{
	if (input.buttons[cameraModifierKey].isDown) return false;

	Grid& activeGrid = *state.activeControl;
	int newCursorIndex = GetCursorIndex(input, activeGrid, activeGrid.selectedIndex);
	bool handledInput = activeGrid.selectedIndex != newCursorIndex;
	activeGrid.selectedIndex = newCursorIndex;

	currentColor = state.palette_.selected_color();

	return handledInput;
}

static bool ApplyCameraMovementToState(const tl::Input& input)
{
	if (!state.pixels_are_selected() || !input.buttons[cameraModifierKey].isDown)
	{
		return false;
	}

	if (input.buttons[tl::KEY_I].keyDown)
	{
		state.pixels.camera.zoom *= 0.75f;
		return true;
	}
	else if (input.buttons[tl::KEY_U].keyDown)
	{
		state.pixels.camera.zoom /= 0.75f;
		return true;
	}

	float delta = (state.pixels.camera.zoom * 0.2f);
	if (input.buttons[tl::KEY_UP].keyDown)
	{
		state.pixels.camera.displacement.y += delta;
		return true;
	}
	else if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		state.pixels.camera.displacement.y -= delta;
		return true;
	}
	else if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		state.pixels.camera.displacement.x -= delta;
		return true;
	}
	else if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		state.pixels.camera.displacement.x += delta;
		return true;
	}

	return false;
}

static bool ApplySelectedRangeMovementToState(const tl::Input& input)
{
	Grid& activeGrid = *state.activeControl;
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

static void set_bits_per_pixel(int bits_per_pixel)
{
	if (bits_per_pixel == state.canvas.bitsPerPixel)
	{
		return;
	}

	if (bits_per_pixel != 1 && bits_per_pixel != 24)
	{
		WriteStringToCommandBuffer("Invalid bits per pixel");
		return;
	}

	if (bits_per_pixel == 24)
	{
		for (int i = 0; i < state.canvas.pixel_count(); i += 1)
		{
			uint32_t color_table_index = state.canvas.get_pixel_data(i);
			uint32_t pixel_color = state.canvas.p_color_table->get_pixel_data(color_table_index);
			state.canvas.set_pixel_data(i, pixel_color);
		}

		state.canvas.p_color_table->height = 0;
	}
	else if (bits_per_pixel == 1)
	{
		state.canvas.p_color_table->height = 2;
		state.canvas.p_color_table->set_pixel_data(0, 0x000000);
		state.canvas.p_color_table->set_pixel_data(1, 0xFFFFFF);

		for (int i = 0; i < state.canvas.pixel_count(); i += 1)
		{
			uint32_t pixel_color = state.canvas.get_pixel_data(i);
			uint32_t pixel_data = (pixel_color > 0x000000) ? 1 : 0;
			state.canvas.set_pixel_data(i, pixel_data);
		}
	}

	state.canvas.bitsPerPixel = (uint16_t)bits_per_pixel;
	InitializeLayout(state);
	state.pixels.size();
	state.color_table.size();
};

static void update_filepath(char* source)
{
	for (int i = 0; i < filePathBufferSize; i += 1)
	{
		filePathBuffer[i] = '\0';
	}
	int counter = 0;
	while (*source && counter < filePathBufferSize)
	{
		filePathBuffer[counter] = *source;
		counter++;
		source++;
	}
}

static int Initialize(char* commandLine)
{
	if (*commandLine)
	{
		update_filepath(commandLine);
	}

	// Load file
	int fileReadResult = tl::file_interface_read(filePath, fileReadMemory);
	int spriteLoadedResult = -1;

	if (fileReadResult == tl::Success)
	{
		tl::bitmap_interface_initialize(currentBitmap, fileReadMemory);
		spriteLoadedResult = InitializeSpriteCFromBitmap(
			state.canvas,
			currentBitmap);
	}

	if (spriteLoadedResult != 0)
	{
		const int default_dim = 2;
		state.canvas.width = default_dim;
		state.canvas.height = default_dim;
		state.canvas.bitsPerPixel = 24;
		state.canvas_color_table.width = 1;
		state.canvas_color_table.height = 0;
		for (int i = 0; i < default_dim * default_dim; i += 1)
		{
			state.canvas.set_pixel_data(i, 0x000000);
		}

		switch (fileReadResult)
		{
			case tl::InvalidFilePath:
			case tl::FileDoesNotExist:
				WriteStringToCommandBuffer("File not found");
				break;
			case tl::Success:
				if (spriteLoadedResult == -1)
				{
					WriteStringToCommandBuffer("Unsupported bitmap bits per pixel!");
				}
				else
				{
					WriteStringToCommandBuffer("Bitmap size is too big!");
				}
				break;
			default:
				WriteStringToCommandBuffer("BAD FILE READ");
		}
	}

	state.pixels.camera.zoom = 1.0f;
	state.pixels.camera.displacement = { 0.0f, 0.0f };
	state.pixels.selectedIndex = 0;
	state.pixels.selectedRangeIndex = 0;

	state.mode = View;

	ClearCommandBuffer();
	InitializeLayout(state);
	state.pixels.size();
	state.color_table.size();
	InitializePalettes(paletteMemory, tempMemory, state);

	return 0;
}

int InitializeState(const tl::GameMemory& gameMemory, char* commandLine, int clientX, int clientY)
{
	state.windowWidth = clientX;
	state.windowHeight = clientY;

	// Define memory slices
	tl::MemorySpace perm = gameMemory.permanent;
	const uint64_t oneKiloByteInBytes = 1024;
	const uint64_t oneMegaByteInBytes = oneKiloByteInBytes * 1024;
	constexpr uint64_t color_table_size_in_bytes = sizeof(uint32_t) * 8;

	tl::MemorySpace working;
	tl::font_interface_initialize_from_file("font-mono.tlsf", perm, working);

	paletteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	fileReadMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	sprite_color_table_memory = tl::CarveMemorySpace(color_table_size_in_bytes, working);
	spritePixelMemory = working; // left over memory goes to main sprite
	tempMemory = gameMemory.transient;

	state.canvas.pixel_memory = spritePixelMemory;
	state.canvas.p_color_table = &state.canvas_color_table;
	state.canvas_color_table.pixel_memory = sprite_color_table_memory;
	state.commandBuffer = &commandBuffer[0];
	state.pixels.sprite = &state.canvas;
	state.color_table.sprite = &state.canvas_color_table;

	return Initialize(commandLine);
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
	int cursor = 0;
	bool has = CommandHas(prefix, cursor);
	return has && (commands.get(cursor) != '\0');
}

static bool CommandIs(char* command)
{
	int cursor = 0;
	bool has = CommandHas(command, cursor);
	return has && (commands.get(cursor) == '\0');
}

static void ExecuteCurrentCommand()
{
	if (commands.get(0) != ':')
	{
		return;
	}

	if (CommandIs("R")) // append row
	{
		InsertRow(state.pixels);
		state.pixels.size();
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("C")) // append column
	{
		InsertColumn(state.pixels);
		state.pixels.size();
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
		tl::DeleteFromArray(state.pixels.sprite->pixels(), startDeleteIndex, endDeleteIndex, spriteLength);

		// Subtract 1 from the sprite height
		state.pixels.sprite->height -= 1;

		state.pixels.size();
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
			tl::DeleteFromArray(state.pixels.sprite->pixels(), deleteIndex, deleteIndex, spriteLength);
			spriteLength -= 1;
		}
		state.pixels.sprite->width -= 1;

		state.pixels.size();
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("W") || CommandStartsWith("W ")) // write to file
	{
		char* targetFilePath = CommandIs("W") ? filePath : &commands.access(3);

		int saveResult = SaveBitmap(tempMemory, *state.pixels.sprite, targetFilePath);
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
		Initialize(&commands.access(6));
		return;
	}
	else if (CommandStartsWith("E ")) // edit selected pixel
	{
		if (state.pixels_are_selected() && state.canvas.has_color_table())
		{
			return;
		}

		char* pointerToNumberChar = tl::GetNextNumberChar(&commands.access(1));
		Grid& activeGrid = *state.activeControl;
		uint32_t parsed_color;
		ParseColorFromCharArray(pointerToNumberChar, tempMemory, parsed_color);
		activeGrid.sprite->set_pixel_data(activeGrid.selectedIndex, parsed_color);
		ClearCommandBuffer();
		return;
	}
	else if (CommandStartsWith("BPP ")) // set bits per pixel
	{
		int new_bits_per_pixel = tl::CharStringToInt(&commands.access(5));
		set_bits_per_pixel(new_bits_per_pixel);
		return;
	}

	ClearCommandBuffer();
	WriteStringToCommandBuffer("NOT A COMMAND");
}

static bool CheckForCopy(const tl::Input& input)
{
	if (input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_C].keyDown && state.pixels_are_selected())
	{
		hasCopied = true;
		copiedColor = state.pixels.selected_color();
		return true;
	}
	return false;
}

static bool CheckForPaste(const tl::Input& input)
{
	if (hasCopied && input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_V].keyDown && state.pixels_are_selected())
	{
		state.pixels.sprite->set_pixel_data(state.pixels.selectedIndex, copiedColor);
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
	if (input.buttons[tl::KEY_ENTER].keyDown)
	{
		uint32_t pixel_data_to_set = (state.pixels_are_selected() && state.canvas.has_color_table())
			? state.color_table.selectedIndex
			: currentColor;
		Grid& activeControl = *state.activeControl;
		activeControl.sprite->set_pixel_data(activeControl.selectedIndex, pixel_data_to_set);
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

static Grid* cycle_active_control()
{
	if (state.activeControl == &state.pixels)
	{
		if (state.pixels.sprite->has_color_table())
		{
			return &state.color_table;
		}
		return &state.palette_;
	}

	if (state.activeControl == &state.color_table)
	{
		return &state.palette_;
	}

	return &state.pixels;
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
		Grid* next_control = cycle_active_control();
		state.activeControl = next_control;
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

