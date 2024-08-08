const uint32_t BACKGROUND_COLOR = 0x551100;

const uint32_t playerColor = 0x00FF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

int rainbowColor = 0;
const tl::Vec2<float> smallFontHalfSize = { 5.0f, 10.0f };
const tl::Vec2<float> titleFontHalfSize = { 15.0f, 30.0f };

char* islaAvatar = "\
   0\n\
  000\n\
  000\n\
   0\n\
0000000\n\
   0\n\
   0\n\
  0 0\n\
  0 0\n\
 0   0\n\
0     0";
char* islaAvatarMoveright1 = "\
      0\n\
     000\n\
     000\n\
      0\n\
000000\n\
    0\n\
   0\n\
\n\
\n\
\n\
";
char* islaAvatarMoveright2 = "\
      0\n\
     000\n\
     000\n\
      0\n\
000000\n\
      0\n\
       0\n\
\n\
\n\
\n\
";
char* islaAvatarMoveleft1 = "\
 0\n\
000\n\
000\n\
 0\n\
  000000\n\
 0\n\
0\n\
\n\
\n\
\n\
";
char* islaAvatarMoveleft2 = "\
 0\n\
000\n\
000\n\
 0\n\
  000000\n\
   0\n\
    0\n\
\n\
\n\
\n\
";

tl::Sprite islaSprite = tl::LoadSprite(islaAvatar);
tl::Sprite islaSpriteMoveRight1 = tl::LoadSprite(islaAvatarMoveright1);
tl::Sprite islaSpriteMoveRight2 = tl::LoadSprite(islaAvatarMoveright2);
tl::Sprite islaSpriteMoveLeft1 =  tl::LoadSprite(islaAvatarMoveleft1);
tl::Sprite islaSpriteMoveLeft2 =  tl::LoadSprite(islaAvatarMoveleft2);

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

static void RenderGameState(const tl::RenderBuffer &renderBuffer, const GameState &state)
{
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
			"GIVE ME BOBUX",
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
			TEXT_COLOR
		);

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
		return;
	}

	// background
	tl::Rect<float> backgroundRect;
	backgroundRect.halfSize = { 640.0f, 360.0f };
	backgroundRect.position = { 640.0f, 360.0f };
	tl::bitmap_interface_render(
		renderBuffer,
		backgroundBitmap,
		backgroundRect
	);

	// Show info about z-position
	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 100.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };

	tl::DrawAlphabetCharacters(renderBuffer, "COL", charFoot, 0x999999);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, state.collision, charFoot, 0x999999);

	RenderBlocks(renderBuffer, state);
	
	// player
	tl::bitmap_interface_render(
		renderBuffer,
		playerBitmaps,
		state.player
	);
}
