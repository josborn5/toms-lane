void Save(
	const tl::GameMemory& gameMemory,
	const tl::SpriteC& sprite,
	char* commandBuffer
) {
	// Serialize to string
	int charCount = SpriteCToCharString(sprite, gameMemory.transient);
	tl::MemorySpace toSaveToFile;
	toSaveToFile.content = gameMemory.transient.content;
	toSaveToFile.sizeInBytes = charCount * sizeof(char);
	if (tl::WriteFile(filePath, toSaveToFile) == tl::Success)
	{
		commandBuffer[0] = 'S';
		commandBuffer[1] = 'A';
		commandBuffer[2] = 'V';
		commandBuffer[3] = 'E';
		commandBuffer[4] = 'D';
	}
	else
	{
		commandBuffer[0] = 'E';
		commandBuffer[1] = 'R';
		commandBuffer[2] = 'R';
		commandBuffer[3] = 'O';
		commandBuffer[4] = 'R';
	}
}
