#include "../platform/toms-lane-platform.hpp"
#include "../win32/toms-lane-win32.hpp"
#include "./sprite-editor-win32.cpp"

#define COMMAND_BUFFER_SIZE 5

static tl::SpriteC sprite;
static tl::Rect<float> spriteRect;
static tl::Rect<float> gridRect;
static tl::Rect<float> commandRect;
static tl::Rect<float> commandCharFootprint;

static char commandBuffer[COMMAND_BUFFER_SIZE];
tl::HeapArray<char> commands = tl::HeapArray<char>(commandBuffer, COMMAND_BUFFER_SIZE);

int tl::Initialize(const GameMemory& gameMemory, const RenderBuffer& renderBuffer)
{
	// Load file
	tl::MemorySpace fileReadMemory = gameMemory.permanent;
	tl::MemorySpace tempMemory = gameMemory.transient;

	uint64_t fileSize = 0;
	if (tl::GetFileSize(filePath, fileSize) != tl::Success)
	{
		return 1;
	}

	if (tl::ReadFile(filePath, fileReadMemory) != tl::Success)
	{
		return 1;
	}

	for (int i = 0; i < COMMAND_BUFFER_SIZE; i += 1)
	{
		commandBuffer[i] = '\0';
	}

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
	uint64_t spriteCharArrayLength = fileSize / sizeof(char);
	char* addressAfterSpriteCharArray = &spriteCharArray[spriteCharArrayLength];

	sprite.content = (tl::Color*)addressAfterSpriteCharArray;
	tl::LoadSpriteC(spriteCharArray, tempMemory, sprite);

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
	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	// Update command buffer from input
	if (tl::IsReleased(input, tl::KEY_S))
	{
		if (commands.length < commands.capacity)
		{
			commands.append('S');
		}
	}
	else if (tl::IsReleased(input, tl::KEY_ENTER) || tl::IsReleased(input, tl::KEY_ESCAPE))
	{
		for (int i = 0; i < commands.capacity; i += 1)
		{
			commands.content[i] = '\0';
			commands.clear();
		}
	}

	// Render
	const uint32_t commandBackgroundColor = 0x000000;
	const uint32_t spriteBackgroundColor = 0x222222;
	const uint32_t gridBorderColor = 0x444444;
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
