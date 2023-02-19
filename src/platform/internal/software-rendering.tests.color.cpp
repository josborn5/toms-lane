#include <assert.h>

void RunColorTests()
{
	assert(0xFF0000 == tl::GetColorFromRGB(255, 0, 0));
	assert(0x00FF00 == tl::GetColorFromRGB(0, 255, 0));
	assert(0x0000FF == tl::GetColorFromRGB(0, 0, 255));
	assert(0x000000 == tl::GetColorFromRGB(0, 0, 0));
}