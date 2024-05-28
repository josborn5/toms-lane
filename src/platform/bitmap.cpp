#include "./bitmap.hpp"

namespace tl
{

int bitmap_interface_initialize(bitmap& bitmap, const MemorySpace& memory)
{
	bitmap.file_header = (bitmap_file_header*)memory.content;
	size_t dibsHeaderOffsetInBytes = sizeof(bitmap_file_header);
	// Cast to a char pointer to do pointer arithmetic in bytes
	bitmap.dibs_header = (bitmap_dibs_header*)((char*)memory.content + dibsHeaderOffsetInBytes);
	size_t pixelDataOffsetInBytes = bitmap.file_header->offsetToPixelDataInBytes;
	bitmap.content = (char*)memory.content + pixelDataOffsetInBytes;
	return 0;
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	Vec2<int> topLeftCornerPosition)
{
	if (bitmap.file_header == nullptr) return -1;

	return 0;
}

}
