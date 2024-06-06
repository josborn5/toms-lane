#include "./bitmap.hpp"

namespace tl
{

struct RGB24Bit
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

template<typename T>
static T read_int32_from_little_endian(char* data)
{
	const int bufferSize = 4;
	char charBuffer[bufferSize];
	for (int i = 0; i < bufferSize; i += 1)
	{
		charBuffer[i] = *data;
		data++;
	}
	T intValue = charBuffer[3] << 24 | charBuffer[2] << 16 | charBuffer[1] << 8 | charBuffer[0];
	return intValue;
}

static uint16_t read_uint16_t_from_little_endian(char* data)
{
	char charBuffer[2];
	charBuffer[0] = *data;
	data++;
	charBuffer[1] = *data;

	uint16_t intValue = charBuffer[1] << 8 | charBuffer[0];
	return intValue;
}

int bitmap_interface_initialize(bitmap& bitmap, const MemorySpace& memory)
{
	char* bitmapDataAsBytes = (char*)memory.content;

	bitmap.file_header.fileType = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap.file_header.fileSizeInBytes = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap.file_header.reserved1 = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap.file_header.reserved2 = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap.file_header.offsetToPixelDataInBytes = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	// Cast to a char pointer to do pointer arithmetic in bytes
	bitmap.dibs_header = (bitmap_dibs_header*)bitmapDataAsBytes;
	size_t pixelDataOffsetInBytes = bitmap.file_header.offsetToPixelDataInBytes;
	bitmap.content = (char*)memory.content + pixelDataOffsetInBytes;

	bitmap._dibs_header.headerSizeInBytes = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap._dibs_header.width = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap._dibs_header.height = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap._dibs_header.numberOfColorPlanes = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap._dibs_header.bitsPerPixel = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap._dibs_header.compressionMethod = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap._dibs_header.imageSizeInBytes = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap._dibs_header.horizontalPixelsPerMeter = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap._dibs_header.verticalPixelsPerMeter = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap._dibs_header.numberOfColorsInPalette = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap._dibs_header.numberOfImportantColors = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);

	return 0;
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	Vec2<int> bottomLeftCornerPosition)
{
	if (bitmap.file_header.fileType == 0) return -1;

	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;

	for (int j = bottomLeftCornerPosition.y; j < bitmap.dibs_header->height; j += 1)
	{
		for (int i = bottomLeftCornerPosition.x; i < bitmap.dibs_header->width; i += 1)
		{
			uint32_t colorRGBA = twentyFourBitContent->r << 16 | twentyFourBitContent->g << 8 | twentyFourBitContent->b;
			PlotPixel(buffer, colorRGBA, i, j);
			twentyFourBitContent++;
		}
	}
	return 0;
}

}
