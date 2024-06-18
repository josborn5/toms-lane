#include <assert.h>
#include "./bitmap.hpp"

static const uint32_t red = 0xFF0000;
static const uint32_t green = 0x00FF00;
static const uint32_t blue = 0x0000FF;
static const uint32_t white = 0xFFFFFF;
static const uint32_t black = 0x000000;

static MemorySpace bitmapMemory;
static MemorySpace renderBufferPixels;
static MemorySpace renderBufferDepth;
static RenderBuffer renderBuffer;

static void InitializeMemory()
{
	bitmapMemory.sizeInBytes = 1024 * 60;
	tl::memory_interface_initialize(bitmapMemory);

	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;

	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferDepth.sizeInBytes = sizeof(float) * renderBuffer.width * renderBuffer.height;
	tl::memory_interface_initialize(renderBufferPixels);
	tl::memory_interface_initialize(renderBufferDepth);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;
	renderBuffer.depth = (float*)renderBufferDepth.content;

}

static void RunInitializeSmallBitmapTest(tl::bitmap& testBitmap)
{
	int fileReadResult = tl::file_interface_read("../src/platform/test.bmp", bitmapMemory);
	assert(fileReadResult == 0);

	tl::bitmap_interface_initialize(testBitmap, bitmapMemory);

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

void RunSmallBitmapRenderTest(const tl::bitmap testBitmap)
{
	tl::ClearScreen(renderBuffer, white);
	tl::bitmap_interface_render(renderBuffer, testBitmap, { 0, 0 });

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
	assert(*topLeftPixel == white);

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

static void RunInitializeLargeBitmapTest(tl::bitmap& largeBitmap)
{
	int fileReadResult = tl::file_interface_read("../src/platform/monochrome.bmp", bitmapMemory);
	assert(fileReadResult == 0);

	tl::bitmap_interface_initialize(largeBitmap, bitmapMemory);
	assert(largeBitmap.file_header.fileType == 0x4d42);
	assert(largeBitmap.file_header.fileSizeInBytes == 60062);
	assert(largeBitmap.file_header.reserved1 == 0);
	assert(largeBitmap.file_header.reserved2 == 0);
	assert(largeBitmap.file_header.offsetToPixelDataInBytes == 62);

	assert(largeBitmap.dibs_header.headerSizeInBytes == 40);

	assert(largeBitmap.dibs_header.width == 800);
	assert(largeBitmap.dibs_header.height == 600);

	assert(largeBitmap.dibs_header.numberOfColorPlanes == 1);
	assert(largeBitmap.dibs_header.bitsPerPixel == 1);
	assert(largeBitmap.dibs_header.compressionMethod == 0);
	assert(largeBitmap.dibs_header.imageSizeInBytes == 60000);

	assert(largeBitmap.dibs_header.horizontalPixelsPerMeter == 0);
	assert(largeBitmap.dibs_header.verticalPixelsPerMeter == 0);
	assert(largeBitmap.dibs_header.numberOfColorsInPalette == 0);
	assert(largeBitmap.dibs_header.numberOfImportantColors == 0);
}

static void RunLargeBitmapRenderTest(const tl::bitmap& largeBitmap)
{
	tl::ClearScreen(renderBuffer, red);
	tl::bitmap_interface_render(renderBuffer, largeBitmap, { 0, 0 });

	uint32_t* bottomLeftPixel = renderBuffer.pixels;
	uint32_t* rightOfBottomLeftPixel = renderBuffer.pixels + 1;
	assert(*bottomLeftPixel == white);
	assert(*rightOfBottomLeftPixel == black);

	tl::ClearScreen(renderBuffer, red);
	tl::bitmap_interface_render(renderBuffer, largeBitmap, { 6, 4 });

	assert(*bottomLeftPixel == red);

	uint32_t* sixAcrossFourUpFromBottomLeft = renderBuffer.pixels + 6 + (4 * renderBuffer.width);
	assert(*sixAcrossFourUpFromBottomLeft == white);
	assert(*(sixAcrossFourUpFromBottomLeft + 1) == black);
	assert(*(sixAcrossFourUpFromBottomLeft + 2) == white);
}

static void RunLargeBitmapTest()
{
	tl::bitmap largeBitmap;
	RunInitializeLargeBitmapTest(largeBitmap);
	RunLargeBitmapRenderTest(largeBitmap);
}

void RunBitmapTests()
{
	InitializeMemory();

	RunSmallBitmapTest();

	RunLargeBitmapTest();
}

