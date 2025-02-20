#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./utilities.hpp"

struct sprite_control_view
{
	tl::Rect<float> container;
	tl::Rect<float> footprint;
	Grid* sprite_control;
};

struct window_view
{
	sprite_control_view canvas;
	sprite_control_view color_table;
	sprite_control_view palette;
};

window_view main_view;

static tl::Rect<float> commandTextRect;
static tl::Rect<float> commandCharFootprint;

static const float textAreaHalfHeight = 10.0f;
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

	background_rect.position = {
		containerRect.position.x,
		containerRect.y_max() - background_rect.halfSize.y
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

static void pixel_footprint_get(
	const sprite_control_view& view,
	tl::Vec2<float>& pixel_footprint
)
{
	float pixel_dimension = (2.0f * view.footprint.halfSize.x) / (float)view.sprite_control->sprite->width;
	pixel_footprint = { 0.5F * pixel_dimension, 0.5f * pixel_dimension };
}

static void set_camera_rect(const sprite_control_view& view, tl::Rect<float>& camera_rect)
{
	tl::Vec2<float> pixel_half_size;
	pixel_footprint_get(
		view,
		pixel_half_size
	);
	Grid& grid = *view.sprite_control;
	bool even_horizontal = (view.sprite_control->sprite->width % 2) == 0;
	float horizontal_offset = even_horizontal ? 0.0f : 0.5f;

	bool even_vertical = (view.sprite_control->sprite->height % 2) == 0;
	float vertical_offset = even_vertical ? 0.0f : 0.5f;

	camera_rect.position.x = view.footprint.position.x - view.footprint.halfSize.x
		+ ((float)grid.camera_focus.column_index() + horizontal_offset) * (pixel_half_size.x * 2.0f);
	camera_rect.position.y = view.footprint.position.y - view.footprint.halfSize.y
		+ ((float)grid.camera_focus.row_index() + vertical_offset) * (pixel_half_size.y * 2.0f);

	camera_rect.halfSize.x = view.footprint.halfSize.x * grid.camera_focus.zoom;
	camera_rect.halfSize.y = view.footprint.halfSize.y * grid.camera_focus.zoom;
}

static void RenderSpriteAsGrid(
	const sprite_control_view& view,
	const tl::RenderBuffer& renderBuffer,
	Mode mode
) {
	Grid& grid = *view.sprite_control;
	SpriteC& sprite = *grid.sprite;

	const uint32_t selectedPixelColor = 0xFFFF00;

	tl::Vec2<float> pixelHalfSize;
	pixel_footprint_get(
		view,
		pixelHalfSize
	);

	tl::Rect<float> camera_rect;
	set_camera_rect(view, camera_rect);

	tl::Matrix2x3<float> gridToRenderProjection;
	tl::transform_interface_create_2d_projection_matrix(
		camera_rect,
		view.footprint,
		gridToRenderProjection
	);

	// Check if we need to iterate through the pixels on the screen (i.e. downsample the sprite)
	// or the pixles in the bitmap (i.e. upsample the sprite)
	tl::Rect<float> testPixel;
	testPixel.halfSize = pixelHalfSize;
	testPixel.position = pixelHalfSize;
	tl::Rect<float> pixelRenderFootprint;
	tl::transform_interface_project_rect(
		gridToRenderProjection,
		testPixel,
		pixelRenderFootprint
	);

	bool need_to_downsample = pixelRenderFootprint.halfSize.x < 1.0f;
	if (need_to_downsample)
	{
		tl::Rect<float> projected_footprint;
		tl::transform_interface_project_rect(
			gridToRenderProjection,
			view.footprint,
			projected_footprint
		);

		float sprite_pixels_per_screen_pixel = (float)sprite.width / projected_footprint.halfSize.x * 2.0f;

		for (unsigned int j = 0; j < (unsigned int)(2.0f * projected_footprint.halfSize.y); j += 1)
		{
			int sprite_j = (int)(sprite_pixels_per_screen_pixel * (float)j);
			int screen_j = (int)(projected_footprint.position.y - projected_footprint.halfSize.y) + j;
			for (unsigned int i = 0; i < (unsigned int)(2.0f * projected_footprint.halfSize.x); i += 1)
			{
				int sprite_i = (int)(sprite_pixels_per_screen_pixel * (float(i)));

				int pixel_index = sprite_i + (sprite_j * sprite.width);
				uint32_t pixel_data = sprite.get_pixel_data(pixel_index);
				if (sprite.has_color_table())
				{
					pixel_data = sprite.p_color_table->get_pixel_data(pixel_data);
				}

				int screen_i = (int)(projected_footprint.position.x - projected_footprint.halfSize.x) + i;

				tl::PlotPixel(renderBuffer, pixel_data, screen_i, screen_j);
			}
		}
		return;
	}

	float yOriginalPosition = view.footprint.y_min() + pixelHalfSize.y;
	float xOriginalPosition = view.footprint.x_min() + pixelHalfSize.x;

	unsigned int selectedColIndex = grid.cursor.column_index();
	unsigned int rangeColIndex = grid.range.column_index();
	unsigned int selectedRowIndex = grid.cursor.row_index();
	unsigned int rangeRowIndex = grid.range.row_index();

	unsigned int startColIndex = (mode != Visual || selectedColIndex < rangeColIndex)
		? selectedColIndex
		: rangeColIndex;
	unsigned int endColIndex = (mode != Visual || rangeColIndex < selectedColIndex)
		? selectedColIndex
		: rangeColIndex;
	unsigned int startRowIndex = (mode != Visual || selectedRowIndex < rangeRowIndex)
		? selectedRowIndex
		: rangeRowIndex;
	unsigned int endRowIndex = (mode != Visual || rangeRowIndex < selectedRowIndex)
		? selectedRowIndex
		: rangeRowIndex;

	for (unsigned int j = 0; j < sprite.height; j += 1)
	{
		float yPosition = yOriginalPosition + (j * pixelHalfSize.y * 2.0f);
		bool yIsInSelectedRange = (j >= startRowIndex && j <= endRowIndex);
		for (unsigned int i = 0; i < sprite.width; i += 1)
		{
			float xPosition = xOriginalPosition + (i * pixelHalfSize.x * 2.0f);

			tl::Vec2<float> pixelPosition = { xPosition, yPosition };
			tl::Rect<float> pixelFootPrint;
			pixelFootPrint.halfSize = pixelHalfSize;
			pixelFootPrint.position = pixelPosition;
			tl::transform_interface_project_rect(
				gridToRenderProjection,
				pixelFootPrint,
				pixelRenderFootprint
			);

			if (yIsInSelectedRange && i >= startColIndex && i <= endColIndex)
			{
				tl::DrawRect(renderBuffer, selectedPixelColor, pixelRenderFootprint);
			}

			int pixelIndex = (j * sprite.width) + i;
			uint32_t pixelData = sprite.get_pixel_data(pixelIndex);
			if (sprite.has_color_table())
			{
				pixelData = sprite.p_color_table->get_pixel_data(pixelData);
			}
			pixelRenderFootprint.halfSize.x -= 1;
			pixelRenderFootprint.halfSize.y -= 1;
			tl::DrawRect(renderBuffer, pixelData, pixelRenderFootprint);
		}
	}

	tl::Rect<float> charFootprint;
	charFootprint.halfSize = textCharFootprintHalfsize;
	charFootprint.position = {
		view.container.x_min() + textCharFootprintHalfsize.x,
		view.container.y_min() + textCharFootprintHalfsize.y
	};

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

void size_control_view(sprite_control_view& view)
{
	view.footprint = SizeBoundingRectForSpriteInContainingRect(*(view.sprite_control->sprite), view.container);
}

void size_canvas()
{
	size_control_view(main_view.canvas);
}

void size_color_table()
{
	size_control_view(main_view.color_table);
}

void size_palette()
{
	size_control_view(main_view.palette);
}

static void PlaceRectToRightOfRect(const tl::Rect<float>& rect, tl::Rect<float>& toPlace)
{
	toPlace.position = {
		rect.x_max() + toPlace.halfSize.x,
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
	float color_table_width_percent = state.pixels.sprite->has_color_table() ? 0.1f : 0.0f;;
	float visualYHalfSize = ((float)state.windowHeight * 0.5f) - commandTextRect.halfSize.y;

	main_view.canvas.container.halfSize = {
		windowHalfWidth * (1.0f - paletteHalfWidthPercent - color_table_width_percent),
		visualYHalfSize
	};
	float visualYPosition =  commandTextRect.y_max() + main_view.canvas.container.halfSize.y;
	main_view.canvas.container.position = {
		main_view.canvas.container.halfSize.x,
		visualYPosition
	};
	main_view.palette.container.halfSize = {
		windowHalfWidth * paletteHalfWidthPercent,
		visualYHalfSize
	};
	main_view.color_table.container.halfSize = {
		windowHalfWidth * color_table_width_percent,
		visualYHalfSize
	};
	PlaceRectToRightOfRect(main_view.canvas.container, main_view.color_table.container);
	PlaceRectToRightOfRect(main_view.color_table.container, main_view.palette.container);

	main_view.canvas.sprite_control = &state.pixels;
	state.pixels.size_change_callback = &size_canvas;
	main_view.color_table.sprite_control = &state.color_table;
	state.color_table.size_change_callback = &size_color_table;
	main_view.palette.sprite_control = &state.palette_;
	state.palette_.size_change_callback = &size_palette;
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
	tl::DrawRect(renderBuffer, spriteBackgroundColor, main_view.canvas.container);
	tl::DrawRect(renderBuffer, paletteBackgroundColor, main_view.palette.container);

	RenderSpriteAsGrid(
		main_view.canvas,
		renderBuffer,
		state.mode
	);

	RenderCommandBuffer(renderBuffer, state, dt);

	RenderSpriteAsGrid(
		main_view.palette,
		renderBuffer,
		state.mode
	);

	if (state.canvas.has_color_table())
	{
		RenderSpriteAsGrid(
			main_view.color_table,
			renderBuffer,
			state.mode
		);
	}
}
