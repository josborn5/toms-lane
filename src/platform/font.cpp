#include "./font.hpp"
#include "./geometry.hpp"
#include "./utilities.hpp"
#include "./software-rendering.hpp"
#include "./software-rendering.sprite.hpp"
#include "./generated-assets.hpp"

namespace tl
{
static const int sprite_count = 94;
static Sprite ascii_chars[sprite_count];

static bool initialized = false;

static void load_sprites(unsigned char source[], int size)
{
	int char_cursor = 0;
	unsigned char parsed_width = source[char_cursor];
	char_cursor += 2; // increment past the newline
	unsigned char parsed_height = source[char_cursor];
	char_cursor += 2; // increment past the newline

	int width = parsed_width - '0';
	int height = parsed_height - '0';

	for (int i = 0; i < sprite_count; i += 1)
	{
		ascii_chars[i].width = width;
		ascii_chars[i].height = height;
		ascii_chars[i].content = (char*)&(source[char_cursor]);

		int row_index = 0;
		while ((char_cursor < size) && (row_index < ascii_chars[i].height))
		{
			if (source[char_cursor] == '\n')
			{
				row_index += 1;
			}
			char_cursor += 1;
		}
	}
}

static int font_interface_initialize_from_sprite()
{
	load_sprites(font_mono_tlsf, font_mono_tlsf_len);
	initialized = true;

	return 0;
}

int font_interface_initialize()
{
	return font_interface_initialize_from_sprite();
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


