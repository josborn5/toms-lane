#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

struct RGB24Bit
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

static void WriteColorTo24BitBitmap(const Color& spriteColor, int bitmapPixelIndex, tl::bitmap& bitmap)
{
	RGB24Bit bitmapPixel;
	bitmapPixel.r = (uint8_t)(255.0f * spriteColor.r);
	bitmapPixel.g = (uint8_t)(255.0f * spriteColor.g);
	bitmapPixel.b = (uint8_t)(255.0f * spriteColor.b);
	*((RGB24Bit*)bitmap.content + bitmapPixelIndex) = bitmapPixel;
}

int InitializeBitmapFromSpriteC(
	const SpriteC& sprite,
	tl::bitmap& bitmap,
	const tl::MemorySpace tempMemory)
{
	uint16_t bitsPerPixel = sprite.bitsPerPixel;
	uint32_t imageSizeInBytes = bitsPerPixel * sprite.height * sprite.width / 8;

	bitmap.file_header.fileType = 0x4d42;
	bitmap.file_header.reserved1 = 0;
	bitmap.file_header.reserved2 = 0;
	bitmap.file_header.offsetToPixelDataInBytes = 54;

	bitmap.dibs_header.headerSizeInBytes = 40;
	bitmap.dibs_header.width = sprite.width;
	bitmap.dibs_header.height = sprite.height;
	bitmap.dibs_header.numberOfColorPlanes = 1;
	bitmap.dibs_header.bitsPerPixel = bitsPerPixel;
	bitmap.dibs_header.compressionMethod = 0;
	bitmap.dibs_header.imageSizeInBytes = imageSizeInBytes;

	bitmap.file_header.fileSizeInBytes = bitmap.file_header.offsetToPixelDataInBytes + bitmap.dibs_header.imageSizeInBytes;

	if (bitmap.file_header.fileSizeInBytes > tempMemory.sizeInBytes) return -1;

	bitmap.content = (uint8_t*)tempMemory.content;
	int bitmapPixelIndex = 0;
	int spritePixelCount = sprite.width * sprite.height;

	// SpriteC origin is top left
	// Bitmap origin is bottom left
	int bottomLeftSpritePixelIndex =  spritePixelCount - sprite.width;
	for (int startRowPixelIndex = bottomLeftSpritePixelIndex; startRowPixelIndex >= 0; startRowPixelIndex -= sprite.width)
	{
		for (int columnIndex = 0; columnIndex < sprite.width; columnIndex += 1)
		{
			int spritePixelIndex = startRowPixelIndex + columnIndex;
			Color spriteColor = sprite.content[spritePixelIndex];
			WriteColorTo24BitBitmap(spriteColor, bitmapPixelIndex, bitmap);
			bitmapPixelIndex += 1;
		}
	}

	return 0;
}

static Color GetColorFrom24BitBitmap(const tl::bitmap& bitmap, int bitmapPixelIndex)
{
	RGB24Bit bitmapPixel = *((RGB24Bit*)bitmap.content + bitmapPixelIndex);

	Color spriteColor;
	spriteColor.r = (float)bitmapPixel.r / 255.0f;
	spriteColor.g = (float)bitmapPixel.g / 255.0f;
	spriteColor.b = (float)bitmapPixel.b / 255.0f;

	return spriteColor;
}

int InitializeSpriteCFromBitmap(
	SpriteC& sprite,
	const tl::bitmap& bitmap,
	const tl::MemorySpace spriteMemory)
{
	const int supportedBitsPerPixel = 24;
	int bytesPerPixel = supportedBitsPerPixel / 8;
	if (bitmap.dibs_header.bitsPerPixel != supportedBitsPerPixel) return -1;
	sprite.bitsPerPixel = bitmap.dibs_header.bitsPerPixel;

	int spritePixelCount = bitmap.dibs_header.imageSizeInBytes / bytesPerPixel;

	if ((sizeof(Color) * spritePixelCount) > spriteMemory.sizeInBytes) return -2;

	sprite.width = bitmap.dibs_header.width;
	sprite.height = bitmap.dibs_header.height;

	// SpriteC origin is top left
	// Bitmap origin is bottom left
	int bottomLeftSpritePixelIndex =  spritePixelCount - sprite.width;
	int bitmapPixelIndex = 0;
	for (int startRowPixelIndex = bottomLeftSpritePixelIndex; startRowPixelIndex >= 0; startRowPixelIndex -= sprite.width)
	{
		for (int columnIndex = 0; columnIndex < sprite.width; columnIndex += 1)
		{
			int spritePixelIndex = startRowPixelIndex + columnIndex;
			sprite.content[spritePixelIndex] = GetColorFrom24BitBitmap(bitmap, bitmapPixelIndex);
			bitmapPixelIndex += 1;
		}
	}

	return 0;
}
