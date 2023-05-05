#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

static tl::Rect<float> spriteContainerRect;
static tl::Rect<float> spriteBoundingRect;
static tl::Rect<float> commandTextRect;
static tl::Rect<float> commandCharFootprint;
static tl::Rect<float> displayTextRect;
static tl::Rect<float> displayCharFootprint;
static tl::Rect<float> paletteContainerRect;

tl::Rect<float> SizeBoundingRectForSpriteInContainingRect(const tl::SpriteC& sprite, const tl::Rect<float> boundingRect)
{
	tl::Rect<float> sizeRect;
	float spriteAspectRatio = (float)sprite.height / (float)sprite.width;
	float backgroundAspectRatio = boundingRect.halfSize.y / boundingRect.halfSize.x;
	float relativeAspectRatio = spriteAspectRatio / backgroundAspectRatio;
	if (relativeAspectRatio >= 1.0f)
	{
		sizeRect.halfSize.y = spriteContainerRect.halfSize.y;
		sizeRect.halfSize.x = sizeRect.halfSize.y * (float)sprite.width / (float)sprite.height;
	}
	else
	{
		sizeRect.halfSize.x = spriteContainerRect.halfSize.x;
		sizeRect.halfSize.y = sizeRect.halfSize.x * (float)sprite.height / (float)sprite.width;
	}

	sizeRect.position = {
		spriteContainerRect.position.x,
		spriteContainerRect.position.y
	};

	return sizeRect;
}

void SizeGridForSprite(tl::SpriteC& sprite)
{
	spriteBoundingRect = SizeBoundingRectForSpriteInContainingRect(sprite, spriteContainerRect);
}

void InitializeLayout()
{
	const float textAreaHeight = 30.0f;
	commandTextRect.halfSize = {
		(float)windowWidth * 0.25f,
		textAreaHeight
	};
	commandTextRect.position = tl::CopyVec2(commandTextRect.halfSize);
	commandCharFootprint.halfSize = { 0.3f * commandTextRect.halfSize.y, commandTextRect.halfSize.y };
	commandCharFootprint.position = tl::CopyVec2(commandCharFootprint.halfSize);

	displayTextRect.halfSize = tl::CopyVec2(commandTextRect.halfSize);
	displayTextRect.position = {
		commandTextRect.position.x + commandTextRect.halfSize.x + displayTextRect.halfSize.x,
		displayTextRect.halfSize.y
	};
	displayCharFootprint.halfSize = tl::CopyVec2(commandCharFootprint.halfSize);
	displayCharFootprint.position = {
		displayTextRect.position.x - displayTextRect.halfSize.x + displayCharFootprint.halfSize.x,
		displayTextRect.halfSize.y
	};

	float paletteHalfWidthPercent = 0.2f;
	float windowHalfWidth = (float)windowWidth * 0.5f;
	float visualYHalfSize = ((float)windowHeight * 0.5f) - commandTextRect.halfSize.y;
	spriteContainerRect.halfSize = {
		windowHalfWidth * (1.0f - paletteHalfWidthPercent),
		visualYHalfSize
	};

	float visualYPosition = spriteContainerRect.halfSize.y + commandTextRect.position.y + commandTextRect.halfSize.y;
	spriteContainerRect.position = {
		spriteContainerRect.halfSize.x,
		visualYPosition
	};

	paletteContainerRect.halfSize = {
		windowHalfWidth * paletteHalfWidthPercent,
		visualYHalfSize
	};
	paletteContainerRect.position = {
		spriteContainerRect.position.x + spriteContainerRect.halfSize.x + paletteContainerRect.halfSize.x,
		visualYPosition
	};
}

void Render(const tl::RenderBuffer &renderBuffer, const EditorState state)
{
	// Render
	const uint32_t commandBackgroundColor = 0x000000;
	const uint32_t displayBackgroundColor = 0x111111;
	const uint32_t spriteBackgroundColor = 0x222222;
	const uint32_t paletteBackgroundColor = 0x333333;
	const uint32_t selectedPixelColor = 0xFFFF00;
	const uint32_t commandTextColor = 0xFFFFFF;
	const uint32_t displayTextColor = 0xFFFF00;
	const float pixelBorderWidth = 2.0f;

	tl::DrawRect(renderBuffer, commandBackgroundColor, commandTextRect);
	tl::DrawRect(renderBuffer, displayBackgroundColor, displayTextRect);
	tl::DrawRect(renderBuffer, spriteBackgroundColor, spriteContainerRect);
	tl::DrawRect(renderBuffer, paletteBackgroundColor, paletteContainerRect);

	float pixelDimensionWithBorder = (2.0f * spriteBoundingRect.halfSize.x) / state.sprite.width;
	float pixelDimension = pixelDimensionWithBorder - (2.0f * pixelBorderWidth);
	tl::Vec2<float> pixelHalfSize = { 0.5F * pixelDimension, 0.5f * pixelDimension };

	float yOriginalPosition = spriteBoundingRect.position.y + spriteBoundingRect.halfSize.y - (0.5f * pixelDimensionWithBorder);
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