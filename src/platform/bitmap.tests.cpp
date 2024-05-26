#include <assert.h>
#include "./bitmap.hpp"

void RunBitmapTests()
{
	MemorySpace memory;
	bitmap testBitmap;
	memory.sizeInBytes = 1024;
	tl::memory_interface_initialize(memory);
	tl::file_interface_read("../src/platform/test.bmp", memory);
	tl::bitmap_interface_initialize(testBitmap, memory);

//	assert(testBitmap.header->width == 12);
}

