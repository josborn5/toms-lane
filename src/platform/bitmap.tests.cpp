#include <assert.h>
#include "./bitmap.hpp"

static MemorySpace testMemory;
static bitmap testBitmap;

static void OneTimeSetup()
{
	testMemory.sizeInBytes = 1024;
	tl::memory_interface_initialize(testMemory);
	int fileReadResult = tl::file_interface_read("../src/platform/test.bmp", testMemory);
	assert(fileReadResult == 0);
}

static void RunBitmapInitializeTest()
{
	tl::bitmap_interface_initialize(testBitmap, testMemory);

	assert(testBitmap.file_header->fileType == 0x4d42);
	assert(testBitmap.file_header->fileSizeInBytes == 342);
	assert(testBitmap.file_header->reserved1 == 0);
	assert(testBitmap.file_header->reserved2 == 0);
	assert(testBitmap.file_header->offsetToPixelDataInBytes == 54);

	assert(testBitmap.dibs_header->headerSizeInBytes == 40);
	assert(testBitmap.dibs_header->width == 12);
	assert(testBitmap.dibs_header->height == 8);

	assert(testBitmap.dibs_header->numberOfColorPlanes == 1);
	assert(testBitmap.dibs_header->bitsPerPixel == 24);
	assert(testBitmap.dibs_header->compressionMethod == 0);
	assert(testBitmap.dibs_header->imageSizeInBytes == 288);

	assert(testBitmap.dibs_header->horizontalPixelsPerMeter == 0);
	assert(testBitmap.dibs_header->verticalPixelsPerMeter == 0);
	assert(testBitmap.dibs_header->numberOfColorsInPalette == 0);
	assert(testBitmap.dibs_header->numberOfImportantColors == 0);
}

void RunBitmapRenderTest()
{
	RenderBuffer renderBuffer;
	renderBuffer.width = 30;
	renderBuffer.height = 20;
	renderBuffer.bytesPerPixel = 4;
	MemorySpace renderBufferPixels;
	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	tl::memory_interface_initialize(renderBufferPixels);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;

	tl::bitmap_interface_render(renderBuffer, testBitmap, { 0, 0 });

	assert((uint32_t)*renderBuffer.pixels == 0x00FF00);
}

void RunBitmapTests()
{
	OneTimeSetup();

	RunBitmapInitializeTest();

	RunBitmapRenderTest();
}

