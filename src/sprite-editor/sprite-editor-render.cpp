#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

static tl::Rect<float> spriteRect;
static tl::Rect<float> gridRect;
static tl::Rect<float> commandRect;
static tl::Rect<float> commandCharFootprint;
static tl::Rect<float> displayRect;
static tl::Rect<float> displayCharFootprint;
static tl::Rect<float> paletteRect;

void SizeGridForSprite(tl::SpriteC& sprite)
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

void InitializeLayout()
{
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

	float paletteHalfWidthPercent = 0.2f;
	float windowHalfWidth = (float)windowWidth * 0.5f;
	float visualYHalfSize = ((float)windowHeight * 0.5f) - commandRect.halfSize.y;
	spriteRect.halfSize = {
		windowHalfWidth * (1.0f - paletteHalfWidthPercent),
		visualYHalfSize
	};

	float visualYPosition = spriteRect.halfSize.y + commandRect.position.y + commandRect.halfSize.y;
	spriteRect.position = {
		spriteRect.halfSize.x,
		visualYPosition
	};

	paletteRect.halfSize = {
		windowHalfWidth * paletteHalfWidthPercent,
		visualYHalfSize
	};
	paletteRect.position = {
		spriteRect.position.x + spriteRect.halfSize.x + paletteRect.halfSize.x,
		visualYPosition
	};
}

void Render(const tl::RenderBuffer &renderBuffer, const EditorState state)
{
	// Render
	const uint32_t commandBackgroundColor = 0x000000;
	const uint32_t displayBackgroundColor = 0x666666;
	const uint32_t spriteBackgroundColor = 0x222222;
	const uint32_t paletteBackgroundColor = 0x888888;
	const uint32_t gridBorderColor = 0x444444;
	const uint32_t selectedPixelColor = 0xFFFF00;
	const uint32_t commandTextColor = 0xFFFFFF;
	const uint32_t displayTextColor = 0xFFFF00;
	const float pixelBorderWidth = 2.0f;

	tl::DrawRect(renderBuffer, commandBackgroundColor, commandRect);
	tl::DrawRect(renderBuffer, displayBackgroundColor, displayRect);
	tl::DrawRect(renderBuffer, spriteBackgroundColor, spriteRect);
	tl::DrawRect(renderBuffer, paletteBackgroundColor, paletteRect);
	tl::DrawRect(renderBuffer, gridBorderColor, gridRect);

	float pixelDimensionWithBorder = (2.0f * gridRect.halfSize.x) / state.sprite.width;
	float pixelDimension = pixelDimensionWithBorder - (2.0f * pixelBorderWidth);
	tl::Vec2<float> pixelHalfSize = { 0.5F * pixelDimension, 0.5f * pixelDimension };

	float yOriginalPosition = gridRect.position.y + gridRect.halfSize.y - (0.5f * pixelDimensionWithBorder);
	for (int j = 0; j < state.sprite.height; j += 1)
	{
		float yPosition = yOriginalPosition - (j * pixelDimensionWithBorder);
		for (int i = 0; i < state.sprite.width; i += 1)
		{
			float xPosition = (0.5f * pixelDimensionWithBorder) + (i * pixelDimensionWithBorder);

			tl::Vec2<float> pixelPosition = { xPosition, yPosition };
			tl::Rect<float> pixelFootPrint;
			pixelFootPrint.halfSize = pixelHalfSize;
			pixelFootPrint.position = pixelPosition;

			int pixelIndex = (j * state.sprite.width) + i;
			if (pixelIndex == state.selectedPixelIndex)
			{
				tl::Rect<float> selectedFootprint;
				selectedFootprint.position = pixelPosition;
				selectedFootprint.halfSize = { pixelHalfSize.x + 1, pixelHalfSize.y + 1 };
				tl::DrawRect(renderBuffer, selectedPixelColor, selectedFootprint);
			}
			tl::Color blockColor = state.sprite.content[pixelIndex];

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
		state.commandBuffer,
		commandCharFootprint,
		commandTextColor
	);

	tl::DrawAlphabetCharacters(
		renderBuffer,
		state.displayBuffer,
		displayCharFootprint,
		displayTextColor
	);
}