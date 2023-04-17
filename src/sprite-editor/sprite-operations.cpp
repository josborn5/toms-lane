int SpriteCToCharString(const tl::SpriteC& sprite, tl::MemorySpace memory)
{
	int pixelCount = sprite.height * sprite.width;
	int dimensionCount = 20; // 10 chars per x & y size

	int charCountPerPixel = 4 * 4; // 3 number chars + 1 space char per R, G, B & A value for each pixel
	int totalCharCount = dimensionCount + (pixelCount * charCountPerPixel);

	if (memory.sizeInBytes < (unsigned long)totalCharCount)
	{
		return 1;
	}

	char* cursor = (char*)memory.content;

	tl::IntToCharString(sprite.width, memory);
	while (*cursor) cursor++;
	*cursor = '\n';
	cursor++;

	tl::IntToCharString(sprite.height, memory);
	while (*cursor) cursor++;
	*cursor = '\n';
	cursor++;

	for (int i = 0; i < pixelCount; i += 1)
	{
		tl::Color color = sprite.content[i];
		tl::IntToCharString((int)(color.r * 255), cursor);
		while (*cursor) cursor++;
		*cursor = ' ';
		cursor++;

		tl::IntToCharString((int)(color.g * 255), cursor);
		while (*cursor) cursor++;
		*cursor = ' ';
		cursor++;

		tl::IntToCharString((int)(color.b * 255), cursor);
		while (*cursor) cursor++;
		*cursor = ' ';
		cursor++;

		tl::IntToCharString((int)(color.a * 255), cursor);
		while (*cursor) cursor++;
		*cursor = '\n';
		cursor++;
	}

	*cursor = '\0';
	return 0;
}