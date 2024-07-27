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
	if (!memory.content)
	{
		return bitmap_read_missing_memory_source;
	}

	// Need at least 6 bytes available to be able to read the file size
	const int minimumBytesNeededToReadFileSize = 6; 
	if (memory.sizeInBytes < minimumBytesNeededToReadFileSize )
	{
		return bitmap_read_invalid_memory_source;
	}

	uint8_t* bitmapDataAsBytes = (uint8_t*)memory.content;

	bitmap.file_header.fileType = read_uint16_t_from_little_endian(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(uint16_t);

	bitmap.file_header.fileSizeInBytes = read_int32_from_little_endian<int32_t>(bitmapDataAsBytes);
	bitmapDataAsBytes += sizeof(int32_t);

	if (memory.sizeInBytes < bitmap.file_header.fileSizeInBytes)
	{
		return bitmap_read_invalid_memory_source;
	}

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

typedef uint32_t GetColorFromBitmap(const bitmap& bitmap, int xOrdinal, int yOrdinal);

static uint32_t GetColorFrom24BitBitmap(const bitmap& bitmap, int xOrdinal, int yOrdinal)
{
	int pixelOffset = (yOrdinal * bitmap.dibs_header.width) + xOrdinal;

	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;
	RGB24Bit* bitmapPixel = twentyFourBitContent + pixelOffset;

	uint32_t colorRGB = bitmapPixel->r << 16 | bitmapPixel->g << 8 | bitmapPixel->b;

	return colorRGB;
}

static uint32_t GetColorFrom1BitBitmap(const bitmap& bitmap, int xOrdinal, int yOrdinal)
{
	uint8_t* eightBitContent = (uint8_t*)bitmap.content;
	int contentOffset = (yOrdinal * bitmap.dibs_header.width) + xOrdinal;
	const uint32_t white = 0xFFFFFF;
	const uint32_t black = 0x000000;

	const int byteSize = 8;
	int byteOffset = contentOffset / byteSize;
	int bitOffset = contentOffset % byteSize;
	int bitShiftOffset = byteSize - bitOffset - 1;

	uint8_t* byteFromBitmap = eightBitContent + byteOffset;

	// 1.shift the bit of interest over to the right most bit
	// 2. AND with a mask to evaluate the right most bit as true/false
	// 3. true --> white, false --> black
	uint32_t color = ((*byteFromBitmap  >> bitShiftOffset) & 0b00000001) ? white : black;
	return color;
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	const Vec2<int>& bottomLeftCornerPosition)
{
	if (bitmap.file_header.fileType == 0) return -1;

	GetColorFromBitmap* colorResolutionFunction = nullptr;
	switch (bitmap.dibs_header.bitsPerPixel)
	{
		case 24:
			colorResolutionFunction = &GetColorFrom24BitBitmap;
			break;
		case 1:
			colorResolutionFunction = &GetColorFrom1BitBitmap;
			break;
		default:
			return -1;
	}

	int bitmapEndX = bottomLeftCornerPosition.x + bitmap.dibs_header.width;
	int bitmapEndY = bottomLeftCornerPosition.y + bitmap.dibs_header.height;
	int endX = (bitmapEndX > buffer.width) ? buffer.width : bitmapEndX;
	int endY = (bitmapEndY > buffer.height) ? buffer.height : bitmapEndY;

	int bitmapY = 0;
	for (int j = bottomLeftCornerPosition.y; j < endY; j += 1)
	{
		int bitmapX = 0;
		for (int i = bottomLeftCornerPosition.x; i < endX; i += 1)
		{
			uint32_t pixelColor = (*colorResolutionFunction)(bitmap, bitmapX, bitmapY);
			PlotPixel(buffer, pixelColor, i, j);
			bitmapX += 1;
		}
		bitmapY += 1;
	}
	return 0;
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	const Rect<float>& footprint)
{
	Rect<float> pixelFootprint;
	pixelFootprint.halfSize = {
		footprint.halfSize.x / (float)buffer.width,
		footprint.halfSize.y / (float)buffer.height
	};
	pixelFootprint.position = {
		footprint.position.x - footprint.halfSize.x + pixelFootprint.halfSize.x,
		footprint.position.y - footprint.halfSize.y + pixelFootprint.halfSize.y
	};

	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;
	uint32_t colorRGBA = twentyFourBitContent->r << 16 | twentyFourBitContent->g << 8 | twentyFourBitContent->b;
	DrawRect(buffer, colorRGBA, pixelFootprint);

	return 0;
}

int bitmap_interface_write(
	const bitmap& bitmap,
	const MemorySpace& memory)
{
	uint64_t requiredSpaceInBytes = bitmap.file_header.fileSizeInBytes;
	uint64_t actualSpaceInBytes = memory.sizeInBytes;
	if (actualSpaceInBytes < requiredSpaceInBytes)
	{
		return bitmap_write_not_enough_space;
	}

	uint8_t* writeTargetAsBytes = (uint8_t*)memory.content;

	// write the file header
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.fileType, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.file_header.fileSizeInBytes, writeTargetAsBytes);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.reserved1, writeTargetAsBytes);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.reserved2, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.file_header.offsetToPixelDataInBytes, writeTargetAsBytes);

	// write the dibs header
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.headerSizeInBytes, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.width, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.height, writeTargetAsBytes);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.dibs_header.numberOfColorPlanes, writeTargetAsBytes);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.dibs_header.bitsPerPixel, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.compressionMethod, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.imageSizeInBytes, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.horizontalPixelsPerMeter, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.verticalPixelsPerMeter, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.numberOfColorsInPalette, writeTargetAsBytes);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.numberOfImportantColors, writeTargetAsBytes);

	// write the content
	uint8_t* readContent = bitmap.content;
	for (uint32_t i = 0; i < bitmap.dibs_header.imageSizeInBytes; i += 1)
	{
		*writeTargetAsBytes = *readContent;
		writeTargetAsBytes++;
		readContent++;
	}
	return bitmap_write_success;
}

}
