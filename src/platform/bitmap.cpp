#include "./bitmap.hpp"

namespace tl
{

int bitmap_interface_initialize(bitmap& bitmap, const MemorySpace& memory)
{
	bitmap.header = (bitmap_header*)memory.content;
	size_t headerOffset = bitmap.header->headerSizeInBytes;
	bitmap.content = bitmap.header + headerOffset;
	return 0;
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	Vec2<int> topLeftCornerPosition)
{
	return 0;
}

}
