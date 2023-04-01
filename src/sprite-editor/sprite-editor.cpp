#include "../platform/toms-lane-platform.hpp"
#include "../win32/toms-lane-win32.hpp"
#include "./sprite-editor-win32.cpp"

tl::SpriteC sprite;
tl::Rect<float> rootGrid;

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

	char* spriteCharArray = (char*)fileReadMemory.content;
	uint64_t spriteCharArrayLength = fileSize / sizeof(char);
	char* addressAfterSpriteCharArray = &spriteCharArray[spriteCharArrayLength];

	sprite.content = (tl::Color*)addressAfterSpriteCharArray;
	tl::LoadSpriteC(spriteCharArray, tempMemory, sprite);

	float aspectRatio = (float)sprite.height / (float)sprite.width;
	if (aspectRatio >= 1)
	{
		rootGrid.halfSize.y = 600.0f * 0.5f;
		rootGrid.halfSize.x = aspectRatio * rootGrid.halfSize.y * 0.5f;
	}
	else
	{
		rootGrid.halfSize.x = 800;
		rootGrid.halfSize.y = aspectRatio * rootGrid.halfSize.x * 0.5f;
	}

	rootGrid.position = tl::CopyVec2(rootGrid.halfSize);
	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	uint32_t gridBorderColor = 0x444444;
	tl::DrawRect(renderBuffer, gridBorderColor, rootGrid);

	float pixelBorderWidth = 5.0f;
	float pixelDimensionWithBorder = (rootGrid.halfSize.x) / sprite.width;
	float pixelDimension = pixelDimensionWithBorder - (2.0f * pixelBorderWidth);
	tl::Vec2<float> pixelHalfSize = { 0.5F * pixelDimension, 0.5f * pixelDimension };

	float yOriginalPosition = (2.0f * rootGrid.halfSize.y) - (0.5f * pixelDimensionWithBorder);
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

			tl::DrawRect(renderBuffer, 0xFF0000, pixelFootPrint);
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
