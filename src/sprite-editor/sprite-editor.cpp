#include <windows.h>
#include "../win32/toms-lane-win32.hpp"
#include "../win32/win32-memory.hpp"

char* filePath;
const int windowWidth = 800;
const int windowHeight = 600;

#include "../platform/toms-lane-platform.hpp"
#include "./sprite-operations.cpp"
#include "./sprite-commands.cpp"
#include "./editor.hpp"
#include "./render.cpp"
#include "./sprite-editor-palettes.cpp"
#include "./input-processing.cpp"

#define COMMAND_BUFFER_SIZE 15
#define DISPLAY_BUFFER_SIZE 15

tl::GameMemory appMemory;

static char commandBuffer[COMMAND_BUFFER_SIZE];
static char displayBuffer[DISPLAY_BUFFER_SIZE];
tl::HeapArray<char> commands = tl::HeapArray<char>(commandBuffer, COMMAND_BUFFER_SIZE);
tl::HeapArray<char> display = tl::HeapArray<char>(displayBuffer, DISPLAY_BUFFER_SIZE);

EditorState state;

static bool hasCopied = false;

static tl::MemorySpace spriteMemory;
static tl::MemorySpace paletteMemory;

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
		commands.content[i] = '\0';
		commands.clear();
	}
}

static void ClearDisplayBuffer()
{
	for (int i = 0; i < display.capacity(); i += 1)
	{
		display.content[i] = '\0';
		display.clear();
	}
}

int tl::Initialize(const GameMemory& gameMemory, const RenderBuffer& renderBuffer)
{
	state.commandBuffer = commandBuffer;
	state.displayBuffer = displayBuffer;

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
		if (tl::win32_file_interface_size_get(filePath, fileSize) != tl::Success)
		{
			return 1;
		}

		if (tl::win32_file_interface_read(filePath, fileReadMemory) != tl::Success)
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

	InitializeLayout();
	InitializePalettes(paletteMemory, tempMemory, state);

	char* spriteCharArray = (char*)fileReadMemory.content;
	state.sprite.content = (tl::Color*)spriteMemory.content;
	tl::LoadSpriteC(spriteCharArray, tempMemory, state.sprite);

	SizeGridForSprite(state.sprite);
	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	ProcessActiveControl(input, state);
	ProcessCursorMovementInput(input, state);

	state.mouse.x = input.mouse.x;
	state.mouse.y = input.mouse.y;

	if (!input.buttons[KEY_CTRL].isDown)
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
			switch (commandBuffer[0])
			{
				case 'S': // save
				{
					if (commandBuffer[1] == '\0') // save to current filePath
					{
						Save(gameMemory, state.sprite, displayBuffer);
					}
					else if (commandBuffer[1] == ' ' && commandBuffer[2]) // save to new filePath
					{
						filePath = &commandBuffer[2];
						Save(gameMemory, state.sprite, displayBuffer);
					}
					break;
				}
				case 'R': // append row
				{
					if (commandBuffer[1] == '\0')
					{
						AppendRowToSpriteC(state.sprite, spriteMemory); // TODO: make the MemorySpace a field of the SpriteC struct. The pointer to the sprite content is shared between the two - make it a single pointer owner!
						SizeGridForSprite(state.sprite);
					}
					break;
				}
				case 'C': // append column
				{
					if (commandBuffer[1] == '\0')
					{
						AppendColumnToSpriteC(state.sprite, spriteMemory);
						SizeGridForSprite(state.sprite);
					}
					break;
				}
				case 'E': // edit color of selected pixel
				{
					char* pointer = GetNextNumberChar(&commandBuffer[1]);
					tl::MemorySpace transient = gameMemory.transient;
					ParseColorFromCharArray(pointer, transient, state.sprite.content[state.selectedPixelIndex]);
					ClearCommandBuffer();
					break;
				}
				case 'I': // inspect color of selected pixel
				{
					if (commandBuffer[1] == '\0')
					{
						ClearDisplayBuffer();
						tl::Color selectedColor = state.sprite.content[state.selectedPixelIndex];

						int color = (int)(selectedColor.r * 255.0f);
						char* cursor = display.content;
						IntToCharString(color, cursor);

						while (*cursor) cursor++;
						*cursor = ' ';
						cursor++;

						color = (int)(selectedColor.g * 255.0f);
						IntToCharString(color, cursor);

						while (*cursor) cursor++;
						*cursor = ' ';
						cursor++;

						color = (int)(selectedColor.b * 255.0f);
						IntToCharString(color, cursor);
					}
					break;
				}
				case 'P': // switch palette
				{
					if (commandBuffer[1] == '\0')
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
					if (commandBuffer[1] == 'R' && commandBuffer[2] == '\0' && state.sprite.height > 1)
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
					else if (commandBuffer[1] == 'C' && commandBuffer[2] == '\0' && state.sprite.width > 1)
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
			state.copiedColor = state.sprite.content[state.selectedPixelIndex];
			ClearDisplayBuffer();
			display.append('C');
			display.append('O');
			display.append('P');
			display.append('Y');
		}
		else if (hasCopied && tl::IsReleased(input, tl::KEY_V))
		{
			state.sprite.content[state.selectedPixelIndex] = state.copiedColor;
			ClearDisplayBuffer();
			display.append('P');
			display.append('A');
			display.append('S');
			display.append('T');
			display.append('E');
		}
	}

	Render(renderBuffer, state);

	return 0;
}

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return tl::UpdateAndRender(appMemory, input, renderBuffer, dt);
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Sprite Editor";
	settings.width = 800;
	settings.height = 600;

	if (*commandLine)
	{
		filePath = commandLine;
	}

	int windowOpenResult = tl::OpenWindow(instance, settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::InitializeMemory(
		2,
		2,
		appMemory
	);

	tl::RenderBuffer garbage;
	tl::Initialize(appMemory, garbage);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
