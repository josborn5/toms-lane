#include "../tl-application.hpp"
#include "./geometry.hpp"
#include "./utilities.hpp"
#include "./software-rendering.hpp"
#include "./software-rendering.sprite.hpp"

namespace tl
{

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
	initialized = true;
}

float font_interface_render_alphabet(
	const RenderBuffer& buffer,
	char* text,
	const Rect<float>& firstCharFootprint,
	uint32_t color
)
{
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

void font_interface_render_integer(
	const RenderBuffer& buffer,
	int number,
	const Rect<float>& firstCharFootprint,
	uint32_t color
)
{
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


