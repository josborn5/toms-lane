const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

const float SMALL_FONT_SIZE = 20.0f;
const float TITLE_FONT_SIZE = 120.0f;

int rainbowColor = 0;


static void RenderGameState(const tl::RenderBuffer& renderBuffer, const GameState& state)
{
	if (state.mode != Started)
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
		titleCharRect.position = tl::Vec2<float> { 250.0f, 400.0f};
		titleCharRect.halfSize = tl::Vec2<float> { 0.5f * TITLE_FONT_SIZE, TITLE_FONT_SIZE };
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"BREAKOUT",
			titleCharRect,
			rainbowValue
		);

		tl::Rect<float> smallCharRect;
		smallCharRect.position = tl::Vec2<float> { 100.0f, 100.0f};
		smallCharRect.halfSize = tl::Vec2<float> { 0.5f * SMALL_FONT_SIZE, SMALL_FONT_SIZE };
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"PRESS S TO START",
			smallCharRect,
			TEXT_COLOR
		);

		return;
	}

	// background
	tl::ClearScreen(renderBuffer, 0x000000);
	tl::DrawRect(renderBuffer, BACKGROUND_COLOR, state.world);

	// player
	tl::DrawRect(renderBuffer, BAT_COLOR, state.player);

	// blocks & powerups
	for (int i = 0; i < BLOCK_ARRAY_SIZE; i += 1)
	{
		Block block = state.blocks[i];
		if (block.exists) {
			tl::DrawRect(renderBuffer, block.color, block);
		}

		if (block.powerUp.exists) {
			tl::DrawRect(renderBuffer, block.powerUp.color, block.powerUp);
		}
	}

	// ball
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		if (!state.balls[i].exists) continue;

		tl::DrawRect(renderBuffer, 0x000000, state.checkArea[i]);
		tl::DrawRect(renderBuffer, BALL_COLOR, state.balls[i]);
	}

	// Balls, Level & Score
	tl::Vec2<float> inGameFontHalfSize = tl::Vec2<float> { 0.5f * SMALL_FONT_SIZE, SMALL_FONT_SIZE };
	tl::Rect<float> inGameCursor;
	inGameCursor.halfSize = inGameFontHalfSize;
	inGameCursor.position = tl::Vec2<float> { 100.0f, 100.0f };

	DrawAlphabetCharacters(renderBuffer, "BALLS", inGameCursor, TEXT_COLOR);
	inGameCursor.position = tl::Vec2<float> { 250.0f, 100.0f };
	DrawNumber(renderBuffer, state.lives, inGameCursor, TEXT_COLOR);

	inGameCursor.position = tl::Vec2<float> { 450.0f, 100.0f };
	DrawAlphabetCharacters(renderBuffer, "LEVEL", inGameCursor, TEXT_COLOR);
	inGameCursor.position = tl::Vec2<float> { 600.0f, 100.0f };
	DrawNumber(renderBuffer, state.level, inGameCursor, TEXT_COLOR);

	inGameCursor.position = tl::Vec2<float> { 900.0f, 100.0f };
	DrawAlphabetCharacters(renderBuffer, "SCORE", inGameCursor, TEXT_COLOR);
	inGameCursor.position = tl::Vec2<float> { 1050.0f, 100.0f };
	DrawNumber(renderBuffer, state.score, inGameCursor, TEXT_COLOR);
}

