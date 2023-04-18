// Returns the number of chars
int SpriteCToCharString(const tl::SpriteC& sprite, tl::MemorySpace memory)
{
	int charCount = 0;
	int pixelCount = sprite.height * sprite.width;
	int dimensionCount = 20; // 10 chars per x & y size

	int charCountPerPixel = 4 * 4; // 3 number chars + 1 space char per R, G, B & A value for each pixel
	int totalCharCount = dimensionCount + (pixelCount * charCountPerPixel);

	if (memory.sizeInBytes < (unsigned long)totalCharCount)
	{
		return charCount;
	}

	char* cursor = (char*)memory.content;

	// Width
	tl::IntToCharString(sprite.width, cursor);
	while (*cursor)
	{
		cursor++;
		charCount += 1;
	}
	*cursor = '\n';
	cursor++;
	charCount += 1;

	// Height
	tl::IntToCharString(sprite.height, cursor);
	while (*cursor)
	{
		cursor++;
		charCount += 1;
	}
	*cursor = '\n';
	cursor++;
	charCount += 1;

	for (int i = 0; i < pixelCount; i += 1)
	{
		tl::Color color = sprite.content[i];
		tl::IntToCharString((int)(color.r * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = ' ';
		cursor++;
		charCount += 1;

		tl::IntToCharString((int)(color.g * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = ' ';
		cursor++;
		charCount += 1;

		tl::IntToCharString((int)(color.b * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = ' ';
		cursor++;
		charCount += 1;

		tl::IntToCharString((int)(color.a * 255), cursor);
		while (*cursor)
		{
			cursor++;
			charCount += 1;
		}
		*cursor = '\n';
		cursor++;
		charCount += 1;
	}
	*cursor = '\0';
	return charCount;
}