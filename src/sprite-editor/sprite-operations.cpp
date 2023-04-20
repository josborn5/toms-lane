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

int AppendRowToSpriteC(tl::SpriteC& sprite, tl::MemorySpace spriteMemory)
{
	// Check there is space to add a final row
	int currentPixelCount = sprite.width * sprite.height;
	unsigned long currentPixelSpace = currentPixelCount * sizeof(tl::Color);
	unsigned long newPixelSpace = sprite.width * sizeof(tl::Color);
	unsigned long availableSpace = spriteMemory.sizeInBytes - currentPixelSpace;

	if (availableSpace < newPixelSpace)
	{
		return -1;
	}

	for (int i = 0; i < sprite.width; i += 1)
	{
		int pixelIndex = currentPixelCount + i;
		tl::Color pixel;
		pixel.r = 0.0f;
		pixel.g = 0.0f;
		pixel.b = 0.0f;
		pixel.a = 0.0f;
		sprite.content[pixelIndex] = pixel;
	}

	sprite.height += 1;

	return 0;
}

int AppendColumnToSpriteC(tl::SpriteC& sprite, tl::MemorySpace spriteMemory)
{
	// Check there is space to add a final row
	int currentPixelCount = sprite.width * sprite.height;
	unsigned long currentPixelSpace = currentPixelCount * sizeof(tl::Color);
	unsigned long newPixelSpace = sprite.height * sizeof(tl::Color);
	unsigned long availableSpace = spriteMemory.sizeInBytes - currentPixelSpace;

	if (availableSpace < newPixelSpace)
	{
		return -1;
	}

	for (int i = currentPixelCount - 1; i >= sprite.width; i -= 1)
	{
		int offset = i / sprite.width;
		int newIndex = i + offset;
		sprite.content[newIndex] = sprite.content[i];
		sprite.content[i].r = 0.0f;
		sprite.content[i].g = 0.0f;
		sprite.content[i].b = 0.0f;
		sprite.content[i].a = 0.0f;
	}

	sprite.width += 1;

	return 0;
}
