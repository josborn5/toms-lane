#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"

static tl::Rect<float> commandTextRect;
static tl::Rect<float> commandCharFootprint;

static const float textAreaHalfHeight = 15.0f;
static const tl::Vec2<float> textCharFootprintHalfsize = {
	4.0f * textAreaHalfHeight / 7.0f,
	textAreaHalfHeight
};

static float cursorTime = 0.0f;

static void GetDisplayStringForGrid(const Grid& grid, char* writeTo)
{
	tl::Color selectedColor = grid.sprite->content[grid.selectedIndex];

	int color = (int)(selectedColor.r * 255.0f);
	char* cursor = writeTo;
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

static tl::Rect<float> SizeBoundingRectForSpriteInContainingRect(const tl::SpriteC& sprite, const tl::Rect<float>& containerRect)
{
	tl::Rect<float> sizeRect;
	float spriteAspectRatio = (float)sprite.height / (float)sprite.width;
	float backgroundAspectRatio = (containerRect.halfSize.y - textCharFootprintHalfsize.y) / containerRect.halfSize.x;
	float relativeAspectRatio = spriteAspectRatio / backgroundAspectRatio;
	if (relativeAspectRatio >= 1.0f)
	{
		sizeRect.halfSize.y = containerRect.halfSize.y - textCharFootprintHalfsize.y;
		sizeRect.halfSize.x = sizeRect.halfSize.y / spriteAspectRatio;
	}
	else
	{
		sizeRect.halfSize.x = containerRect.halfSize.x;
		sizeRect.halfSize.y = (sizeRect.halfSize.x * spriteAspectRatio) - textCharFootprintHalfsize.y;
	}

	sizeRect.position = {
		containerRect.position.x,
		containerRect.position.y + textCharFootprintHalfsize.y
	};

	return sizeRect;
}

static void GetSelectedRangeFootprint(
	const Grid& grid,
	const tl::Rect<float>& selectedPixelFootprint,
	tl::Rect<float>& rangeFootprint,
	Mode mode)
{
	if (mode != Visual)
	{
		rangeFootprint.position = selectedPixelFootprint.position;
		rangeFootprint.halfSize = {
			selectedPixelFootprint.halfSize.x + 1,
			selectedPixelFootprint.halfSize.y + 1
		};
		return;
	}

	int selectedRow = GetRowIndex(grid, grid.selectedIndex);
	int selectedColumn = GetColumnIndex(grid, grid.selectedIndex);
	int rangeRow = GetRowIndex(grid, grid.selectedRangeIndex);
	int rangeColumn = GetColumnIndex(grid, grid.selectedRangeIndex);

	int height = rangeRow - selectedRow + 1;
	int width = rangeColumn - selectedColumn + 1;

	rangeFootprint.halfSize = {
		(width * selectedPixelFootprint.halfSize.x) + 1,
		(height * selectedPixelFootprint.halfSize.y) + 1
	};
	rangeFootprint.position = {
		selectedPixelFootprint.position.x + ((float)(width - 1) * selectedPixelFootprint.halfSize.x),
		selectedPixelFootprint.position.y - ((float)(height - 1) * selectedPixelFootprint.halfSize.y)
	};
}

static void RenderSpriteAsGrid(
	const Grid& grid,
	const tl::RenderBuffer& renderBuffer,
	int selectedBlockIndex,
	Mode mode
) {
	tl::SpriteC& sprite = *grid.sprite;
	const tl::Rect<float>& boundingRect = grid.footprint;

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
			if (pixelIndex == selectedBlockIndex)
			{
				tl::Rect<float> selectedFootprint;
				GetSelectedRangeFootprint(grid, pixelFootPrint, selectedFootprint, mode);
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

	tl::Rect<float> charFootprint;
	charFootprint.halfSize = textCharFootprintHalfsize;
	charFootprint.position = {
		grid.container.position.x - grid.container.halfSize.x + textCharFootprintHalfsize.x,
		grid.container.position.y - grid.container.halfSize.y + textCharFootprintHalfsize.y
	};

	const uint32_t displayTextColor = 0xFFFF00;
	const int displayBufferSize = 16;
	char displayBuffer[displayBufferSize];
	GetDisplayStringForGrid(grid, displayBuffer);

	tl::DrawAlphabetCharacters(
		renderBuffer,
		displayBuffer,
		charFootprint,
		displayTextColor
	);
}

void SizeGrid(Grid& grid)
{
	grid.footprint = SizeBoundingRectForSpriteInContainingRect(*grid.sprite, grid.container);
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

void InitializeLayout(EditorState& state)
{
	tl::Rect<float> windowRect;
	windowRect.halfSize = {
		(float)state.windowWidth * 0.5f,
		(float)state.windowHeight * 0.5f,
	};
	windowRect.position = windowRect.halfSize;
	float windowHalfWidth = windowRect.halfSize.x;
	commandTextRect.halfSize = {
		windowHalfWidth,
		textAreaHalfHeight
	};
	commandCharFootprint.halfSize = textCharFootprintHalfsize;

	PlaceRectInLeftSideOfContainer(windowRect, commandTextRect);
	PlaceRectInLeftSideOfContainer(commandTextRect, commandCharFootprint);

	float paletteHalfWidthPercent = 0.2f;
	float visualYHalfSize = ((float)state.windowHeight * 0.5f) - commandTextRect.halfSize.y;
	state.pixels.container.halfSize = {
		windowHalfWidth * (1.0f - paletteHalfWidthPercent),
		visualYHalfSize
	};

	float visualYPosition = state.pixels.container.halfSize.y + commandTextRect.position.y + commandTextRect.halfSize.y;
	state.pixels.container.position = {
		state.pixels.container.halfSize.x,
		visualYPosition
	};

	state.palette_.container.halfSize = {
		windowHalfWidth * paletteHalfWidthPercent,
		visualYHalfSize
	};

	PlaceRectToRightOfRect(state.pixels.container, state.palette_.container);
}

static void RenderCommandBuffer(const tl::RenderBuffer& renderBuffer, const EditorState& state, float dt)
{
	const uint32_t commandBackgroundColor = 0x111111;
	const uint32_t commandTextColor = 0xFFFFFF;
	tl::DrawRect(renderBuffer, commandBackgroundColor, commandTextRect);
	float dx = tl::DrawAlphabetCharacters(
		renderBuffer,
		state.commandBuffer,
		commandCharFootprint,
		commandTextColor
	);

	cursorTime += dt;

	if (state.mode == Command || state.mode == NoFile)
	{
		tl::Rect<float> cursorFootprint = tl::CopyRect(commandCharFootprint);
		cursorFootprint.position.x += dx;
		if (cursorTime <= 0.75f)
		{
			tl::DrawRect(renderBuffer, commandTextColor, cursorFootprint);
		}
	}

	if (cursorTime > 1.2f)
	{
		cursorTime = 0.0f;
	}
}

void Render(const tl::RenderBuffer& renderBuffer, const EditorState& state, float dt)
{
	const uint32_t spriteBackgroundColor = 0x333333;
	const uint32_t paletteBackgroundColor = 0x444444;
	tl::ClearScreen(renderBuffer, 0x000000);
	tl::DrawRect(renderBuffer, spriteBackgroundColor, state.pixels.container);
	tl::DrawRect(renderBuffer, paletteBackgroundColor, state.palette_.container);

	if (state.mode != NoFile)
	{
		int displaySelectedPixelIndex = (state.activeControl == SpriteGrid && state.mode != Command)
			? state.pixels.selectedIndex
			: -1;
		RenderSpriteAsGrid(
			state.pixels,
			renderBuffer,
			displaySelectedPixelIndex,
			state.mode
		);
	}

	RenderCommandBuffer(renderBuffer, state, dt);

	RenderSpriteAsGrid(
		state.palette_,
		renderBuffer,
		state.palette_.selectedIndex,
		state.mode
	);
}
