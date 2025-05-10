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

static bool ApplyCameraMovementToState(const tl::Input& input)
{
	if (!state.pixels_are_selected() || !input.buttons[cameraModifierKey].isDown)
	{
		return false;
	}

	if (input.buttons[tl::KEY_R].keyDown)
	{
		state.pixels.camera_focus.reset();
		return true;
	}
	else if (input.buttons[tl::KEY_I].keyDown)
	{
		state.pixels.camera_focus.zoom_in();
		return true;
	}
	else if (input.buttons[tl::KEY_U].keyDown)
	{
		state.pixels.camera_focus.zoom_out();
		return true;
	}

	int delta_y = (int)(state.pixels.camera_focus.zoom * 0.2f * (float)state.pixels.sprite->height);
	if (delta_y < 1)
	{
		delta_y = 1;
	}
	if (input.buttons[tl::KEY_UP].keyDown)
	{
		for (int i = 0; i < delta_y; i += 1)
		{
			state.pixels.camera_focus.move_up();
		}
		return true;
	}
	else if (input.buttons[tl::KEY_PAGEUP].keyDown)
	{
		for (int i = 0; i < delta_y; i += 1)
		{
			state.pixels.camera_focus.move_up();
		}
		return true;
	}
	else if (input.buttons[tl::KEY_DOWN].keyDown)
	{
		for (int i = 0; i < delta_y; i += 1)
		{
			state.pixels.camera_focus.move_down();
		}
		return true;
	}
	else if (input.buttons[tl::KEY_PAGEDOWN].keyDown)
	{
		for (int i = 0; i < delta_y; i += 1)
		{
			state.pixels.camera_focus.move_down();
		}
		return true;
	}

	int delta_x = (int)(state.pixels.camera_focus.zoom * 0.2f * (float)state.pixels.sprite->width);
	if (delta_x < 1)
	{
		delta_x = 1;
	}
	if (input.buttons[tl::KEY_LEFT].keyDown)
	{
		for (int i = 0; i < delta_x; i += 1)
		{
			state.pixels.camera_focus.move_left();
		}
		return true;
	}
	else if (input.buttons[tl::KEY_RIGHT].keyDown)
	{
		for (int i = 0; i < delta_x; i += 1)
		{
			state.pixels.camera_focus.move_right();
		}
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

	if (bits_per_pixel != 1 && bits_per_pixel != 24 && bits_per_pixel != 4 && bits_per_pixel != 2)
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
	} else if (bits_per_pixel == 4) {
		const unsigned int color_table_size = 16;
		state.canvas.p_color_table->height = color_table_size;

		// for now pick the first 16 unique colors from the sprite to set in the color table
		unsigned int color_table_fill_count = 0;

		for (unsigned int i = 0; i < state.canvas.pixel_count(); i += 1) {
			uint32_t pixel_color = state.canvas.get_pixel_data(i);

			int index_in_color_table = -1;
			for (unsigned int j = 0; j < color_table_fill_count; j += 1) {
				uint32_t color_table_color = state.canvas.p_color_table->get_pixel_data(j);
				if (color_table_color == pixel_color) {
					index_in_color_table = j;
					break;
				}
			}

			uint32_t pixel_data_to_set = 0x000000;
			if (index_in_color_table != -1) {
				pixel_data_to_set = index_in_color_table;
			} else if (color_table_fill_count < color_table_size) {
				state.canvas.p_color_table->set_pixel_data(color_table_fill_count, pixel_color);
				pixel_data_to_set = color_table_fill_count;
				color_table_fill_count += 1;
			}
			state.canvas.set_pixel_data(i, pixel_data_to_set);
		}
	} else if (bits_per_pixel == 2) {
		state.canvas.p_color_table->height = 4;
		state.canvas.p_color_table->set_pixel_data(0, 0x000000);
		state.canvas.p_color_table->set_pixel_data(1, 0x444444);
		state.canvas.p_color_table->set_pixel_data(2, 0x888888);
		state.canvas.p_color_table->set_pixel_data(3, 0xFFFFFF);

		for (unsigned int i = 0; i < state.canvas.pixel_count(); i += 1)
		{
			uint32_t pixel_color = state.canvas.get_pixel_data(i);
			uint32_t pixel_data = (pixel_color > 0xCCCCCC) // midpoint between 0x888888 & 0xFFFFFF
				? 3
				: (pixel_color > 0x666666) // midpoint between 0x444444 & 0x888888
					? 2
					: (pixel_color > 0x222222) // midpoint between 0x000000 & 0x444444
						? 1
						: 0;
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

	state.pixels.camera_focus.reset();

	state.mode = View;
	the_clipboard.clear();

	ClearCommandBuffer();
	InitializeLayout(state);
	state.pixels.size();
	state.pixels.reset_items();
	state.color_table.size();
	state.color_table.reset_items();

	return 0;
}

int InitializeState(const tl::GameMemory& gameMemory, char* commandLine, int clientX, int clientY)
{
	state.windowWidth = clientX;
	state.windowHeight = clientY;

	// Define memory slices
	const uint64_t oneKiloByteInBytes = 1024;
	const uint64_t oneMegaByteInBytes = oneKiloByteInBytes * 1024;
	const unsigned int max_color_table_size_in_items = 256;
	constexpr uint64_t color_table_size_in_bytes = sizeof(uint32_t) * max_color_table_size_in_items;

	tl::MemorySpace working = gameMemory.permanent;
	tl::font_interface_initialize();

	paletteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	fileReadMemory = tl::CarveMemorySpace(oneMegaByteInBytes, working);
	state.canvas_color_table.init(tl::CarveMemorySpace(color_table_size_in_bytes, working));
	state.canvas.init(working); // left over memory goes to main sprite
	tempMemory = gameMemory.transient;

	state.canvas.p_color_table = &state.canvas_color_table;
	state.commandBuffer = &commandBuffer[0];
	state.pixels.initialize(&state.canvas);
	state.color_table.initialize(&state.canvas_color_table);

	InitializeLayout(state);
	InitializePalettes(paletteMemory, tempMemory, state);

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

static bool execute_command_insert_row(Grid& grid)
{
	if (can_insert_row(*grid.sprite))
	{
		insert_row_operation& insert_operation = the_undo.get_insert_row(&grid);
		insert_operation.execute();
		return true;
	}
	return false;
}

static bool execute_command_insert_column(Grid& grid)
{
	if (can_insert_column(*grid.sprite))
	{
		insert_column_operation& insert_operation = the_undo.get_insert_column(&grid);
		insert_operation.execute();
		return true;
	}
	return false;
}

static bool execute_command_delete_row(Grid& grid)
{
	if (can_delete_row(*grid.sprite))
	{
		delete_row_operation& delete_operation = the_undo.get_delete_row(&grid);
		delete_operation.execute();
		return true;
	}
	return false;
}

static bool execute_command_delete_column(Grid& grid)
{
	if (can_delete_column(*grid.sprite))
	{
		delete_column_operation& delete_operation = the_undo.get_delete_column(&grid);
		delete_operation.execute();
		return true;
	}
	return false;
}

static bool execute_command_cut(EditorState& editor_state)
{
	int copy_cursor_index = editor_state.pixels.cursor.index();
	int copy_range_index = (editor_state.mode == Visual) ? editor_state.pixels.range.index() : copy_cursor_index;

	Grid grid = editor_state.pixels;
	paste_pixel_data_operation& cut_operation = the_undo.get_paste_pixel_data(grid.sprite);
	cut_to_clipboard_operation(
		*grid.sprite,
		grid.cursor.index(),
		copy_range_index,
		cut_operation,
		the_clipboard
	);

	cut_operation.execute();

	return true;
}

static bool execute_command_paste(const Grid& grid)
{
	paste_pixel_data_operation& paste_operation  = the_undo.get_paste_pixel_data(grid.sprite);
	paste_from_clipboard_operation(
		grid,
		the_clipboard,
		paste_operation
	);
	paste_operation.execute();

	return true;
}

static bool execute_command_edit_pixel(Grid* grid, uint32_t parsed_color)
{
	set_pixel_data_operation& operation = the_undo.get_set_pixel_data(grid, parsed_color);
	operation.execute();
	return true;
}

static void ExecuteCurrentCommand()
{
	if (commands.get(0) != ':')
	{
		return;
	}

	if (CommandIs("r")) // append row
	{
		if (execute_command_insert_row(state.pixels))
		{
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("INSERT ROW FAILED!");
		}
		return;
	}
	else if (CommandIs("q"))
	{
		state.quit = true;
		return;
	}
	else if (CommandStartsWith("r")) // append multiple rows
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
			execute_command_insert_row(state.pixels);
		}
		ClearCommandBuffer();
	}
	else if (CommandIs("c")) // append column
	{
		if (execute_command_insert_column(state.pixels))
		{
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("INSERT COLUMN FAILED!");
		}
		return;
	}
	else if (CommandIs("p")) // switch palette
	{
		SwitchPalette(state);
		ClearCommandBuffer();
		return;
	}
	else if (CommandIs("dr")) // delete row
	{
		if (execute_command_delete_row(state.pixels))
		{
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("DELETE ROW FAILED!");
		}
		return;
	}
	else if (CommandIs("dc")) // delete column
	{
		if (execute_command_delete_column(state.pixels))
		{
			ClearCommandBuffer();
		}
		else
		{
			WriteStringToCommandBuffer("DELETE COLUMN FAILED!");
		}
		return;
	}
	else if (CommandIs("w") || CommandStartsWith("w ")) // write to file
	{
		char* targetFilePath = CommandIs("w") ? filePath : &commands.access(3);

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
	else if (CommandStartsWith("edit ")) // edit new file
	{
		Initialize(&commands.access(6));
		return;
	}
	else if (CommandStartsWith("e ")) // edit selected pixel
	{
		if (state.pixels_are_selected() && state.canvas.has_color_table())
		{
			return;
		}

		char* pointerToNumberChar = tl::GetNextNumberChar(&commands.access(1));
		uint32_t parsed_color;
		ParseColorFromCharArray(pointerToNumberChar, tempMemory, parsed_color);

		execute_command_edit_pixel(state.activeControl, parsed_color);

		ClearCommandBuffer();
		return;
	}
	else if (CommandStartsWith("bpp ")) // set bits per pixel
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
		execute_command_cut(state);
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
		execute_command_paste(state.pixels);
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
		commands.append(input.character);
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
		int camera_col_index = state.activeControl->camera_focus.column_index();
		int camera_row_index = state.activeControl->camera_focus.row_index();
		state.activeControl->cursor.set_index(camera_col_index, camera_row_index);
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

	if (input.buttons[tl::KEY_G].keyDown)
	{
		state.mode = ViewNoGrid;
		WriteStringToCommandBuffer("NO GRID");
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
		case ViewNoGrid:
			if (ApplyCameraMovementToState(input)) return state;
			break;
	}

	return state;
}

