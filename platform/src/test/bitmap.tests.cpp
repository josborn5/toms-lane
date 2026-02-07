#include <assert.h>
#include "./generated-test-assets.hpp"
#include "../bitmap.hpp"

#include "../assert.hpp"

static const uint32_t grey = 0xAAAAAA;
static const uint32_t red = 0xFF0000;
static const uint32_t green = 0x00FF00;
static const uint32_t blue = 0x0000FF;
static const uint32_t white = 0xFFFFFF;
static const uint32_t black = 0x000000;

static const int smallBitmapFileSizeInBytes = 342;

static MemorySpace small_bmp_data;
static MemorySpace __4bit_bmp_data;
static MemorySpace monochrome_bmp_data;
static MemorySpace player_bmp_data;
static MemorySpace __9x9_bmp_data;
static MemorySpace __8x8_bmp_data;
static MemorySpace __7x7_bmp_data;
static MemorySpace __6x6_bmp_data;
static MemorySpace __5x5_bmp_data;
static MemorySpace __4x4_bmp_data;

static void initialize_data_for_test_bitmap(
	const unsigned char* data,
	const unsigned int size,
	MemorySpace& memory
) {
	memory.content = (void*)& data[0];
	memory.sizeInBytes = sizeof(unsigned int) * size;
}


static MemorySpace bitmapReadMemory;
static MemorySpace bitmapWriteMemory;
static MemorySpace renderBufferPixels;
static RenderBuffer renderBuffer;

static void InitializeMemory()
{
	initialize_data_for_test_bitmap(test_bmp, test_bmp_len, small_bmp_data);
	initialize_data_for_test_bitmap(bitmap_4bit_bmp, bitmap_4bit_bmp_len, __4bit_bmp_data);
	initialize_data_for_test_bitmap(monochrome_bmp, monochrome_bmp_len, monochrome_bmp_data);
	initialize_data_for_test_bitmap(player_m_bmp, player_m_bmp_len, player_bmp_data);
	initialize_data_for_test_bitmap(__9x9_1bit_bmp, __9x9_1bit_bmp_len, __9x9_bmp_data);
	initialize_data_for_test_bitmap(__8x8_1bit_bmp, __8x8_1bit_bmp_len, __8x8_bmp_data);
	initialize_data_for_test_bitmap(__7x7_1bit_bmp, __7x7_1bit_bmp_len, __7x7_bmp_data);
	initialize_data_for_test_bitmap(__6x6_1bit_bmp, __6x6_1bit_bmp_len, __6x6_bmp_data);
	initialize_data_for_test_bitmap(__5x5_1bit_bmp, __5x5_1bit_bmp_len, __5x5_bmp_data);
	initialize_data_for_test_bitmap(__4x4_1bit_bmp, __4x4_1bit_bmp_len, __4x4_bmp_data);

	bitmapReadMemory.sizeInBytes = 1024 * 60;
	bitmapReadMemory.content = malloc(bitmapReadMemory.sizeInBytes);
	bitmapWriteMemory.sizeInBytes = 1024 * 60;
	bitmapWriteMemory.content = malloc(bitmapWriteMemory.sizeInBytes);

	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;

	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferPixels.content = malloc(renderBufferPixels.sizeInBytes);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;
}

static void RunInitializeSmallBitmapTest(const MemorySpace& bitmap_data, tl::bitmap& testBitmap)
{
	tl::bitmap_interface_initialize(testBitmap, bitmap_data);

	assert_uint16_t(testBitmap.file_header.fileType, 0x4d42, "bitmap header file type");
	assert_int32_t(testBitmap.file_header.fileSizeInBytes, smallBitmapFileSizeInBytes, "bitmap header file size");
	assert_uint16_t(testBitmap.file_header.reserved1, 0, "bitmap header reserved 1");
	assert_uint16_t(testBitmap.file_header.reserved2, 0, "bitmap header reserved 2");
	assert_int32_t(testBitmap.file_header.offsetToPixelDataInBytes, 54, "bitmap header offset to pixel data");

	assert_uint32_t(testBitmap.dibs_header.headerSizeInBytes, 40, "bitmap dibs header size");

	assert_int32_t(testBitmap.dibs_header.width, 12, "bitmap dibs header width");
	assert_int32_t(testBitmap.dibs_header.height, 8, "bitmap dibs header height");

	assert_uint16_t(testBitmap.dibs_header.numberOfColorPlanes, 1, "bitmap dibs header color plane count");
	assert_uint16_t(testBitmap.dibs_header.bitsPerPixel, 24, "bitmap dibs header bits per pixel");
	assert_uint32_t(testBitmap.dibs_header.compressionMethod, 0, "bitmap dibs header compression method");
	assert_uint32_t(testBitmap.dibs_header.imageSizeInBytes, 288, "bitmap dibs header image size");

	assert_int32_t(testBitmap.dibs_header.horizontalPixelsPerMeter, 0, "bitmap dibs header horizontal pixels per meter");
	assert_int32_t(testBitmap.dibs_header.verticalPixelsPerMeter, 0, "bitmap dibs header vertical pixels per meter");
	assert_uint32_t(testBitmap.dibs_header.numberOfColorsInPalette, 0, "bitmap dibs header color palette size");
	assert_uint32_t(testBitmap.dibs_header.numberOfImportantColors, 0, "bitmap dibs header important colors");

	assert_int32_t(testBitmap.color_table.size, 0, "bitmap color table size");
}

static void RunBitmapWriteTest(const bitmap& bitmap, const MemorySpace& reference_memory)
{
	int writeResult = tl::bitmap_interface_write(bitmap, bitmapWriteMemory);
	assert(writeResult == tl::bitmap_write_success);

	uint8_t* readMemory = (uint8_t*)reference_memory.content;
	uint8_t* writeMemory = (uint8_t*)bitmapWriteMemory.content;

	const int fileHeaderSizeInBytes = 14;
	int minPaddingIndex = fileHeaderSizeInBytes + bitmap.dibs_header.headerSizeInBytes + (bitmap.color_table.size * 4);
	int maxPaddingIndex = bitmap.file_header.offsetToPixelDataInBytes;
	for (int i = 0; i < bitmap.file_header.fileSizeInBytes; i += 1)
	{
		if (i < minPaddingIndex || i > maxPaddingIndex) // don't care about padding memory content
		{
			assert_uint8_t(*readMemory, *writeMemory, "bitmap written byte");
		}
		readMemory++;
		writeMemory++;
	}
}

static void initialize_4_bit_bitmap_test_run() {
	tl::bitmap test_bitmap;
	tl::bitmap_interface_initialize(test_bitmap, __4bit_bmp_data);

	assert_uint16_t(test_bitmap.file_header.fileType, 0x4d42, "bitmap header file type");
	assert_int32_t(test_bitmap.file_header.offsetToPixelDataInBytes, 118, "bitmap header offset to pixel data");

	assert_uint32_t(test_bitmap.dibs_header.headerSizeInBytes, 40, "bitmap header size in bytes");
	assert_uint16_t(test_bitmap.dibs_header.bitsPerPixel, 4, "bitmap header bits per pixel");

	assert_int32_t(test_bitmap.color_table.size, 16, "bitmap color table size");

	tl::ClearScreen(renderBuffer, grey);
	tl::bitmap_interface_render(renderBuffer, test_bitmap, tl::Vec2<int>{ 0, 0 });

	uint32_t* bottom_left_color = renderBuffer.pixels;
	assert_uint32_t(*bottom_left_color, 0x000000, "bitmap rendered pixel color");

	uint32_t test_pixel_data = 0xFFFFFF;
	bitmap_interface_get_pixel_data(test_bitmap, 0, 0, test_pixel_data);
	assert_uint32_t(test_pixel_data, 0, "bitmap read pixel color");

	bitmap_interface_get_pixel_data(test_bitmap, 0, 0, test_pixel_data);
	assert_uint32_t(test_pixel_data, 0, "bitmap read pixel color - first pixel");
	bitmap_interface_get_pixel_data(test_bitmap, 1, 0, test_pixel_data);
	assert_uint32_t(test_pixel_data, 15, "bitmap read pixel color - last pixel");
	bitmap_interface_get_pixel_data(test_bitmap, 2, 0, test_pixel_data);
	assert_uint32_t(test_pixel_data, 9, "bitmap read pixel color");
	bitmap_interface_get_pixel_data(test_bitmap, 3, 0, test_pixel_data);
	assert_uint32_t(test_pixel_data, 10, "bitmap read pixel color");
	bitmap_interface_get_pixel_data(test_bitmap, 4, 0, test_pixel_data);
	assert_uint32_t(test_pixel_data, 12, "bitmap read pixel color");

	assert_uint32_t(test_bitmap.color_table.content[0], 0x000000, "bitmap color table color");
	assert_uint32_t(test_bitmap.color_table.content[15], 0xFFFFFF, "bitmap color table color");
	assert_uint32_t(test_bitmap.color_table.content[9], 0xFF0000, "bitmap color table color");
	assert_uint32_t(test_bitmap.color_table.content[10], 0x00FF00, "bitmap color table color");
	assert_uint32_t(test_bitmap.color_table.content[12], 0x0000FF, "bitmap color table color");

	RunBitmapWriteTest(test_bitmap, __4bit_bmp_data);
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

	assert_uint32_t(*bottomLeftPixel, green, "bitmap rendered pixel color");
	uint32_t bottomLeftPixelColorTest;
	assert_uint32_t(bitmap_interface_get_pixel_data(testBitmap, 0, 0, bottomLeftPixelColorTest), 0, "bitmap read color operation");
	assert_uint32_t(bottomLeftPixelColorTest, green, "bitmap read color");

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
	MemorySpace badMemory = {0};


	printf("!!! small bitmap file size %d\n", bitmap.file_header.fileSizeInBytes);

	int readResult = tl::bitmap_interface_initialize(bitmap, badMemory);


	printf("!!! small bitmap file size %d\n", bitmap.file_header.fileSizeInBytes);

	assert_int(readResult, tl::bitmap_read_missing_memory_source, "bitmap initialize fails with uninitialized memory");

	// source memory is not big enough to read the file size
	badMemory.content = bitmapReadMemory.content;
	badMemory.sizeInBytes = 5;
	readResult = tl::bitmap_interface_initialize(bitmap, badMemory);
	assert_int(readResult, tl::bitmap_read_invalid_memory_source, "bitmap initialize fails with memory less than bitmap header size");

	// source memory is blank
	badMemory.content = bitmapReadMemory.content;
	badMemory.sizeInBytes = 5;

	// source memory is big enough to read the file size but is smaller that the read file size
	badMemory.sizeInBytes = smallBitmapFileSizeInBytes - 1;
	readResult = tl::bitmap_interface_initialize(bitmap, badMemory);
	assert_int(readResult, tl::bitmap_read_invalid_memory_source, "bitmap initialize fails with memory less than bitmap file size");
}

static void RunSmallBitmapTest()
{
	tl::bitmap smallBitmap;
	RunInitializeSmallBitmapTest(small_bmp_data, smallBitmap);
	RunSmallBitmapRenderTest(smallBitmap);
	RunBitmapWriteTest(smallBitmap, small_bmp_data);
	RunBitmapWriteToSmallMemoryTest(smallBitmap);

	printf("!!! small bitmap file size %d\n", smallBitmap.file_header.fileSizeInBytes);

	RunBitmapReadFromBadMemoryTests(smallBitmap);
}

static void RunInitializeLargeBitmapTest(tl::bitmap& largeBitmap)
{
	tl::bitmap_interface_initialize(largeBitmap, monochrome_bmp_data);
	assert(largeBitmap.file_header.fileType == 0x4d42);
	assert_int32_t(largeBitmap.file_header.fileSizeInBytes, 60062, "bitmap header file size");
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

static void run_square_monochrome_bitmap_test(tl::MemorySpace& bitmap_memory_space, int side)
{
	tl::bitmap monoBitmap;

	int bitmapLoadResult = tl::bitmap_interface_initialize(monoBitmap, bitmap_memory_space);

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
	tl::bitmap monoBitmap;
	int bitmapLoadResult = tl::bitmap_interface_initialize(monoBitmap, player_bmp_data);

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

	RunBitmapWriteTest(monoBitmap, player_bmp_data);

	run_square_monochrome_bitmap_test(__9x9_bmp_data, 9);
	run_square_monochrome_bitmap_test(__8x8_bmp_data, 8);
	run_square_monochrome_bitmap_test(__7x7_bmp_data, 7);
	run_square_monochrome_bitmap_test(__6x6_bmp_data, 6);
	run_square_monochrome_bitmap_test(__5x5_bmp_data, 5);
	run_square_monochrome_bitmap_test(__4x4_bmp_data, 4);
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
	RunInitializeSmallBitmapTest(small_bmp_data, bitmap);
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

