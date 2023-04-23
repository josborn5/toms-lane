#include "../platform/toms-lane-platform.hpp"
#include "../win32/toms-lane-win32.hpp"
#include "./sprite-editor-win32.cpp"
#include "./sprite-operations.cpp"
#include "./sprite-commands.cpp"

#define COMMAND_BUFFER_SIZE 15
#define DISPLAY_BUFFER_SIZE 15

static char commandBuffer[COMMAND_BUFFER_SIZE];
static char displayBuffer[DISPLAY_BUFFER_SIZE];
tl::HeapArray<char> commands = tl::HeapArray<char>(commandBuffer, COMMAND_BUFFER_SIZE);
tl::HeapArray<char> display = tl::HeapArray<char>(displayBuffer, DISPLAY_BUFFER_SIZE);

static tl::SpriteC sprite;
static tl::Rect<float> spriteRect;
static tl::Rect<float> gridRect;
static tl::Rect<float> commandRect;
static tl::Rect<float> commandCharFootprint;
static tl::Rect<float> displayRect;
static tl::Rect<float> displayCharFootprint;

static int selectedPixelIndex = 0;
static bool hasCopied = false;
static tl::Color copiedColor;

static tl::MemorySpace spriteMemory;

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
	for (int i = 0; i < commands.capacity; i += 1)
	{
		commands.content[i] = '\0';
		commands.clear();
	}
}

static void ClearDisplayBuffer()
{
	for (int i = 0; i < display.capacity; i += 1)
	{
		display.content[i] = '\0';
		display.clear();
	}
}

static void SizeGridForSprite()
{
	float spriteAspectRatio = (float)sprite.height / (float)sprite.width;
	float backgroundAspectRatio = spriteRect.halfSize.y / spriteRect.halfSize.x;
	float relativeAspectRatio = spriteAspectRatio / backgroundAspectRatio;
	if (relativeAspectRatio >= 1.0f)
	{
		gridRect.halfSize.y = spriteRect.halfSize.y;
		gridRect.halfSize.x = gridRect.halfSize.y * (float)sprite.width / (float)sprite.height;
	}
	else
	{
		gridRect.halfSize.x = spriteRect.halfSize.x;
		gridRect.halfSize.y = gridRect.halfSize.x * (float)sprite.height / (float)sprite.width;
	}

	gridRect.position = {
		gridRect.halfSize.x,
		gridRect.halfSize.y + commandRect.position.y + commandRect.halfSize.y
	};
}

int tl::Initialize(const GameMemory& gameMemory, const RenderBuffer& renderBuffer)
{
	// Load file
	spriteMemory = gameMemory.permanent;
	tl::MemorySpace fileReadMemory;
	fileReadMemory.content = gameMemory.transient.content;
	fileReadMemory.sizeInBytes = 512;
	tl::MemorySpace tempMemory;
	tempMemory.content = (char*)gameMemory.transient.content + (fileReadMemory.sizeInBytes * sizeof(char));
	tempMemory.sizeInBytes = 512;

	if (filePath)
	{
		uint64_t fileSize = 0;
		if (tl::GetFileSize(filePath, fileSize) != tl::Success)
		{
			return 1;
		}

		if (tl::ReadFile(filePath, fileReadMemory) != tl::Success)
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

	const float textAreaHeight = 30.0f;
	commandRect.halfSize = {
		(float)windowWidth * 0.25f,
		textAreaHeight
	};
	commandRect.position = tl::CopyVec2(commandRect.halfSize);
	commandCharFootprint.halfSize = { 0.3f * commandRect.halfSize.y, commandRect.halfSize.y };
	commandCharFootprint.position = tl::CopyVec2(commandCharFootprint.halfSize);

	displayRect.halfSize = tl::CopyVec2(commandRect.halfSize);
	displayRect.position = {
		commandRect.position.x + commandRect.halfSize.x + displayRect.halfSize.x,
		displayRect.halfSize.y
	};
	displayCharFootprint.halfSize = tl::CopyVec2(commandCharFootprint.halfSize);
	displayCharFootprint.position = {
		displayRect.position.x - displayRect.halfSize.x + displayCharFootprint.halfSize.x,
		displayRect.halfSize.y
	};

	spriteRect.halfSize = {
		(float)windowWidth * 0.5f,
		((float)windowHeight * 0.5f) - commandRect.halfSize.y
	};
	spriteRect.position = {
		spriteRect.halfSize.x,
		spriteRect.halfSize.y + commandRect.position.y + commandRect.halfSize.y
	};

	char* spriteCharArray = (char*)fileReadMemory.content;
	sprite.content = (tl::Color*)spriteMemory.content;
	tl::LoadSpriteC(spriteCharArray, tempMemory, sprite);

	SizeGridForSprite();
	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	// Check for arrow key press to move selected pixel
	int maxPixelIndex = (sprite.width * sprite.height) - 1;
	if (!input.buttons[KEY_CTRL].isDown)
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

		// Update command buffer from input
		if (commands.length < commands.capacity)
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
						Save(gameMemory, sprite, displayBuffer);
					}
					else if (commandBuffer[1] == ' ' && commandBuffer[2]) // save to new filePath
					{
						filePath = &commandBuffer[2];
						Save(gameMemory, sprite, displayBuffer);
					}
					break;
				}
				case 'R': // append row
				{
					if (commandBuffer[1] == '\0')
					{
						AppendRowToSpriteC(sprite, spriteMemory);
						SizeGridForSprite();
					}
					break;
				}
				case 'C': // append column
				{
					if (commandBuffer[1] == '\0')
					{
						AppendColumnToSpriteC(sprite, spriteMemory);
						SizeGridForSprite();
					}
					break;
				}
				case 'E': // edit color of selected pixel
				{
					char* pointer = GetNextNumberChar(&commandBuffer[1]);
					tl::MemorySpace transient = gameMemory.transient;
					ParseColorFromCharArray(pointer, transient, sprite.content[selectedPixelIndex]);
					ClearCommandBuffer();
					break;
				}
				case 'I': // inspect color of selected pixel
				{
					if (commandBuffer[1] == '\0')
					{
						ClearDisplayBuffer();
						tl::Color selectedColor = sprite.content[selectedPixelIndex];

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
			}
			ClearCommandBuffer();
		}
	}
	else
	{
		if (tl::IsReleased(input, tl::KEY_C))
		{
			hasCopied = true;
			copiedColor = sprite.content[selectedPixelIndex];
			ClearDisplayBuffer();
			display.append('C');
			display.append('O');
			display.append('P');
			display.append('Y');
		}
		else if (hasCopied && tl::IsReleased(input, tl::KEY_V))
		{
			sprite.content[selectedPixelIndex] = copiedColor;
			ClearDisplayBuffer();
			display.append('P');
			display.append('A');
			display.append('S');
			display.append('T');
			display.append('E');
		}
	}

	// Render
	const uint32_t commandBackgroundColor = 0x000000;
	const uint32_t displayBackgroundColor = 0x666666;
	const uint32_t spriteBackgroundColor = 0x222222;
	const uint32_t gridBorderColor = 0x444444;
	const uint32_t selectedPixelColor = 0xFFFF00;
	const uint32_t commandTextColor = 0xFFFFFF;
	const uint32_t displayTextColor = 0xFFFF00;
	const float pixelBorderWidth = 2.0f;

	tl::DrawRect(renderBuffer, commandBackgroundColor, commandRect);
	tl::DrawRect(renderBuffer, displayBackgroundColor, displayRect);
	tl::DrawRect(renderBuffer, spriteBackgroundColor, spriteRect);
	tl::DrawRect(renderBuffer, gridBorderColor, gridRect);

	float pixelDimensionWithBorder = (2.0f * gridRect.halfSize.x) / sprite.width;
	float pixelDimension = pixelDimensionWithBorder - (2.0f * pixelBorderWidth);
	tl::Vec2<float> pixelHalfSize = { 0.5F * pixelDimension, 0.5f * pixelDimension };

	float yOriginalPosition = gridRect.position.y + gridRect.halfSize.y - (0.5f * pixelDimensionWithBorder);
	for (int j = 0; j < sprite.height; j += 1)
	{
		float yPosition = yOriginalPosition - (j * pixelDimensionWithBorder);
		for (int i = 0; i < sprite.width; i += 1)
		{
			float xPosition = (0.5f * pixelDimensionWithBorder) + (i * pixelDimensionWithBorder);

			tl::Vec2<float> pixelPosition = { xPosition, yPosition };
			tl::Rect<float> pixelFootPrint;
			pixelFootPrint.halfSize = pixelHalfSize;
			pixelFootPrint.position = pixelPosition;

			int pixelIndex = (j * sprite.width) + i;
			if (pixelIndex == selectedPixelIndex)
			{
				tl::Rect<float> selectedFootprint;
				selectedFootprint.position = pixelPosition;
				selectedFootprint.halfSize = { pixelHalfSize.x + 1, pixelHalfSize.y + 1 };
				tl::DrawRect(renderBuffer, selectedPixelColor, selectedFootprint);
			}
			tl::Color blockColor = sprite.content[pixelIndex];

			uint32_t color = tl::GetColorFromRGB(
				(int)(255.0f * blockColor.r),
				(int)(255.0f * blockColor.g),
				(int)(255.0f * blockColor.b)
			);
			tl::DrawRect(renderBuffer, color, pixelFootPrint);
		}
	}

	tl::DrawAlphabetCharacters(
		renderBuffer,
		commandBuffer,
		commandCharFootprint,
		commandTextColor
	);

	tl::DrawAlphabetCharacters(
		renderBuffer,
		displayBuffer,
		displayCharFootprint,
		displayTextColor
	);

	return 0;
}
