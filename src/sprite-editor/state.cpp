#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./palettes.hpp"
#include "./commands.hpp"
#include "./utilities.hpp"
#include "./transform.hpp"
#include "./operations.hpp"

static const int commandBufferSize = 256;
static const int filePathBufferSize = 256;
static const int modeBufferSize = 2;
static const int skipModifierKey = tl::KEY_CTRL;
static const int cameraModifierKey = tl::KEY_SHIFT;

static operation_executor the_undo;
static clipboard the_clipboard;

static tl::MemorySpace spritePixelMemory;
static tl::MemorySpace sprite_color_table_memory;
static tl::MemorySpace fileReadMemory;
static tl::MemorySpace paletteMemory;
static tl::MemorySpace tempMemory;
static uint32_t currentColor = 0;
static tl::bitmap currentBitmap;

EditorState state;
static char commandBuffer[commandBufferSize] = {0};
static char filePathBuffer[filePathBufferSize] = {0};
static constexpr char* filePath = &filePathBuffer[0];
static tl::array<char> commands = tl::array<char>(commandBuffer, commandBufferSize);

static bool apply_movement_to_item_in_grid(const tl::Input &input, item_in_grid& grid)
{
	if (input.buttons[skipModifierKey].isDown)
	{
		if (input.buttons[tl::KEY_HOME].keyDown)
		{
			grid.move_start();
			return true;
		}

		if (input.buttons[tl::KEY_END].keyDown)
		{
			grid.move_end();
			return true;
		}

		if (input.buttons[tl::KEY_LEFT].keyDown)
		{
			grid.color_jump_left();
			return true;
		}

		if (input.buttons[tl::KEY_RIGHT].keyDown)
		{
			grid.color_jump_right();
			return true;
		}

		if (input.buttons[tl::KEY_UP].keyDown)
		{
			grid.color_jump_up();
			return true;
		}

		if (input.buttons[tl::KEY_DOWN].keyDown)
		{
			grid.color_jump_down();
			return true;
		}
		return false;
	}

	if (input.buttons[tl::KEY_HOME].keyDown)
	{
		grid.move_row_start();
		return true;
	}
	if (input.buttons[tl::KEY_END].keyDown)
	{
		grid.move_row_end();
		return true;
	}
	if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		grid.move_right();
		return true;
	}
	if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		grid.move_left();
		return true;
	}
	if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		grid.move_down();
		return true;
	}
	if (input.buttons[tl::KEY_UP].keyDown)
	{
		grid.move_up();
		return true;
	}
	return false;
}

static bool ApplyCursorMovementToState(const tl::Input& input)
{
	if (input.buttons[cameraModifierKey].isDown) return false;

	Grid& activeGrid = *state.activeControl;
	bool handledInput = apply_movement_to_item_in_grid(input, activeGrid.cursor);
	currentColor = state.palette_.selected_color();
	return handledInput;
}

static float clamp_float(float min, float val, float max)
{
	if (val < min)
	{
		return min;
	}

	if (val > max)
	{
		return max;
	}

	return val;
}

static void pin_camera_displacement()
{
	float max_displacement = 1.0f + state.pixels.camera.zoom;
	float min_displacement = -1.0f - state.pixels.camera.zoom;

	state.pixels.camera.displacement.y = clamp_float(min_displacement, state.pixels.camera.displacement.y, max_displacement);
	state.pixels.camera.displacement.x = clamp_float(min_displacement, state.pixels.camera.displacement.x, max_displacement);
}

static bool ApplyCameraMovementToState(const tl::Input& input)
{
	if (!state.pixels_are_selected() || !input.buttons[cameraModifierKey].isDown)
	{
		return false;
	}

	if (input.buttons[tl::KEY_R].keyDown)
	{
		state.pixels.camera.zoom = 1.0f;
		state.pixels.camera.displacement = { 0.0f, 0.0f };
	}

	if (input.buttons[tl::KEY_I].keyDown)
	{
		state.pixels.camera.zoom *= 0.75f;
		pin_camera_displacement();
		return true;
	}
	else if (input.buttons[tl::KEY_U].keyDown)
	{
		state.pixels.camera.zoom /= 0.75f;
		pin_camera_displacement();
		return true;
	}

	float delta = (state.pixels.camera.zoom * 0.2f);
	if (input.buttons[tl::KEY_UP].keyDown)
	{
		state.pixels.camera.displacement.y += delta;
		pin_camera_displacement();
		return true;
	}
	else if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		state.pixels.camera.displacement.y -= delta;
		pin_camera_displacement();
		return true;
	}
	else if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		state.pixels.camera.displacement.x -= delta;
		pin_camera_displacement();
		return true;
	}
	else if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		state.pixels.camera.displacement.x += delta;
		pin_camera_displacement();
		return true;
	}

	return false;
}

static bool ApplySelectedRangeMovementToState(const tl::Input& input)
{
	Grid& activeGrid = *state.activeControl;
	return apply_movement_to_item_in_grid(input, activeGrid.range);
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
		for (unsigned int i = 0; i < state.canvas.pixel_count(); i += 1)
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

		for (unsigned int i = 0; i < state.canvas.pixel_count(); i += 1)
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
	state.pixels.cursor.move_start();
	state.pixels.range.move_start();

	state.mode = View;
	the_clipboard.clear();

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
	const uint64_t oneKiloByteInBytes = 1024;
	const uint64_t oneMegaByteInBytes = oneKiloByteInBytes * 1024;
	constexpr uint64_t color_table_size_in_bytes = sizeof(uint32_t) * 8;

	tl::MemorySpace working = gameMemory.permanent;
	tl::font_interface_initialize();

	paletteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	fileReadMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	sprite_color_table_memory = tl::CarveMemorySpace(color_table_size_in_bytes, working);
	spritePixelMemory = working; // left over memory goes to main sprite
	tempMemory = gameMemory.transient;

	state.canvas.pixel_memory = spritePixelMemory;
	state.canvas.p_color_table = &state.canvas_color_table;
	state.canvas_color_table.pixel_memory = sprite_color_table_memory;
	state.commandBuffer = &commandBuffer[0];
	state.pixels.initialize(&state.canvas);
	state.color_table.initialize(&state.canvas_color_table);

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
		if (can_insert_row(*state.pixels.sprite))
		{
			insert_row_operation& insert_operation = the_undo.get_insert_row(&state.pixels);
			insert_operation.execute();
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("INSERT ROW FAILED!");
		}
		return;
	}
	else if (CommandStartsWith("R")) // append multiple rows
	{
		char row_count_char = commands.get(2);
		if (row_count_char < '0' || row_count_char > '9')
		{
			WriteStringToCommandBuffer("BAD INSERT ROW COUNT!");
			return;
		}
		int row_count = row_count_char - '0';
		for (int i = 0; i < row_count; i += 1)
		{
			insert_row_operation& insert_operation = the_undo.get_insert_row(&state.pixels);
			insert_operation.execute();
		}
		ClearCommandBuffer();
	}
	else if (CommandIs("C")) // append column
	{
		if (can_insert_column(*state.pixels.sprite))
		{
			insert_column_operation& insert_operation = the_undo.get_insert_column(&state.pixels);
			insert_operation.execute();
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("INSERT COLUMN FAILED!");
		}
		return;
	}
	else if (CommandIs("P")) // switch palette
	{
		SwitchPalette(state);
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("DR")) // delete row
	{
		if (can_delete_row(*state.pixels.sprite))
		{
			delete_row_operation& delete_operation = the_undo.get_delete_row(&state.pixels);
			delete_operation.execute();
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("DELETE ROW FAILED!");
		}
		return;
	}
	else if (CommandIs("DC")) // delete column
	{
		if (can_delete_column(*state.pixels.sprite))
		{
			delete_column_operation& delete_operation = the_undo.get_delete_column(&state.pixels);
			delete_operation.execute();
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("DELETE COLUMN FAILED!");
		}
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
		uint32_t parsed_color;
		ParseColorFromCharArray(pointerToNumberChar, tempMemory, parsed_color);

		set_pixel_data_operation& operation = the_undo.get_set_pixel_data(state.activeControl, parsed_color);
		operation.execute();

		ClearCommandBuffer();
		return;
	}
	else if (CommandStartsWith("BPP ")) // set bits per pixel
	{
		int new_bits_per_pixel = tl::CharStringToInt(&commands.access(5));
		set_bits_per_pixel(new_bits_per_pixel);
		return;
	}

	WriteStringToCommandBuffer("NOT A COMMAND");
}

static bool CheckForCopy(const tl::Input& input)
{
	if ((
			(input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_C].keyDown)
			|| input.buttons[tl::KEY_Y].keyDown
		)
		&& state.pixels_are_selected())
	{
		int copy_cursor_index = state.pixels.cursor.index();
		int copy_range_index = (state.mode == Visual) ? state.pixels.range.index() : copy_cursor_index;

		Grid& grid = state.pixels;
		copy_to_clipboard(
			*grid.sprite,
			grid.cursor.index(),
			copy_range_index,
			the_clipboard
		);

		WriteStringToCommandBuffer("COPIED");
		return true;
	}

	if ((
			(input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_X].keyDown)
			|| input.buttons[tl::KEY_D].keyDown
		)
		&& state.pixels_are_selected())
	{
		int copy_cursor_index = state.pixels.cursor.index();
		int copy_range_index = (state.mode == Visual) ? state.pixels.range.index() : copy_cursor_index;

		Grid grid = state.pixels;
		paste_pixel_data_operation& cut_operation = the_undo.get_paste_pixel_data(grid.sprite);
		cut_to_clipboard_operation(
			*grid.sprite,
			grid.cursor.index(),
			copy_range_index,
			cut_operation,
			the_clipboard
		);

		cut_operation.execute();

		WriteStringToCommandBuffer("CUT");
		return true;
	}

	return false;
}

static bool CheckForPaste(const tl::Input& input)
{
	if (
			(
				(input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_V].keyDown)
				|| input.buttons[tl::KEY_P].keyDown
			)
			&& state.pixels_are_selected()
			&& the_clipboard.is_set()
		)
	{
		paste_pixel_data_operation& paste_operation  = the_undo.get_paste_pixel_data(state.pixels.sprite);
		paste_from_clipboard_operation(
			state.pixels,
			the_clipboard,
			paste_operation
		);

		paste_operation.execute();
		WriteStringToCommandBuffer("PASTE");
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

static bool check_for_undo(const tl::Input& input)
{
	if ((input.buttons[tl::KEY_CTRL].isDown && input.buttons[tl::KEY_Z].keyDown)
		|| input.buttons[tl::KEY_U].keyDown)
	{
		the_undo.do_undo();
		return true;
	}

	return false;
}

static bool center_cursor_on_screen(const tl::Input& input)
{
	if (input.buttons[tl::KEY_C].keyDown)
	{
		state.activeControl->cursor.center();
		return true;
	}

	return false;
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
		state.pixels.range.set_index(state.pixels.cursor.index());
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
	if (check_for_undo(input)) return;
	if (center_cursor_on_screen(input)) return;
}

static void ApplyInsertModeInputToState(const tl::Input& input)
{
	if (ApplyCursorMovementToState(input)) return;
	if (input.buttons[tl::KEY_ENTER].keyDown)
	{
		uint32_t data_to_set = (state.pixels_are_selected() && state.canvas.has_color_table())
			? state.color_table.cursor.index()
			: currentColor;
		set_pixel_data_operation& operation = the_undo.get_set_pixel_data(state.activeControl, data_to_set);
		operation.execute();
		return;
	}
	if (check_for_undo(input)) return;
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
			if (ApplyCameraMovementToState(input)) return state;
			if (ApplySelectedRangeMovementToState(input)) return state;
			if (CheckForCopy(input))
			{
				state.mode = View;
				 return state;
			}
			if (CheckForPaste(input)) return state;
			break;
		case Command:
			ApplyCommandModeInputToState(input);
			break;
	}

	return state;
}

