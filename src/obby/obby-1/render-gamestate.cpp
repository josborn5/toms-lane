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


void CreateWorldToCameraProjectionMatrix(
	const tl::Rect<float>& world,
	const tl::Rect<float>& camera,
	tl::Matrix2x3<float>& matrix
)
{
	float dX = camera.position.x - world.position.x;
	float dY = camera.position.y - world.position.y;
	float scaleFactor = 1.0f;

	matrix.m[0][0] = scaleFactor;
	matrix.m[1][0] = 0.0f;
	matrix.m[2][0] = dX;

	matrix.m[0][1] = 0.0f;
	matrix.m[1][1] = scaleFactor;
	matrix.m[2][1] = dY;
}

void TransformFromWorldToCamera(
	const tl::Matrix2x3<float>& projection,
	const tl::Rect<float>& worldSpace,
	tl::Rect<float>& cameraSpace
)
{
	cameraSpace.position = tl::Transform2DVector(worldSpace.position, projection);
	cameraSpace.halfSize = tl::Transform2DVector(worldSpace.halfSize, projection);
}

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
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"PLAY GAME SO I CAN GET BOBUX  ",
			titleCharRect,
			rainbowValue
		);

		tl::Rect<float> smallCharRect;
		smallCharRect.halfSize = smallFontHalfSize;
		smallCharRect.position = tl::Vec2<float> { 100.0f, 100.0f };
		tl::DrawAlphabetCharacters(
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
		tl::DrawAlphabetCharacters(
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
	CreateWorldToCameraProjectionMatrix(
		state.world,
		state.camera,
		worldToCameraProjection
	);

	for (int i = 0; i < state.blockCount; i += 1)
	{
		Block block = state.blocks[i];
		if (!block.exists) continue;

		tl::Rect<float> blockInCameraSpace;
		TransformFromWorldToCamera(
			worldToCameraProjection,
			block,
			blockInCameraSpace
		);

		if (block.sprite == nullptr)
		{
			tl::DrawRect(renderBuffer, block.color, blockInCameraSpace);
		}
		else
		{
			tl::SpriteC sprite = *block.sprite;
			tl::DrawSpriteC(
				renderBuffer,
				sprite,
				blockInCameraSpace
			);
		}
	}

	tl::DrawSpriteC(
		renderBuffer,
		state.player.spriteTest,
		state.player
	);
}
