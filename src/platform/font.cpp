#include "../tl-application.hpp"
#include "./geometry.hpp"
#include "./utilities.hpp"
#include "./software-rendering.hpp"
#include "./software-rendering.sprite.hpp"

namespace tl
{

static char* letterDefinitions = "\
 00\n\
0  0\n\
0  0\n\
0000\n\
0  0\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
000\n\
 000\n\
0\n\
0\n\
0\n\
0\n\
0\n\
 000\n\
000\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
000\n\
0000\n\
0\n\
0\n\
000\n\
0\n\
0\n\
0000\n\
0000\n\
0\n\
0\n\
000\n\
0\n\
0\n\
0\n\
 000\n\
0\n\
0\n\
0 00\n\
0  0\n\
0  0\n\
 000\n\
0  0\n\
0  0\n\
0  0\n\
0000\n\
0  0\n\
0  0\n\
0  0\n\
000\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
000\n\
 000\n\
   0\n\
   0\n\
   0\n\
0  0\n\
0  0\n\
 000\n\
0  0\n\
0  0\n\
0 0\n\
00\n\
0 0\n\
0  0\n\
0  0\n\
0\n\
0\n\
0\n\
0\n\
0\n\
0\n\
0000\n\
00 00\n\
0 0 0\n\
0 0 0\n\
0   0\n\
0   0\n\
0   0\n\
0   0\n\
0   0\n\
0   0\n\
00  0\n\
0 0 0\n\
0  00\n\
0   0\n\
0   0\n\
 00 \n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
 00\n\
000\n\
0  0\n\
0  0\n\
000\n\
0\n\
0\n\
0\n\
 000\n\
0   0\n\
0   0\n\
0   0\n\
0 0 0\n\
0  0\n\
 00 0\n\
000\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
0  0\n\
 000\n\
0\n\
0\n\
 00\n\
   0\n\
   0\n\
000\n\
000\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
 00\n\
0   0\n\
0   0\n\
0   0\n\
 0 0\n\
 0 0\n\
  0\n\
  0\n\
0   0\n\
0   0\n\
0   0\n\
0 0 0\n\
0 0 0\n\
 0 0\n\
 0 0\n\
0   0\n\
0   0\n\
 0 0\n\
  0\n\
 0 0\n\
0   0\n\
0   0\n\
0   0\n\
0   0\n\
 0 0\n\
 0 0\n\
  0\n\
  0\n\
  0\n\
0000\n\
   0\n\
  0\n\
 0\n\
0\n\
0\n\
0000\n";


static Sprite digits[10];
static Sprite letters[26];
static Sprite negSprite;
static Sprite periodSprite;
static Sprite colonSprite;
static Sprite backSlashSprite;
static Sprite forwardSlashSprite;

static bool initialized = false;

static int GetLetterIndex(char c)
{
	return c - 'A';
}

static int GetDigitIndex(char c)
{
	return c - '0';
}

void font_interface_initialize()
{
	char* readCursor = letterDefinitions;


	for (int i = 0; i < 26; i += 1)
	{
		letters[i].width = 4;
		letters[i].height = 7;
		letters[i].content = readCursor;

		int rowCounter = 0;
		while (*readCursor && (rowCounter < letters[i].height))
		{
			if (*readCursor == '\n')
			{
				rowCounter += 1;
			}
			readCursor++;
		}
	}

	digits[0] = LoadSprite("\
 000 \n\
0   0\n\
0  00\n\
0 0 0\n\
00  0\n\
0   0\n\
 000");
	digits[1] = LoadSprite("\
 0\n\
00\n\
 0\n\
 0\n\
 0\n\
 0\n\
000");
	digits[2] = LoadSprite("\
 00\n\
0  0\n\
   0\n\
  0\n\
 0\n\
0\n\
0000");
	digits[3] = LoadSprite("\
 00\n\
0  0\n\
   0\n\
 00\n\
   0\n\
0  0\n\
 00");
	digits[4] = LoadSprite("\
  00\n\
 0 0\n\
0  0\n\
0000\n\
   0\n\
   0\n\
   0");
	digits[5] = LoadSprite("\
0000\n\
0\n\
0\n\
000\n\
   0\n\
   0\n\
000");
	digits[6] = LoadSprite("\
 000\n\
0\n\
0\n\
000\n\
0  0\n\
0  0\n\
 00");
	digits[7] = LoadSprite("\
0000\n\
   0\n\
   0\n\
  0\n\
 0\n\
0\n\
0");
	digits[8] = LoadSprite("\
 00\n\
0  0\n\
0  0\n\
 00\n\
0  0\n\
0  0\n\
 00");
	digits[9] = LoadSprite("\
 00\n\
0  0\n\
0  0\n\
 00\n\
  0\n\
 0\n\
0");

	negSprite = LoadSprite("\
\n\
\n\
\n\
0000\n\
\n\
\n\
");
	periodSprite = LoadSprite("\
\n\
\n\
\n\
\n\
\n\
 00 \n\
 00 ");
	colonSprite = LoadSprite("\
\n\
 00 \n\
 00 \n\
\n\
 00 \n\
 00 \n\
");
	backSlashSprite = LoadSprite("\
0\n\
 0\n\
 0\n\
  0\n\
  0\n\
   0");
	forwardSlashSprite = LoadSprite("\
   0\n\
  0\n\
  0\n\
 0\n\
 0\n\
0");

	initialized = true;
}

float font_interface_render_chars(
	const RenderBuffer& buffer,
	char* text,
	const Rect<float>& firstCharFootprint,
	uint32_t color
)
{
	if (!initialized) return 0.0f;

	Rect<float> charRect;
	charRect.halfSize = firstCharFootprint.halfSize;
	charRect.position = tl::Vec2<float> { firstCharFootprint.position.x, firstCharFootprint.position.y };
	float spaceWidth = 0.2f * firstCharFootprint.halfSize.x;
	float charWidth = (2.0f * firstCharFootprint.halfSize.x) + spaceWidth;
	for (char *letterAt = text; *letterAt; letterAt++)
	{
		if (*letterAt != ' ')
		{
			Sprite* renderChar = nullptr;
			if (*letterAt >= 'A' && *letterAt <= 'Z')
			{
				int letterIndex = GetLetterIndex(*letterAt);
				renderChar = &letters[letterIndex];
			}
			else if (*letterAt >= '0' && *letterAt <= '9')
			{
				int digitIndex = GetDigitIndex(*letterAt);
				renderChar = &digits[digitIndex];
			}
			else if (*letterAt == '-')
			{
				renderChar = &negSprite;
			}
			else if (*letterAt == '.')
			{
				renderChar = &periodSprite;
			}
			else if (*letterAt == '\\')
			{
				renderChar = &backSlashSprite;
			}
			else if (*letterAt == '/')
			{
				renderChar = &forwardSlashSprite;
			}
			else if (*letterAt == ':')
			{
				renderChar = &colonSprite;
			}

			if (renderChar != nullptr)
			{
				tl::DrawSprite(buffer, *renderChar, charRect, color);
			}
			else
			{
				tl::DrawRect(buffer, color, charRect);
			}
		}
		charRect.position.x += charWidth;
	}

	return charRect.position.x - firstCharFootprint.position.x;
}

void font_interface_render_int(
	const RenderBuffer& buffer,
	int number,
	const Rect<float>& firstCharFootprint,
	uint32_t color
)
{
	if (!initialized) return;

	Rect<float> charRect;
	charRect.halfSize = firstCharFootprint.halfSize;
	charRect.position = tl::Vec2<float> { firstCharFootprint.position.x, firstCharFootprint.position.y };
	float spaceWidth = 0.2f * firstCharFootprint.halfSize.x;
	float charWidth = (2.0f * firstCharFootprint.halfSize.x) + spaceWidth;

	char characterBuffer[10];
	MemorySpace charBuffer;
	charBuffer.content = characterBuffer;
	charBuffer.sizeInBytes = 10;
	if (IntToCharString(number, charBuffer) != 0)
	{
		return;
	}

	char* intAsString = (char*)charBuffer.content;

	while (*intAsString)
	{
		char digit = *intAsString;
		if (digit == '-')
		{
			tl::DrawSprite(buffer, negSprite, charRect, color);
		}
		else
		{
			char digitIndex = (digit) - '0';
			Sprite charDigit = digits[digitIndex];
			tl::DrawSprite(buffer, charDigit, charRect, color);
		}

		charRect.position.x += charWidth;
		intAsString++;
	}
}


}


