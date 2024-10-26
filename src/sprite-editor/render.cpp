#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"

static tl::Rect<float> commandTextRect;
static tl::Rect<float> commandCharFootprint;

static const float textAreaHalfHeight = 10.0f;
static const float color_table_halfsize_y = 10.0f;
static const tl::Vec2<float> textCharFootprintHalfsize = {
	4.0f * textAreaHalfHeight / 7.0f,
	textAreaHalfHeight
};

static float cursorTime = 0.0f;

static void GetDisplayStringForGrid(const Grid& grid, char* writeTo)
{
	uint32_t selected_pixel_color = grid.selected_color();

	int r_color = (selected_pixel_color >> 16) & 0x0000FF;
	int g_color = (selected_pixel_color >> 8) & 0x0000FF;
	int b_color = selected_pixel_color & 0x0000FF;

	char* cursor = writeTo;
	tl::IntToCharString(r_color, cursor);

	while (*cursor) cursor++;
	*cursor = ' ';
	cursor++;

	tl::IntToCharString(g_color, cursor);

	while (*cursor) cursor++;
	*cursor = ' ';
	cursor++;

	tl::IntToCharString(b_color, cursor);
}

static tl::Rect<float> SizeBoundingRectForSpriteInContainingRect(const SpriteC& sprite, const tl::Rect<float>& containerRect)
{
	tl::Rect<float> sizeRect;
	float spriteAspectRatio = (float)sprite.height / (float)sprite.width;

	tl::Rect<float> background_rect;
	background_rect.halfSize = {
		containerRect.halfSize.x,
		containerRect.halfSize.y - textCharFootprintHalfsize.y
	};
	if (sprite.color_table.length() > 0)
	{
		background_rect.halfSize.y -= color_table_halfsize_y;
	}
	background_rect.position = {
		containerRect.position.x,
		containerRect.position.y + containerRect.halfSize.y - background_rect.halfSize.y
	};

	float backgroundAspectRatio = background_rect.halfSize.y / background_rect.halfSize.x;
	float relativeAspectRatio = spriteAspectRatio / backgroundAspectRatio;
	if (relativeAspectRatio >= 1.0f)
	{
		sizeRect.halfSize.y = background_rect.halfSize.y;
		sizeRect.halfSize.x = sizeRect.halfSize.y / spriteAspectRatio;
	}
	else
	{
		sizeRect.halfSize.x = background_rect.halfSize.x;
		sizeRect.halfSize.y = sizeRect.halfSize.x * spriteAspectRatio;
	}

	sizeRect.position = background_rect.position;
	return sizeRect;
}

static void GetSelectedRangeFootprint(
	const tl::Rect<float>& selectedPixelFootprint,
	tl::Rect<float>& rangeFootprint)
{
	rangeFootprint.position = selectedPixelFootprint.position;
	rangeFootprint.halfSize = {
		selectedPixelFootprint.halfSize.x + 1,
		selectedPixelFootprint.halfSize.y + 1
	};
}

static void pixel_footprint_get(
	float containing_length,
	int pixel_count,
	float& pixel_with_border_length,
	tl::Vec2<float>& pixel_footprint
)
{
	const float pixelBorderWidth = 2.0f;
	pixel_with_border_length = containing_length / pixel_count;
	float pixel_dimension = pixel_with_border_length - (2.0f * pixelBorderWidth);
	pixel_footprint = { 0.5F * pixel_dimension, 0.5f * pixel_dimension };
}

static void render_color_table(
	const Grid& grid,
	const tl::RenderBuffer& renderBuffer
)
{
	int color_table_length = grid.sprite->color_table.length();
	if (color_table_length == 0)
	{
		return;
	}

	tl::Rect<float> color_table_footprint;
	color_table_footprint.halfSize = {
		grid.footprint.halfSize.x,
		color_table_halfsize_y
	};
	color_table_footprint.position = {
		grid.footprint.position.x,
		grid.footprint.y_min() - color_table_halfsize_y
	};

	float pixel_border_length;
	tl::Vec2<float> pixel_half_size;
	pixel_footprint_get(
		(2.0f * color_table_footprint.halfSize.y),
		1,
		pixel_border_length,
		pixel_half_size
	);

	tl::Rect<float> pixel_footprint;
	pixel_footprint.halfSize = pixel_half_size;
	pixel_footprint.position = {
		color_table_footprint.x_min() + 0.5f * pixel_border_length,
		color_table_footprint.y_min() + 0.5f * pixel_border_length
	};

	for (int i = 0; i < color_table_length; i += 1)
	{
		uint32_t color = grid.sprite->color_table.get_copy(i).value;
		tl::DrawRect(renderBuffer, color, pixel_footprint);
		pixel_footprint.position.x += pixel_border_length;
	}
}

static void RenderSpriteAsGrid(
	const Grid& grid,
	const tl::RenderBuffer& renderBuffer,
	int selectedBlockIndex,
	Mode mode
) {
	SpriteC& sprite = *grid.sprite;
	const tl::Rect<float>& boundingRect = grid.footprint;

	const uint32_t selectedPixelColor = 0xFFFF00;

	float pixelDimensionWithBorder;
	tl::Vec2<float> pixelHalfSize;
	pixel_footprint_get(
		(2.0f * boundingRect.halfSize.x),
		sprite.width,
		pixelDimensionWithBorder,
		pixelHalfSize
	);

	float yOriginalPosition = boundingRect.y_min() + (0.5f * pixelDimensionWithBorder);

	int selectedColIndex = grid.selected_column_index();
	int rangeColIndex = grid.column_index(grid.selectedRangeIndex);
	int selectedRowIndex = grid.selected_row_index();
	int rangeRowIndex = grid.row_index(grid.selectedRangeIndex);

	int startColIndex = (mode != Visual || selectedColIndex < rangeColIndex)
		? selectedColIndex
		: rangeColIndex;
	int endColIndex = (mode != Visual || rangeColIndex < selectedColIndex)
		? selectedColIndex
		: rangeColIndex;
	int startRowIndex = (mode != Visual || selectedRowIndex < rangeRowIndex)
		? selectedRowIndex
		: rangeRowIndex;
	int endRowIndex = (mode != Visual || rangeRowIndex < selectedRowIndex)
		? selectedRowIndex
		: rangeRowIndex;

	tl::Matrix2x3<float> gridToRenderProjection;
	tl::transform_interface_create_2d_projection_matrix(
		grid.camera,
		grid.footprint,
		gridToRenderProjection
	);

	for (int j = 0; j < sprite.height; j += 1)
	{
		float yPosition = yOriginalPosition + (j * pixelDimensionWithBorder);
		bool yIsInSelectedRange = (j >= startRowIndex && j <= endRowIndex);
		for (int i = 0; i < sprite.width; i += 1)
		{
			float xPosition = boundingRect.x_min() + (0.5f * pixelDimensionWithBorder) + (i * pixelDimensionWithBorder);

			tl::Vec2<float> pixelPosition = { xPosition, yPosition };
			tl::Rect<float> pixelFootPrint;
			tl::Rect<float> pixelRenderFootprint;
			pixelFootPrint.halfSize = pixelHalfSize;
			pixelFootPrint.position = pixelPosition;
			tl::transform_interface_project_rect(
				gridToRenderProjection,
				pixelFootPrint,
				pixelRenderFootprint
			);

			if (yIsInSelectedRange && i >= startColIndex && i <= endColIndex)
			{
				tl::Rect<float> selectedFootprint;
				GetSelectedRangeFootprint(pixelRenderFootprint, selectedFootprint);
				tl::DrawRect(renderBuffer, selectedPixelColor, selectedFootprint);
			}

			int pixelIndex = (j * sprite.width) + i;
			uint32_t pixelData = sprite.pixels()[pixelIndex];
			if (sprite.color_table.length() > 0)
			{
				pixelData = sprite.color_table.get_copy(pixelData).value;
			}
			tl::DrawRect(renderBuffer, pixelData, pixelRenderFootprint);
		}
	}

	tl::Rect<float> charFootprint;
	charFootprint.halfSize = textCharFootprintHalfsize;
	charFootprint.position = {
		grid.container.x_min() + textCharFootprintHalfsize.x,
		grid.container.y_min() + textCharFootprintHalfsize.y
	};

	render_color_table(grid, renderBuffer);

	const uint32_t displayTextColor = 0xFFFF00;
	const int displayBufferSize = 16;
	char displayBuffer[displayBufferSize];
	GetDisplayStringForGrid(grid, displayBuffer);

	tl::font_interface_render_chars(
		renderBuffer,
		displayBuffer,
		charFootprint,
		displayTextColor
	);
}

void SizeGrid(Grid& grid)
{
	grid.footprint = SizeBoundingRectForSpriteInContainingRect(*grid.sprite, grid.container);
	grid.camera = grid.footprint;
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
		container.x_min() + toPlace.halfSize.x,
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
	float dx = tl::font_interface_render_chars(
		renderBuffer,
		state.commandBuffer,
		commandCharFootprint,
		commandTextColor
	);

	cursorTime += dt;

	if (state.mode == Command)
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

	int displaySelectedPixelIndex = (state.pixels_are_selected() && state.mode != Command)
		? state.pixels.selectedIndex
		: -1;
	RenderSpriteAsGrid(
		state.pixels,
		renderBuffer,
		displaySelectedPixelIndex,
		state.mode
	);

	RenderCommandBuffer(renderBuffer, state, dt);

	RenderSpriteAsGrid(
		state.palette_,
		renderBuffer,
		state.palette_.selectedIndex,
		state.mode
	);
}
