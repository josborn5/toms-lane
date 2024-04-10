#include "../tl-library.hpp"
#include "./editor.hpp"

static tl::Rect<float> spriteContainerRect;
static tl::Rect<float> spriteBoundingRect;
static tl::Rect<float> commandTextRect;
static tl::Rect<float> commandCharFootprint;
static tl::Rect<float> displayTextRect;
static tl::Rect<float> displayCharFootprint;
static tl::Rect<float> paletteContainerRect;
static tl::Rect<float> paletteBoundingRect;
static tl::Rect<float> modeTextRect;
static tl::Rect<float> modeTextCharFootprint;

static tl::Rect<float> SizeBoundingRectForSpriteInContainingRect(const tl::SpriteC& sprite, const tl::Rect<float> containerRect)
{
	tl::Rect<float> sizeRect;
	float spriteAspectRatio = (float)sprite.height / (float)sprite.width;
	float backgroundAspectRatio = containerRect.halfSize.y / containerRect.halfSize.x;
	float relativeAspectRatio = spriteAspectRatio / backgroundAspectRatio;
	if (relativeAspectRatio >= 1.0f)
	{
		sizeRect.halfSize.y = containerRect.halfSize.y;
		sizeRect.halfSize.x = sizeRect.halfSize.y * (float)sprite.width / (float)sprite.height;
	}
	else
	{
		sizeRect.halfSize.x = containerRect.halfSize.x;
		sizeRect.halfSize.y = sizeRect.halfSize.x * (float)sprite.height / (float)sprite.width;
	}

	sizeRect.position = {
		containerRect.position.x,
		containerRect.position.y
	};

	return sizeRect;
}

static bool MouseIsInRect(const tl::Vec2<int>& mouse, const tl::Rect<float> rect)
{
	int minX = (int)rect.position.x - (int)rect.halfSize.x;
	int maxX = (int)rect.position.x + (int)rect.halfSize.x;
	int minY = (int)rect.position.y - (int)rect.halfSize.y;
	int maxY = (int)rect.position.y + (int)rect.halfSize.y;
	return mouse.x > minX && mouse.x < maxX && mouse.y > minY && mouse.y < maxY;
}

static void RenderSpriteAsGrid(
	const tl::SpriteC& sprite,
	const tl::Rect<float>& boundingRect,
	const tl::RenderBuffer& renderBuffer,
	int selectedBlockIndex,
	const tl::Vec2<int> mouse
) {
	const float pixelBorderWidth = 2.0f;
	const uint32_t selectedPixelColor = 0xFFFF00;

	float pixelDimensionWithBorder = (2.0f * boundingRect.halfSize.x) / sprite.width;
	float pixelDimension = pixelDimensionWithBorder - (2.0f * pixelBorderWidth);
	tl::Vec2<float> pixelHalfSize = { 0.5F * pixelDimension, 0.5f * pixelDimension };

	float yOriginalPosition = boundingRect.position.y + boundingRect.halfSize.y - (0.5f * pixelDimensionWithBorder);
	for (int j = 0; j < sprite.height; j += 1)
	{
		float yPosition = yOriginalPosition - (j * pixelDimensionWithBorder);
		for (int i = 0; i < sprite.width; i += 1)
		{
			float xPosition = boundingRect.position.x - boundingRect.halfSize.x + (0.5f * pixelDimensionWithBorder) + (i * pixelDimensionWithBorder);

			tl::Vec2<float> pixelPosition = { xPosition, yPosition };
			tl::Rect<float> pixelFootPrint;
			pixelFootPrint.halfSize = pixelHalfSize;
			pixelFootPrint.position = pixelPosition;

			int pixelIndex = (j * sprite.width) + i;
			if (pixelIndex == selectedBlockIndex || MouseIsInRect(mouse, pixelFootPrint))
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
}

void SizeGridForSprite(const tl::SpriteC& sprite)
{
	spriteBoundingRect = SizeBoundingRectForSpriteInContainingRect(sprite, spriteContainerRect);
}

void SizePalette(const tl::SpriteC& palette)
{
	paletteBoundingRect = SizeBoundingRectForSpriteInContainingRect(palette, paletteContainerRect);
}

static void PlaceRectToRightOfRect(const tl::Rect<float>& rect, tl::Rect<float>& toPlace)
{
	toPlace.position = {
		rect.position.x + rect.halfSize.x + toPlace.halfSize.x,
		rect.position.y
	};
}

static void PlaceRectInLeftSideOfContainer(const tl::Rect<float>& container, tl::Rect<float>& toPlace)
{
	toPlace.position = {
		container.position.x - container.halfSize.x + toPlace.halfSize.x,
		toPlace.halfSize.y
	};
}

void InitializeLayout(const EditorState& state)
{
	const float textAreaHalfHeight = 15.0f;
	const tl::Vec2<float> textCharFootprintHalfsize = {
		0.3f * textAreaHalfHeight,
		textAreaHalfHeight
	};

	float windowHalfWidth = (float)state.windowWidth * 0.5f;
	modeTextRect.halfSize = {
		windowHalfWidth / 3,
		textAreaHalfHeight
	};
	commandTextRect.halfSize = modeTextRect.halfSize;
	displayTextRect.halfSize = modeTextRect.halfSize;

	modeTextRect.position = modeTextRect.halfSize;
	PlaceRectToRightOfRect(modeTextRect, commandTextRect);
	PlaceRectToRightOfRect(commandTextRect, displayTextRect);

	modeTextCharFootprint.halfSize = textCharFootprintHalfsize;
	commandCharFootprint.halfSize = textCharFootprintHalfsize;
	displayCharFootprint.halfSize = textCharFootprintHalfsize;
	PlaceRectInLeftSideOfContainer(modeTextRect, modeTextCharFootprint);
	PlaceRectInLeftSideOfContainer(commandTextRect, commandCharFootprint);
	PlaceRectInLeftSideOfContainer(displayTextRect, displayCharFootprint);

	float paletteHalfWidthPercent = 0.2f;
	float visualYHalfSize = ((float)state.windowHeight * 0.5f) - commandTextRect.halfSize.y;
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

	PlaceRectToRightOfRect(spriteContainerRect, paletteContainerRect);
}

void Render(const tl::RenderBuffer& renderBuffer, const EditorState& state)
{
	// Render
	const uint32_t modeTextBackgroundColor = 0x000000;
	const uint32_t commandBackgroundColor = 0x111111;
	const uint32_t displayBackgroundColor = 0x222222;
	const uint32_t spriteBackgroundColor = 0x333333;
	const uint32_t paletteBackgroundColor = 0x444444;
	const uint32_t commandTextColor = 0xFFFFFF;
	const uint32_t displayTextColor = 0xFFFF00;

	tl::DrawRect(renderBuffer, modeTextBackgroundColor, modeTextRect);
	tl::DrawRect(renderBuffer, commandBackgroundColor, commandTextRect);
	tl::DrawRect(renderBuffer, displayBackgroundColor, displayTextRect);
	tl::DrawRect(renderBuffer, spriteBackgroundColor, spriteContainerRect);
	tl::DrawRect(renderBuffer, paletteBackgroundColor, paletteContainerRect);

	int displaySelectedPixelIndex = (state.activeControl == SpriteGrid) ? state.selectedPixelIndex : -1;
	RenderSpriteAsGrid(
		state.sprite,
		spriteBoundingRect,
		renderBuffer,
		displaySelectedPixelIndex,
		state.mouse
	);

	RenderSpriteAsGrid(
		*state.palette,
		paletteBoundingRect,
		renderBuffer,
		state.selectedPalettePixelIndex,
		state.mouse
	);

	char firstCommandChar = state.commandBuffer->access(0);
	char* commandString = &firstCommandChar;
	tl::DrawAlphabetCharacters(
		renderBuffer,
		commandString,
		commandCharFootprint,
		commandTextColor
	);

	char firstDisplayChar = state.displayBuffer->access(0);
	char* displayString = &firstDisplayChar;
	tl::DrawAlphabetCharacters(
		renderBuffer,
		displayString,
		displayCharFootprint,
		displayTextColor
	);
}
