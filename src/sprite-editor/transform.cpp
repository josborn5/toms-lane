#include "../tl-library.hpp"

static void initializeBitmapFromSpriteC(const tl::SpriteC& sprite, tl::bitmap& bitmap)
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

	int spritePixelCount = sprite.width * sprite.height;

	for (int i = 0; i < spritePixelCount; i += 1)
	{

	}
}

