#include <assert.h>
#include "./bitmap.hpp"

static MemorySpace testMemory;

static void InitializeMemory()
{
	testMemory.sizeInBytes = 1024 * 60;
	tl::memory_interface_initialize(testMemory);
}

static void RunInitializeSmallBitmapTest(tl::bitmap& testBitmap)
{
	int fileReadResult = tl::file_interface_read("../src/platform/test.bmp", testMemory);
	assert(fileReadResult == 0);

	tl::bitmap_interface_initialize(testBitmap, testMemory);

	assert(testBitmap.file_header.fileType == 0x4d42);
	assert(testBitmap.file_header.fileSizeInBytes == 342);
	assert(testBitmap.file_header.reserved1 == 0);
	assert(testBitmap.file_header.reserved2 == 0);
	assert(testBitmap.file_header.offsetToPixelDataInBytes == 54);

	assert(testBitmap.dibs_header.headerSizeInBytes == 40);

	assert(testBitmap.dibs_header.width == 12);
	assert(testBitmap.dibs_header.height == 8);

	assert(testBitmap.dibs_header.numberOfColorPlanes == 1);
	assert(testBitmap.dibs_header.bitsPerPixel == 24);
	assert(testBitmap.dibs_header.compressionMethod == 0);
	assert(testBitmap.dibs_header.imageSizeInBytes == 288);

	assert(testBitmap.dibs_header.horizontalPixelsPerMeter == 0);
	assert(testBitmap.dibs_header.verticalPixelsPerMeter == 0);
	assert(testBitmap.dibs_header.numberOfColorsInPalette == 0);
	assert(testBitmap.dibs_header.numberOfImportantColors == 0);
}

static void RunInitializeLargeBitmapTest(tl::bitmap& largeBitmap)
{
	int fileReadResult = tl::file_interface_read("../src/platform/monochrome.bmp", testMemory);
	assert(fileReadResult == 0);

	tl::bitmap_interface_initialize(largeBitmap, testMemory);
	assert(largeBitmap.file_header.fileType == 0x4d42);
	assert(largeBitmap.file_header.fileSizeInBytes == 60062);

	assert(largeBitmap.dibs_header.width == 800);
	assert(largeBitmap.dibs_header.height == 600);
}

void RunSmallBitmapRenderTest(const tl::bitmap testBitmap)
{
	RenderBuffer renderBuffer;
	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;
	MemorySpace renderBufferPixels;
	MemorySpace renderBufferDepth;
	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferDepth.sizeInBytes = sizeof(float) * renderBuffer.width * renderBuffer.height;
	tl::memory_interface_initialize(renderBufferPixels);
	tl::memory_interface_initialize(renderBufferDepth);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;
	renderBuffer.depth = (float*)renderBufferDepth.content;

	tl::ClearScreen(renderBuffer, 0x000000);
	tl::bitmap_interface_render(renderBuffer, testBitmap, { 0, 0 });

	const uint32_t red = 0xFFFFFF;
	const uint32_t green = 0x00FF00;
	const uint32_t blue = 0x0000FF;
	const uint32_t white = 0xFFFFFF;
	const uint32_t black = 0x000000;
	uint32_t* bottomLeftPixel = renderBuffer.pixels;
	uint32_t* rightOfBottomLeftPixel = renderBuffer.pixels + 1;
	uint32_t* bottomRightPixel = renderBuffer.pixels + testBitmap.dibs_header.width - 1;
	int pixelCount = testBitmap.dibs_header.width * testBitmap.dibs_header.height;
	uint32_t* topRightPixel = renderBuffer.pixels + pixelCount - 1;
	uint32_t* topLeftPixel = renderBuffer.pixels + pixelCount - testBitmap.dibs_header.width - 1;

	assert(*bottomLeftPixel == green);
	assert(*rightOfBottomLeftPixel == white);
	assert(*bottomRightPixel == blue);
	assert(*topRightPixel == blue);
	assert(*topLeftPixel == red);

	tl::ClearScreen(renderBuffer, black);
	tl::bitmap_interface_render(renderBuffer, testBitmap, { 6, 4 });

	assert(*bottomLeftPixel == black);
	assert(*bottomRightPixel == black);

	uint32_t* sixAcrossFourUpFromBottomLeft = renderBuffer.pixels + 6 + (4 * renderBuffer.width);
	assert(*sixAcrossFourUpFromBottomLeft == green);
	assert(*(sixAcrossFourUpFromBottomLeft + 1) == white);
	assert(*(sixAcrossFourUpFromBottomLeft + 2) == black);
}

static void RunSmallBitmapTest()
{
	tl::bitmap smallBitmap;
	RunInitializeSmallBitmapTest(smallBitmap);
	RunSmallBitmapRenderTest(smallBitmap);
}

static void RunLargeBitmapTest()
{
	tl::bitmap largeBitmap;
	RunInitializeLargeBitmapTest(largeBitmap);
}

void RunBitmapTests()
{
	InitializeMemory();

	RunSmallBitmapTest();

	RunLargeBitmapTest();
}

