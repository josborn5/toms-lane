#include "../../platform/lib/tl-library.hpp"

int rainbowColor = 0;
const tl::Vec2<float> smallFontHalfSize = { 5.0f, 10.0f };
const tl::Vec2<float> titleFontHalfSize = { 15.0f, 30.0f };

char* jumpScare = "\
       00000\n\
     000000000\n\
   0000  0  0000\n\
  000000   000000\n\
    00000000000\n\
     0 0 0 0 0\n\
     000000000\n\
";
tl::Sprite jumpScareSprite = tl::LoadSprite(jumpScare);


static void RenderGameState(
	const tl::RenderBuffer &renderBuffer,
	const GameState &state
) {
	if (state.mode == ReadyToStart)
	{
		tl::ClearScreen(renderBuffer, 0x050505);

		if (rainbowColor == (255 * 4))
		{
			rainbowColor = 0;
		}
		// Red inc, Green min, Blue dec
		// Red max, Green inc, Blue min
		// Red dec, Green max, Blue inc
		// Red min, Green dec, Blue max
		rainbowColor += 5;
		int rValue, gValue, bValue;
		if (rainbowColor < 255)
		{
			rValue = ClampInt(0, rainbowColor, 255);
			gValue = 0;
			bValue = ClampInt(0, -(rainbowColor - 255), 255);
		}
		else if (rainbowColor < (255 * 2))
		{
			rValue = 255;
			gValue = ClampInt(0, (255 * 2) - rainbowColor, 255);
			bValue = 0;
		}
		else if (rainbowColor < (255 * 3))
		{
			rValue = ClampInt(0, -(rainbowColor - (255 * 3)), 255);
			gValue = 255;
			bValue = ClampInt(0, (255 * 3) - rainbowColor, 255);
		}
		else
		{
			rValue = 0;
			gValue = ClampInt(0, -(rainbowColor - (255 * 4)), 255);
			bValue = 255;
		}
		uint32_t rainbowValue = (rValue << 16) | (gValue << 8) | (bValue << 0);
		tl::Rect<float> titleCharRect;
		titleCharRect.halfSize = titleFontHalfSize;
		titleCharRect.position = tl::Vec2<float> { 200.0f, 500.0f };
		tl::font_interface_render_chars(
			renderBuffer,
			"PLAY GAME SO I CAN GET BOBUX  ",
			titleCharRect,
			rainbowValue
		);

		tl::Rect<float> smallCharRect;
		smallCharRect.halfSize = smallFontHalfSize;
		smallCharRect.position = tl::Vec2<float> { 100.0f, 100.0f };
		tl::font_interface_render_chars(
			renderBuffer,
			"PRESS S TO START",
			smallCharRect,
			0xFFFF00
		);

		return;
	}

	if (state.mode == StartingNextLevel)
	{
		tl::ClearScreen(renderBuffer, 0x050505);
		return;
	}

	if (state.mode == GameOver)
	{
		tl::ClearScreen(renderBuffer, 0x050505);

		tl::Rect<float> jumpScareRect = {0};
		jumpScareRect.position = tl::Vec2<float> { 300.0f, 200.0f };
		jumpScareRect.halfSize = tl::Vec2<float> { 200.0f, 200.0f };
		tl::DrawSprite(
			renderBuffer,
			jumpScareSprite,
			jumpScareRect,
			0xFF0000
		);

		tl::Rect<float> titleCharRect = {0};
		titleCharRect.halfSize = tl::Vec2<float> { 50.0f, 75.0f };
		titleCharRect.position = tl::Vec2<float> { 200.0f, 500.0f };
		tl::font_interface_render_chars(
			renderBuffer,
			"YOU DIED",
			titleCharRect,
			0xFF0000
		);

		tl::DrawRect(renderBuffer, 0x00AA00, state.restartLevelButton);
		return;
	}

	// background
	tl::ClearScreen(renderBuffer, 0x222222);

	tl::Matrix2x3<float> worldToCameraProjection;
	tl::Rect<float> renderCamera;
	renderCamera.halfSize = { (float)(renderBuffer.width / 2), (float)(renderBuffer.height / 2) };
	renderCamera.position = renderCamera.halfSize;
	tl::transform_interface_create_2d_projection_matrix(
		state.camera,
		renderCamera,
		worldToCameraProjection
	);

	for (int i = 0; i < state.blockCount; i += 1)
	{
		Block block = state.blocks[i];

		tl::Rect<float> blockInCameraSpace;
		tl::transform_interface_project_rect(
			worldToCameraProjection,
			block,
			blockInCameraSpace
		);

		if (block.bitmap == nullptr)
		{
			tl::DrawRect(renderBuffer, block.color, blockInCameraSpace);
		}
		else
		{
			tl::bitmap_interface_render(
				renderBuffer,
				*block.bitmap,
				blockInCameraSpace
			);
		}
	}

	tl::Rect<float> playerInCameraSpace;
	tl::transform_interface_project_rect(
		worldToCameraProjection,
		state.player,
		playerInCameraSpace
	);
	tl::bitmap_interface_render(
		renderBuffer,
		state.player.bitmap,
		playerInCameraSpace
	);
}
