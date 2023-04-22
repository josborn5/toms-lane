#include "../platform/toms-lane-platform.hpp"
#include "../win32/toms-lane-win32.hpp"
#include "./sprite-editor-win32.cpp"
#include "./sprite-operations.cpp"
#include "./sprite-commands.cpp"

#define COMMAND_BUFFER_SIZE 15

static tl::SpriteC sprite;
static tl::Rect<float> spriteRect;
static tl::Rect<float> gridRect;
static tl::Rect<float> commandRect;
static tl::Rect<float> commandCharFootprint;

static int selectedPixelIndex = 0;

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

static char commandBuffer[COMMAND_BUFFER_SIZE];
tl::HeapArray<char> commands = tl::HeapArray<char>(commandBuffer, COMMAND_BUFFER_SIZE);

static void ClearCommandBuffer()
{
	for (int i = 0; i < commands.capacity; i += 1)
	{
		commands.content[i] = '\0';
		commands.clear();
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

	uint64_t fileSize = 0;
	if (tl::GetFileSize(filePath, fileSize) != tl::Success)
	{
		return 1;
	}

	if (tl::ReadFile(filePath, fileReadMemory) != tl::Success)
	{
		return 1;
	}

	ClearCommandBuffer();

	commandRect.halfSize = {
		(float)windowWidth * 0.5f,
		50.0f
	};
	commandRect.position = tl::CopyVec2(commandRect.halfSize);
	commandCharFootprint.halfSize = { 0.5f * commandRect.halfSize.y, commandRect.halfSize.y };
	commandCharFootprint.position = tl::CopyVec2(commandCharFootprint.halfSize);
	spriteRect.halfSize = {
		commandRect.halfSize.x,
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
			case 'S':
				if (commandBuffer[1] == '\0')
				{
					Save(gameMemory, sprite, commandBuffer);
				}
				break;
			case 'R':
				if (commandBuffer[1] == '\0')
				{
					ClearCommandBuffer();
					AppendRowToSpriteC(sprite, spriteMemory);
					SizeGridForSprite();
				}
				break;
			case 'C':
				if (commandBuffer[1] == '\0')
				{
					ClearCommandBuffer();
					AppendColumnToSpriteC(sprite, spriteMemory);
					SizeGridForSprite();
				}
				break;
			case 'E':
				char* pointer = &commandBuffer[1];
				int rValue = 0;
				int gValue = 0;
				int bValue = 0;
				if (*pointer)
				{
					rValue = CharStringToInt(pointer);
					pointer = GetNextNumberChar(pointer);
				}
				if (*pointer)
				{
					gValue = CharStringToInt(pointer);
					pointer = GetNextNumberChar(pointer);
				}
				if (*pointer)
				{
					bValue = CharStringToInt(pointer);
				}
				// TODO: parse alpha value!

				tl::Color color;
				color.r = (float)rValue / 255.0f;
				color.g = (float)gValue / 255.0f;
				color.b = (float)bValue / 255.0f;
				color.a = 1.0f;
				sprite.content[selectedPixelIndex] = color;

				ClearCommandBuffer();
				break;
		}
	}

	// Render
	const uint32_t commandBackgroundColor = 0x000000;
	const uint32_t spriteBackgroundColor = 0x222222;
	const uint32_t gridBorderColor = 0x444444;
	const uint32_t selectedPixelColor = 0xFFFF00;
	const uint32_t commandTextColor = 0xFFFFFF;
	const float pixelBorderWidth = 2.0f;

	tl::DrawRect(renderBuffer, commandBackgroundColor, commandRect);
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

	return 0;
}
