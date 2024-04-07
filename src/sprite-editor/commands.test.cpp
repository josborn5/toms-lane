#include "./commands.hpp"

// mock implementations of libraries used
int tl::file_interface_write(
	char* fileName,
	const MemorySpace& writeBuffer
)
{
	return 0;
}

int tl::IntToCharString(int a, char* b)
{
	return 0;
}

static void SetColor(tl::Color& color)
{
	color.r = 255.0f;
	color.g = 255.0f;
	color.b = 255.0f;
	color.a = 255.0f;
}


// tests
static void AddColumnTests()
{
	tl::Color spriteContent[8];
	SetColor(spriteContent[0]);

	tl::SpriteC testSprite;
	tl::MemorySpace testMemory;
	testMemory.content = spriteContent;
	testMemory.sizeInBytes = sizeof(tl::Color) * 8;

	AppendColumnToSpriteC(testSprite, testMemory);
}

int main()
{
	AddColumnTests();
	return 0;
}
