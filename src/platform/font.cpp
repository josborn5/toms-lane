#include "../tl-application.hpp"
#include "./geometry.hpp"
#include "./utilities.hpp"
#include "./software-rendering.hpp"
#include "./software-rendering.sprite.hpp"
#include "./font-mono.cpp"

namespace tl
{
static Sprite ascii_chars[94];

static bool initialized = false;

static void load_sprites(Sprite* target, int count, char* source)
{
	char* readCursor = source;

	char parsedWidth = *readCursor;
	readCursor += 2; // increment past the new line
	char parsedHeight = *readCursor;
	readCursor += 2; // increment past the new line

	int width = parsedWidth - '0';
	int height = parsedHeight - '0';

	for (int i = 0; i < count; i += 1)
	{
		target[i].width = width;
		target[i].height = height;
		target[i].content = readCursor;

		int rowCounter = 0;
		while (*readCursor && (rowCounter < target[i].height))
		{
			if (*readCursor == '\n')
			{
				rowCounter += 1;
			}
			readCursor++;
		}
	}
}

void font_interface_initialize()
{
	load_sprites(ascii_chars, 94, ASCII_33_126);
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
			if (*letterAt >= '!' && *letterAt <= '~')
			{
				int ascii_char_index = *letterAt - '!';
				Sprite renderChar = ascii_chars[ascii_char_index];
				tl::DrawSprite(buffer, renderChar, charRect, color);
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
			Sprite neg_sprite = ascii_chars['-' - '!'];
			tl::DrawSprite(buffer, neg_sprite, charRect, color);
		}
		else
		{
			char digitIndex = digit - '!';
			Sprite charDigit = ascii_chars[digitIndex];
			tl::DrawSprite(buffer, charDigit, charRect, color);
		}

		charRect.position.x += charWidth;
		intAsString++;
	}
}


}


