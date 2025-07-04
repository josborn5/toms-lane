#include <assert.h>
#include "./generated-test-assets.hpp"
#include "./bitmap.hpp"

static const uint32_t grey = 0xAAAAAA;
static const uint32_t red = 0xFF0000;
static const uint32_t green = 0x00FF00;
static const uint32_t blue = 0x0000FF;
static const uint32_t white = 0xFFFFFF;
static const uint32_t black = 0x000000;

static const int smallBitmapFileSizeInBytes = 342;

static MemorySpace bitmapReadMemory;
static MemorySpace bitmapWriteMemory;
static MemorySpace renderBufferPixels;
static MemorySpace renderBufferDepth;
static RenderBuffer renderBuffer;

template<typename T>
void assert_whole_number(T actual, T expected) {
	printf("\nactual: %d, expected: %d\n", actual, expected);
	assert(actual == expected);
}

static void InitializeMemory()
{
	bitmapReadMemory.sizeInBytes = 1024 * 60;
	bitmapReadMemory.content = malloc(bitmapReadMemory.sizeInBytes);
	bitmapWriteMemory.sizeInBytes = 1024 * 60;
	bitmapReadMemory.content = malloc(bitmapWriteMemory.sizeInBytes);

	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;

	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferPixels.content = malloc(renderBufferPixels.sizeInBytes);
	renderBufferDepth.sizeInBytes = sizeof(float) * renderBuffer.width * renderBuffer.height;
	renderBufferDepth.content = malloc(renderBufferDepth.sizeInBytes);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;
	renderBuffer.depth = (float*)renderBufferDepth.content;
}

static void RunInitializeSmallBitmapTest(tl::bitmap& testBitmap)
{
	MemorySpace test_bitmap_memory_space;
	test_bitmap_memory_space.content = &test_bmp;
	test_bitmap_memory_space.sizeInBytes = sizeof(unsigned char) * test_bmp_len;

	tl::bitmap_interface_initialize(testBitmap, test_bitmap_memory_space);

	assert(testBitmap.file_header.fileType == 0x4d42);
	assert(testBitmap.file_header.fileSizeInBytes == smallBitmapFileSizeInBytes );
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

	assert(testBitmap.color_table.size == 0);
}

static void RunBitmapWriteTest(const bitmap& bitmap)
{
	int writeResult = tl::bitmap_interface_write(bitmap, bitmapWriteMemory);
	assert(writeResult == tl::bitmap_write_success);

	uint8_t* readMemory = (uint8_t*)bitmapReadMemory.content;
	uint8_t* writeMemory = (uint8_t*)bitmapWriteMemory.content;

	const int fileHeaderSizeInBytes = 14;
	int minPaddingIndex = fileHeaderSizeInBytes + bitmap.dibs_header.headerSizeInBytes + (bitmap.color_table.size * 4);
	int maxPaddingIndex = bitmap.file_header.offsetToPixelDataInBytes;
	for (int i = 0; i < bitmap.file_header.fileSizeInBytes; i += 1)
	{
		if (i < minPaddingIndex || i > maxPaddingIndex) // don't care about padding memory content
		{
			assert_whole_number<uint8_t>(*readMemory, *writeMemory);
		}
		readMemory++;
		writeMemory++;
	}
}

static void initialize_4_bit_bitmap_test_run() {
	MemorySpace test_bitmap_memory_space;
	test_bitmap_memory_space.content = &bitmap_4bit_bmp;
	test_bitmap_memory_space.sizeInBytes = sizeof(unsigned char) * bitmap_4bit_bmp_len;

	tl::bitmap test_bitmap;

	tl::bitmap_interface_initialize(test_bitmap, test_bitmap_memory_space);

	assert_whole_number<uint16_t>(test_bitmap.file_header.fileType, 0x4d42);
	assert_whole_number<int32_t>(test_bitmap.file_header.offsetToPixelDataInBytes, 118);

	assert_whole_number<uint32_t>(test_bitmap.dibs_header.headerSizeInBytes, 40);
	assert_whole_number<uint16_t>(test_bitmap.dibs_header.bitsPerPixel, 4);

	assert_whole_number<int32_t>(test_bitmap.color_table.size, 16);

	tl::ClearScreen(renderBuffer, grey);
	tl::bitmap_interface_render(renderBuffer, test_bitmap, tl::Vec2<int>{ 0, 0 });

	uint32_t* bottom_left_color = renderBuffer.pixels;
	assert_whole_number<uint32_t>(*bottom_left_color, 0x000000);

	uint32_t test_pixel_data = 0xFFFFFF;
	bitmap_interface_get_pixel_data(test_bitmap, 0, 0, test_pixel_data);
	assert_whole_number<uint32_t>(test_pixel_data, 0);

	bitmap_interface_get_pixel_data(test_bitmap, 0, 0, test_pixel_data);
	assert_whole_number<uint32_t>(test_pixel_data, 0);
	bitmap_interface_get_pixel_data(test_bitmap, 1, 0, test_pixel_data);
	assert_whole_number<uint32_t>(test_pixel_data, 15);
	bitmap_interface_get_pixel_data(test_bitmap, 2, 0, test_pixel_data);
	assert_whole_number<uint32_t>(test_pixel_data, 9);
	bitmap_interface_get_pixel_data(test_bitmap, 3, 0, test_pixel_data);
	assert_whole_number<uint32_t>(test_pixel_data, 10);
	bitmap_interface_get_pixel_data(test_bitmap, 4, 0, test_pixel_data);
	assert_whole_number<uint32_t>(test_pixel_data, 12);

	assert_whole_number<uint32_t>(test_bitmap.color_table.content[0], 0x000000);
	assert_whole_number<uint32_t>(test_bitmap.color_table.content[15], 0xFFFFFF);
	assert_whole_number<uint32_t>(test_bitmap.color_table.content[9], 0xFF0000);
	assert_whole_number<uint32_t>(test_bitmap.color_table.content[10], 0x00FF00);
	assert_whole_number<uint32_t>(test_bitmap.color_table.content[12], 0x0000FF);

	RunBitmapWriteTest(test_bitmap);
}

static void RunSmallBitmapRenderTest(const tl::bitmap testBitmap)
{
	tl::ClearScreen(renderBuffer, grey);
	tl::bitmap_interface_render(renderBuffer, testBitmap, tl::Vec2<int>{ 0, 0 });

	uint32_t* bottomLeftPixel = renderBuffer.pixels;
	uint32_t* bottomRightPixel = renderBuffer.pixels + renderBuffer.width - 1;
	int pixelCount = renderBuffer.width * renderBuffer.height;
	uint32_t* topRightPixel = renderBuffer.pixels + pixelCount - 1;
	uint32_t* topLeftPixel = renderBuffer.pixels + pixelCount - renderBuffer.width;

	assert(*bottomLeftPixel == green);
	uint32_t bottomLeftPixelColorTest;
	assert(bitmap_interface_get_pixel_data(testBitmap, 0, 0, bottomLeftPixelColorTest) == 0);
	assert(bottomLeftPixelColorTest == green);

	assert(*(bottomLeftPixel + 1) == white);
	assert(*(bottomLeftPixel + 2) == black);

	assert(*(bottomRightPixel - 2) == black);
	assert(*(bottomRightPixel - 1) == white);
	assert(*bottomRightPixel == blue);

	assert(*topLeftPixel == red);
	assert(*(topLeftPixel + 1) == white);
	assert(*(topLeftPixel + 2) == black);

	assert(*(topRightPixel - 2) == black);
	assert(*(topRightPixel - 1) == white);
	assert(*topRightPixel == blue);

	tl::ClearScreen(renderBuffer, grey);
	tl::bitmap_interface_render(renderBuffer, testBitmap, tl::Vec2<int>{ 6, 4 });

	assert(*bottomLeftPixel == grey);
	assert(*bottomRightPixel == grey);

	uint32_t* sixAcrossFourUpFromBottomLeft = renderBuffer.pixels + 6 + (4 * renderBuffer.width);
	assert(*sixAcrossFourUpFromBottomLeft == green);
	assert(*(sixAcrossFourUpFromBottomLeft + 1) == white);
	assert(*(sixAcrossFourUpFromBottomLeft + 2) == black);

	assert(*(topRightPixel - 2) == black);
	assert(*(topRightPixel - 1) == black);
	assert(*topRightPixel == black);

	// Render scaled up by a factor of 2
	tl::ClearScreen(renderBuffer, grey);
	tl::Rect<float> renderFootprint;
	renderFootprint.halfSize = {
		(float)renderBuffer.width,
		(float)renderBuffer.height
	};
	renderFootprint.position = renderFootprint.halfSize;
	tl::bitmap_interface_render(renderBuffer, testBitmap, renderFootprint);

	assert(*(bottomLeftPixel) == green);
	assert(*(bottomLeftPixel + 1) == green);
	assert(*(bottomLeftPixel + 2) == white);
	assert(*(bottomLeftPixel + 3) == white);
	assert(*(bottomLeftPixel + 4) == black);
}

static void RunBitmapWriteToSmallMemoryTest(const bitmap& bitmap)
{
	MemorySpace uninitializedMemory;

	int writeResult = tl::bitmap_interface_write(bitmap, uninitializedMemory);
	assert(writeResult == tl::bitmap_write_not_enough_space);
}

static void RunBitmapReadFromBadMemoryTests(bitmap& bitmap)
{
	MemorySpace badMemory;

	int readResult = tl::bitmap_interface_initialize(bitmap, badMemory);

	assert(readResult == tl::bitmap_read_missing_memory_source);

	// source memory is not big enough to read the file size
	badMemory.content = bitmapReadMemory.content;
	badMemory.sizeInBytes = 5;
	readResult = tl::bitmap_interface_initialize(bitmap, badMemory);
	assert(readResult == tl::bitmap_read_invalid_memory_source);

	// source memory is big enough to read the file size but is smaller that the read file size
	badMemory.sizeInBytes = smallBitmapFileSizeInBytes - 1; 
	readResult = tl::bitmap_interface_initialize(bitmap, badMemory);
	assert(readResult == tl::bitmap_read_invalid_memory_source);
}

static void RunSmallBitmapTest()
{
	tl::bitmap smallBitmap;
	RunInitializeSmallBitmapTest(smallBitmap);
	RunSmallBitmapRenderTest(smallBitmap);
	RunBitmapWriteTest(smallBitmap);
	RunBitmapWriteToSmallMemoryTest(smallBitmap);
	RunBitmapReadFromBadMemoryTests(smallBitmap);
}

static void RunInitializeLargeBitmapTest(tl::bitmap& largeBitmap)
{
	MemorySpace test_bitmap_memory_space;
	test_bitmap_memory_space.content = &monochrome_bmp;
	test_bitmap_memory_space.sizeInBytes = sizeof(unsigned char) * monochrome_bmp_len;

	tl::bitmap_interface_initialize(largeBitmap, test_bitmap_memory_space);
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

	assert(largeBitmap.color_table.size == 2);
	assert(largeBitmap.color_table.content[0] == black);
	assert(largeBitmap.color_table.content[1] == white);
}

static void RunLargeBitmapRenderTest(const tl::bitmap& largeBitmap)
{
	tl::ClearScreen(renderBuffer, red);
	tl::bitmap_interface_render(renderBuffer, largeBitmap, tl::Vec2<int>{ 0, 0 });

	uint32_t* bottomLeftPixel = renderBuffer.pixels;
	assert(*bottomLeftPixel == white);

	uint32_t bottomLeftPixelDataTest;
	assert(bitmap_interface_get_pixel_data(largeBitmap, 0, 0, bottomLeftPixelDataTest) == 0);
	assert(bottomLeftPixelDataTest == 1);

	assert(*(bottomLeftPixel + 1) == black);

	tl::ClearScreen(renderBuffer, red);
	tl::bitmap_interface_render(renderBuffer, largeBitmap, tl::Vec2<int>{ 6, 4 });

	assert(*bottomLeftPixel == red);

	uint32_t* sixAcrossFourUpFromBottomLeft = renderBuffer.pixels + 6 + (4 * renderBuffer.width);
	assert(*sixAcrossFourUpFromBottomLeft == white);
	assert(*(sixAcrossFourUpFromBottomLeft + 1) == black);
	assert(*(sixAcrossFourUpFromBottomLeft + 2) == white);
}

static void AssertMonochromeBitmapSide(uint32_t* bottomLeftPixel, int length, int step)
{
	bool expectBlack = true;
	for (int i = 0; i < length; i += step)
	{
		uint32_t expectedColor = (expectBlack) ? black : white;
		assert(*(bottomLeftPixel + i) == expectedColor);
		expectBlack = !expectBlack;
	}
}

static void RunSmallMonochromeBitmapTestForFile(char* filepath, int side)
{
	tl::bitmap monoBitmap;
	int fileReadResult = tl::file_interface_read(filepath, bitmapReadMemory);
	assert(fileReadResult == 0);

	int bitmapLoadResult = tl::bitmap_interface_initialize(monoBitmap, bitmapReadMemory);

	assert(bitmapLoadResult == 0);
	assert(monoBitmap.file_header.offsetToPixelDataInBytes == 62);

	assert(monoBitmap.color_table.size == 2);
	assert(monoBitmap.color_table.content[0] == black);
	assert(monoBitmap.color_table.content[1] == white);

	tl::ClearScreen(renderBuffer, red);
	tl::bitmap_interface_render(renderBuffer, monoBitmap, tl::Vec2<int>{ 0, 0 });

	uint32_t* bottomLeftPixel = renderBuffer.pixels;
	AssertMonochromeBitmapSide(bottomLeftPixel, side, 1);
	AssertMonochromeBitmapSide(bottomLeftPixel, side, side);
}

static void RunSmallMonochromeBitmapTests()
{
	MemorySpace test_bitmap_memory_space;
	test_bitmap_memory_space.content = &player_m_bmp;
	test_bitmap_memory_space.sizeInBytes = player_m_bmp_len;

	tl::bitmap monoBitmap;

	int bitmapLoadResult = tl::bitmap_interface_initialize(monoBitmap, test_bitmap_memory_space);

	assert(bitmapLoadResult == 0);

	tl::ClearScreen(renderBuffer, red);
	tl::bitmap_interface_render(renderBuffer, monoBitmap, tl::Vec2<int>{ 0, 0 });

	assert(monoBitmap.dibs_header.imageSizeInBytes == 64);

	uint32_t* bottomLeftPixel = renderBuffer.pixels;
	assert(*bottomLeftPixel == black);
	assert(*(bottomLeftPixel + monoBitmap.dibs_header.width) == black);

	uint32_t* fourthRowLeft = bottomLeftPixel + (monoBitmap.dibs_header.width * 4);
	assert(*fourthRowLeft == white);
	assert(*(fourthRowLeft + 1) == white);

	RunBitmapWriteTest(monoBitmap);

	RunSmallMonochromeBitmapTestForFile("../src/platform/9x9-1bit.bmp", 9);
	RunSmallMonochromeBitmapTestForFile("../src/platform/8x8-1bit.bmp", 8);
	RunSmallMonochromeBitmapTestForFile("../src/platform/7x7-1bit.bmp", 7);
	RunSmallMonochromeBitmapTestForFile("../src/platform/6x6-1bit.bmp", 6);
	RunSmallMonochromeBitmapTestForFile("../src/platform/5x5-1bit.bmp", 5);
	RunSmallMonochromeBitmapTestForFile("../src/platform/4x4-1bit.bmp", 4);
}

static void RunLargeBitmapTest()
{
	tl::bitmap largeBitmap;
	RunInitializeLargeBitmapTest(largeBitmap);
	RunLargeBitmapRenderTest(largeBitmap);
}

static void RunBitmapReinitializeTest()
{
	// TODO: maybe this is a sign of a bad interface... just return a new bitmap value and deal with the copy overhead.
	tl::bitmap bitmap;
	RunInitializeLargeBitmapTest(bitmap);
	RunInitializeSmallBitmapTest(bitmap);
}

void RunBitmapTests()
{
	InitializeMemory();

	RunSmallBitmapTest();

	RunLargeBitmapTest();

	RunSmallMonochromeBitmapTests();

	RunBitmapReinitializeTest();

	initialize_4_bit_bitmap_test_run();
}

