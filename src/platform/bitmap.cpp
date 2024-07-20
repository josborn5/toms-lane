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
static T read_int32_from_little_endian(uint8_t* data)
{
	const int bufferSize = 4;
	uint8_t charBuffer[bufferSize];
	for (int i = 0; i < bufferSize; i += 1)
	{
		charBuffer[i] = *data;
		data++;
	}
	T intValue = charBuffer[3] << 24 | charBuffer[2] << 16 | charBuffer[1] << 8 | charBuffer[0];
	return intValue;
}

template<typename T>
static uint8_t* write_four_byte_value_to_little_endian(T four_byte_value, uint8_t* data)
{
	const int bufferSize = 4;
	uint8_t charBuffer[bufferSize];
	charBuffer[0] = (uint8_t)(four_byte_value & 0x000000FF);
	charBuffer[1] = (uint8_t)((four_byte_value & 0x0000FF00) >> 8);
	charBuffer[2] = (uint8_t)((four_byte_value & 0x00FF0000) >> 16);
	charBuffer[3] = (uint8_t)((four_byte_value & 0xFF000000) >> 24);

	for (int i = 0; i < bufferSize; i += 1)
	{
		*data = charBuffer[i];
		data++;
	}

	return data;
}

static uint16_t read_uint16_t_from_little_endian(uint8_t* data)
{
	uint8_t charBuffer[2];
	charBuffer[0] = *data;
	data++;
	charBuffer[1] = *data;

	uint16_t intValue = charBuffer[1] << 8 | charBuffer[0];
	return intValue;
}

static uint8_t* write_uint16_t_value_to_little_endian(uint16_t two_byte_value, uint8_t* data)
{
	const int bufferSize = 2;
	uint8_t charBuffer[bufferSize];
	charBuffer[0] = (uint8_t)(two_byte_value & 0x00FF);
	charBuffer[1] = (uint8_t)((two_byte_value & 0xFF00) >> 8);

	for (int i = 0; i < bufferSize; i += 1)
	{
		*data = charBuffer[i];
		data++;
	}

	return data;
}


int bitmap_interface_initialize(bitmap& bitmap, const MemorySpace& memory)
{
	uint8_t* bitmapDataAsBytes = (uint8_t*)memory.content;

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
	size_t pixelDataOffsetInBytes = bitmap.file_header.offsetToPixelDataInBytes;
	bitmap.content = (unsigned char*)memory.content + pixelDataOffsetInBytes;

	bitmap.dibs_header.headerSizeInBytes = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap.dibs_header.width = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap.dibs_header.height = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap.dibs_header.numberOfColorPlanes = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap.dibs_header.bitsPerPixel = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap.dibs_header.compressionMethod = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap.dibs_header.imageSizeInBytes = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap.dibs_header.horizontalPixelsPerMeter = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap.dibs_header.verticalPixelsPerMeter = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	bitmap.dibs_header.numberOfColorsInPalette = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint32_t);

	bitmap.dibs_header.numberOfImportantColors = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);

	return 0;
}

static void FillBitmapContentFor24Bits(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	Vec2<int> bottomLeftCornerPosition
)
{
	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;

	for (int j = bottomLeftCornerPosition.y; j < bitmap.dibs_header.height; j += 1)
	{
		for (int i = bottomLeftCornerPosition.x; i < bitmap.dibs_header.width; i += 1)
		{
			uint32_t colorRGBA = twentyFourBitContent->r << 16 | twentyFourBitContent->g << 8 | twentyFourBitContent->b;
			PlotPixel(buffer, colorRGBA, i, j);
			twentyFourBitContent++;
		}
	}
}

static void FillBitmapContentFor1Bits(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	Vec2<int> bottomLeftCornerPosition
)
{
	uint8_t* eightBitContent = (uint8_t*)bitmap.content;
	const uint32_t white = 0xFFFFFF;
	const uint32_t black = 0x000000;

	const int byteSize = 8;
	int x = bottomLeftCornerPosition.x;
	int y = bottomLeftCornerPosition.y;
	int rowCounter = 0;
	for (uint32_t byteIndex = 0; byteIndex < bitmap.dibs_header.imageSizeInBytes; byteIndex += 1)
	{
		uint8_t value = *eightBitContent;
		for (int bitIndex = 0; bitIndex < byteSize; bitIndex += 1)
		{
			int bitOffset = byteSize - bitIndex - 1;
			// 1.shift the bit of interest over to the right most bit
			// 2. AND with a mask to evaluate the right most bit as true/false
			// 3. true --> white, false --> black
			uint32_t color = ((value >> bitOffset) & 0b00000001) ? white : black;
			PlotPixel(buffer, color, x, y);
			rowCounter += 1;
			if (rowCounter == bitmap.dibs_header.width)
			{
				rowCounter = 0;
				x = 0;
				y += 1;
			}
			else
			{
				x += 1;
			}
		}
		eightBitContent++;
	}
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	Vec2<int> bottomLeftCornerPosition)
{
	if (bitmap.file_header.fileType == 0) return -1;

	switch (bitmap.dibs_header.bitsPerPixel)
	{
		case 24:
			FillBitmapContentFor24Bits(buffer, bitmap, bottomLeftCornerPosition);
			break;
		case 1:
			FillBitmapContentFor1Bits(buffer, bitmap, bottomLeftCornerPosition);
			break;
	}

	return 0;
}

int bitmap_interface_write(
	const bitmap& bitmap,
	const MemorySpace& memory)
{
	// TODO: compare memory and bitmap size to check there is space
	uint8_t* writeTargetAsBytes = (uint8_t*)memory.content;

	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.fileType, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.file_header.fileSizeInBytes, writeTargetAsBytes);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.reserved1, writeTargetAsBytes);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.reserved2, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.file_header.offsetToPixelDataInBytes, writeTargetAsBytes);
	return 0;
}

}
