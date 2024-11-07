#include "../tl-application.hpp"
#include "./font.hpp"
#include "./geometry.hpp"
#include "./utilities.hpp"
#include "./software-rendering.hpp"
#include "./software-rendering.sprite.hpp"

namespace tl
{
static const int sprite_count = 94;
static Sprite ascii_chars[sprite_count];

static bool initialized = false;

static void load_sprites(char* source)
{
	char* readCursor = source;

	char parsedWidth = *readCursor;
	readCursor += 2; // increment past the new line
	char parsedHeight = *readCursor;
	readCursor += 2; // increment past the new line

	int width = parsedWidth - '0';
	int height = parsedHeight - '0';

	for (int i = 0; i < sprite_count; i += 1)
	{
		ascii_chars[i].width = width;
		ascii_chars[i].height = height;
		ascii_chars[i].content = readCursor;

		int rowCounter = 0;
		while (*readCursor && (rowCounter < ascii_chars[i].height))
		{
			if (*readCursor == '\n')
			{
				rowCounter += 1;
			}
			readCursor++;
		}
	}
}

int font_interface_initialize_from_sprite(const sprite_font& sprite)
{
	load_sprites(sprite.content);
	initialized = true;

	return 0;
}

int font_interface_initialize_from_file(char* file_name, MemorySpace& target, MemorySpace& remainder)
{
	uint64_t font_file_size;
	tl::file_interface_size_get(file_name, font_file_size);

	remainder = target;
	int file_read_result = file_interface_read(file_name, target);
	if (file_read_result != 0)
	{
		return file_read_result;
	}

	load_sprites((char*)target.content);
	initialized = true;

	target = CarveMemorySpace(font_file_size, remainder);

	return 0;
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


