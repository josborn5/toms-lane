#include "../tl-library.hpp"
#include "./editor.hpp"

static bool hasCopied = false;

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

void UpdateCurrentColor(EditorState& state)
{
	state.currentColor = state.palette->content[state.selectedPalettePixelIndex];
}

void ProcessCursorMovementInput(const tl::Input &input, EditorState& state)
{
	tl::SpriteC& activeSprite = (state.activeControl == SpriteGrid) ? state.sprite : *state.palette;
	int& activeIndex = (state.activeControl == SpriteGrid) ? state.selectedPixelIndex : state.selectedPalettePixelIndex;
	MoveCursorForSprite(input, activeSprite, activeIndex);
	UpdateCurrentColor(state);
}

void ProcessActiveControl(const tl::Input &input, EditorState& state)
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

void ClearCommandBuffer(EditorState& state)
{
	tl::array<char> commands = *state.commandBuffer;
	for (int i = 0; i < commands.capacity(); i += 1)
	{
		commands.access(i) = '\0';
		commands.clear();
	}
}

void ClearDisplayBuffer(EditorState& state)
{
	tl::array<char> display = *state.displayBuffer;
	for (int i = 0; i < display.capacity(); i += 1)
	{
		display.access(i) = '\0';
		display.clear();
	}
}


void ProcessKeyboardInput(const tl::Input& input, EditorState& state, const tl::GameMemory& gameMemory, const tl::MemorySpace& spriteMemory)
{
	tl::array<char>& commands = *state.commandBuffer;
	tl::array<char>& display = *state.displayBuffer;

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
			ClearCommandBuffer(state);
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
						Save(gameMemory, state.sprite, displayString, state);
					}
					else if (commands.get(1) == ' ' && commands.get(2)) // save to new filePath
					{
						state.filePath = &commands.access(2);
						Save(gameMemory, state.sprite, displayString, state);
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
					tl::MemorySpace transient = gameMemory.transient;
					ParseColorFromCharArray(pointer, transient, state.sprite.content[state.selectedPixelIndex]);
					ClearCommandBuffer(state);
					break;
				}
				case 'I': // inspect color of selected pixel
				{
					if (commands.get(1) == '\0')
					{
						ClearDisplayBuffer(state);
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
						state.sprite.content[state.selectedPixelIndex] = state.currentColor;
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
			ClearCommandBuffer(state);
		}
	}
	else
	{
		if (tl::IsReleased(input, tl::KEY_C))
		{
			hasCopied = true;
			state.copiedColor = state.sprite.content[state.selectedPixelIndex];
			ClearDisplayBuffer(state);
			display.append('C');
			display.append('O');
			display.append('P');
			display.append('Y');
		}
		else if (hasCopied && tl::IsReleased(input, tl::KEY_V))
		{
			state.sprite.content[state.selectedPixelIndex] = state.copiedColor;
			ClearDisplayBuffer(state);
			display.append('P');
			display.append('A');
			display.append('S');
			display.append('T');
			display.append('E');
		}
	}
}
