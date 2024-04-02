#include "./editor.hpp"

void Save(
	const tl::GameMemory& gameMemory,
	const tl::SpriteC& sprite,
	char* displayBuffer,
	char* filePath
) {
	// Serialize to string
	int charCount = SpriteCToCharString(sprite, gameMemory.transient);
	tl::MemorySpace toSaveToFile;
	toSaveToFile.content = gameMemory.transient.content;
	toSaveToFile.sizeInBytes = charCount * sizeof(char);
	if (tl::file_interface_write(filePath, toSaveToFile) == tl::Success)
	{
		displayBuffer[0] = 'S';
		displayBuffer[1] = 'A';
		displayBuffer[2] = 'V';
		displayBuffer[3] = 'E';
		displayBuffer[4] = 'D';
		displayBuffer[5] = '\0';
	}
	else
	{
		displayBuffer[0] = 'E';
		displayBuffer[1] = 'R';
		displayBuffer[2] = 'R';
		displayBuffer[3] = 'O';
		displayBuffer[4] = 'R';
		displayBuffer[5] = '\0';
	}
}
