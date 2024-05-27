#include <assert.h>
#include "./bitmap.hpp"

void RunBitmapTests()
{
	MemorySpace memory;
	bitmap testBitmap;
	memory.sizeInBytes = 1024;
	tl::memory_interface_initialize(memory);
	int fileReadResult = tl::file_interface_read("../src/platform/test.bmp", memory);
	assert(fileReadResult == 0);
	tl::bitmap_interface_initialize(testBitmap, memory);

	assert(testBitmap.header->fileType == 0x4d42);
	assert(testBitmap.header->fileSizeInBytes == 342);
	assert(testBitmap.header->reserved1 == 0);
	assert(testBitmap.header->reserved2 == 0);

	assert(testBitmap.header->offsetToPixelDataInBytes == 54);
	assert(testBitmap.header->headerSizeInBytes == 40);

	assert(testBitmap.header->width == 12);
	assert(testBitmap.header->height == 8);

	assert(testBitmap.header->numberOfColorPlanes == 1);
	assert(testBitmap.header->numberOfBitsPerPixel == 24);
	assert(testBitmap.header->compressionMethod == 0);
	assert(testBitmap.header->imageSizeInBytes == 288);

	assert(testBitmap.header->horizontalPixelsPerMeter == 0);
	assert(testBitmap.header->verticalPixelsPerMeter == 0);
	assert(testBitmap.header->numberOfColorsInPalette == 0);
	assert(testBitmap.header->numberOfImportantColors == 0);

}

