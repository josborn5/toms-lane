#include "./bitmap.hpp"

namespace tl
{

struct RGB24Bit
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

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

	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;

	for (int j = 0; j < bitmap.dibs_header->height; j += 1)
	{
		for (int i = 0; i < bitmap.dibs_header->width; i += 1)
		{
			uint32_t colorRGBA = twentyFourBitContent->r << 16 | twentyFourBitContent->g << 8 | twentyFourBitContent->b;
			PlotPixel(buffer, colorRGBA, i, j);
			twentyFourBitContent++;
		}
	}
	return 0;
}

}
