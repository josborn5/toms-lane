const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

const float SMALL_FONT_SIZE = 20.0f;
const float TITLE_FONT_SIZE = 120.0f;

static void color_to_rgb(uint32_t colorRGBA, uint8_t& r, uint8_t& g, uint8_t& b)
{
	r = (uint8_t)(colorRGBA >> 16);
	g = (uint8_t)(colorRGBA >> 8);
	b = (uint8_t)(colorRGBA >> 0);
}

static uint32_t rgb_to_color(uint8_t& r, uint8_t& g, uint8_t& b)
{
	return (r << 16) | (g << 8) | (b << 0);
}

static uint32_t rainbowColor = 0xFF0000; // start as red
static uint8_t rainbowPhase = 0;
static void updateRainbowColor()
{
	uint8_t newR;
	uint8_t newG;
	uint8_t newB;

	uint8_t oldR;
	uint8_t oldG;
	uint8_t oldB;
	color_to_rgb(rainbowColor, oldR, oldG, oldB);
	const uint8_t colorIncrement = 3;
	switch (rainbowPhase)
	{
		case 0:
			newR = oldR - colorIncrement;
			newG = oldG + colorIncrement;
			newB = 0;

			if (newR == 0)
			{
				rainbowPhase += 1;
			}
			break;
		case 1:
			newR = 0;
			newG = oldG - colorIncrement;
			newB = oldB + colorIncrement;

			if (newG == 0)
			{
				rainbowPhase += 1;
			}
			break;
		case 2:
			newR = oldR + colorIncrement;
			newG = 0;
			newB = oldB - colorIncrement;

			if (newB == 0)
			{
				rainbowPhase = 0;
			}
			break;
	}
	rainbowColor = rgb_to_color(newR, newG, newB);
}

static void RenderGameState(const tl::RenderBuffer& renderBuffer, const GameState& state)
{
	if (state.mode != Started)
	{
		tl::ClearScreen(renderBuffer, 0x050505);

		updateRainbowColor();
		tl::Rect<float> titleCharRect;
		titleCharRect.position = tl::Vec2<float> { 250.0f, 400.0f};
		titleCharRect.halfSize = tl::Vec2<float> { 0.5f * TITLE_FONT_SIZE, TITLE_FONT_SIZE };
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"BREAKOUT",
			titleCharRect,
			rainbowColor
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
	for (int i = 0; i < state.blocks_.length(); i += 1)
	{
		Block block = state.blocks_.get(i);
		if (block.exists) {
			tl::DrawRect(renderBuffer, block.color, block);
		}

		if (block.powerUp.exists) {
			tl::DrawRect(renderBuffer, block.powerUp.color, block.powerUp);
		}
	}

	// ball
	for (int i = 0; i < state.balls.length(); i += 1)
	{
		Ball& ball = state.balls.get(i);
		if (!ball.exists) continue;

		tl::DrawRect(renderBuffer, BALL_COLOR, ball);
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

