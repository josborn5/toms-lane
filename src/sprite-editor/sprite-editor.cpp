#include "../platform/toms-lane-platform.hpp"
#include "../win32/toms-lane-win32.hpp"
#include "./sprite-editor-win32.cpp"

tl::SpriteC sprite;
tl::Rect<float> spriteRect;
tl::Rect<float> gridRect;
tl::Rect<float> commandRect;

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

	commandRect.halfSize = {
		(float)windowWidth * 0.5f,
		50.0f
	};
	commandRect.position = tl::CopyVec2(commandRect.halfSize);
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
	uint32_t commandBackgroundColor = 0xAA0000;
	tl::DrawRect(renderBuffer, commandBackgroundColor, commandRect);
	
	uint32_t spriteBackgroundColor = 0x0000AA;
	tl::DrawRect(renderBuffer, spriteBackgroundColor, spriteRect);
	
	uint32_t gridBorderColor = 0x00AA00;
	tl::DrawRect(renderBuffer, gridBorderColor, gridRect);

	float pixelBorderWidth = 5.0f;
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

	tl::Rect<float> numberFoot;
	numberFoot.halfSize = { 10.0f, 20.0f };
	numberFoot.position = { 400.0f, 200.0f };
	tl::DrawNumber(
		renderBuffer,
		sprite.width,
		numberFoot,
		0xFFFFFF
	);
	numberFoot.position.y -= 30.0f;
	tl::DrawNumber(
		renderBuffer,
		sprite.height,
		numberFoot,
		0xFFFFFF
	);
	return 0;
}
