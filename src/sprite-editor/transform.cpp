#include "../tl-library.hpp"

struct RGB24Bit
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

static int InitializeBitmapFromSpriteC(
	const tl::SpriteC& sprite,
	tl::bitmap& bitmap,
	const tl::MemorySpace tempMemory)
{
	const int bitsPerPixel = 24;
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
	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;
	int spritePixelCount = sprite.width * sprite.height;

	// SpriteC origin is top left
	// Bitmap origin is bottom left
	int bottomLeftSpritePixelIndex =  spritePixelCount - sprite.width;
	for (int startRowPixelIndex = bottomLeftSpritePixelIndex; startRowPixelIndex >= 0; startRowPixelIndex  -= sprite.width)
	{
		for (int columnIndex = 0; columnIndex < sprite.width; columnIndex += 1)
		{
			int pixelIndex = startRowPixelIndex + columnIndex;
			tl::Color spriteColor = sprite.content[pixelIndex];
			RGB24Bit bitmapPixel;
			bitmapPixel.r = (uint8_t)(255.0f * spriteColor.r);
			bitmapPixel.g = (uint8_t)(255.0f * spriteColor.g);
			bitmapPixel.b = (uint8_t)(255.0f * spriteColor.b);
			*twentyFourBitContent = bitmapPixel;
			twentyFourBitContent++;
		}
	}

	return 0;
}

