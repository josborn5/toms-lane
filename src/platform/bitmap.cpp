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
static uint8_t* write_four_byte_value_to_little_endian(T four_byte_value, uint8_t* data, int32_t& byteCounter)
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
		byteCounter += 1;
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

static uint8_t* write_uint16_t_value_to_little_endian(uint16_t two_byte_value, uint8_t* data, int32_t& byteCounter)
{
	const int bufferSize = 2;
	uint8_t charBuffer[bufferSize];
	charBuffer[0] = (uint8_t)(two_byte_value & 0x00FF);
	charBuffer[1] = (uint8_t)((two_byte_value & 0xFF00) >> 8);

	for (int i = 0; i < bufferSize; i += 1)
	{
		*data = charBuffer[i];
		data++;
		byteCounter += 1;
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
	bitmapDataAsBytes += sizeof(uint32_t);

	// Check for a color table
	int32_t file_and_dibs_header_size_in_bytes = 14 + bitmap.dibs_header.headerSizeInBytes; // 14 bytes for the file header
	int32_t color_table_size_in_bytes = bitmap.file_header.offsetToPixelDataInBytes - file_and_dibs_header_size_in_bytes;
	if (bitmap.dibs_header.bitsPerPixel == 1 && color_table_size_in_bytes >= (2 * sizeof(uint32_t))) {
		bitmap.color_table.size = 2;
	} else if (bitmap.dibs_header.bitsPerPixel == 4 && color_table_size_in_bytes >= (16 * sizeof(uint32_t))) {
		bitmap.color_table.size = 16;
	} else if (bitmap.dibs_header.bitsPerPixel == 2 && color_table_size_in_bytes >= (4 * sizeof(uint32_t))) {
		bitmap.color_table.size = 4;
	} else {
		bitmap.color_table.size = 0;
	}

	for (int i = 0; i < bitmap.color_table.size; i += 1)
	{
		bitmap.color_table.content[i] = read_int32_from_little_endian<uint32_t>(bitmapDataAsBytes);
		bitmapDataAsBytes += sizeof(uint32_t);
	}
	return 0;
}

typedef uint32_t GetColorFromBitmap(const bitmap& bitmap, int bitmapX, int bitmapY);
typedef uint32_t get_pixel_data_from_bitmap(const bitmap& bitmap, int bitmapX, int bitmapY);

static uint32_t get_pixel_data_24_bit(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	int pixelOffset = (bitmapY * bitmap.dibs_header.width) + bitmapX;
	RGB24Bit* twentyFourBitContent = (RGB24Bit*)bitmap.content;
	RGB24Bit* bitmapPixel = twentyFourBitContent + pixelOffset;

	uint32_t colorRGB = bitmapPixel->r << 16 | bitmapPixel->g << 8 | bitmapPixel->b;
	return colorRGB;
}

static uint32_t get_pixel_data_8_bit(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	// rows have a byte size that is a multiple of 4 bytes (32 bits) !!!
	const int bits_per_byte = 8;
	const int bits_per_pixel = 8;
	constexpr int pixels_per_byte = bits_per_byte / bits_per_pixel;
	int bytes_per_row = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int content_offset_in_bytes = (bitmapY * bytes_per_row) + (bitmapX / pixels_per_byte);

	uint8_t* eight_bit_content = (uint8_t*)bitmap.content;
	uint8_t* byte_from_bitmap = eight_bit_content + content_offset_in_bytes;

	return (uint32_t)(*byte_from_bitmap);
}

static uint32_t get_pixel_data_4_bit(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	// rows have a byte size that is a multiple of 4 bytes (32 bits) !!!
	const int bits_per_byte = 8;
	const int bits_per_pixel = 4;
	constexpr int pixels_per_byte = bits_per_byte / bits_per_pixel;
	int bytes_per_row = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int content_offset_in_bytes = (bitmapY * bytes_per_row) + (bitmapX / pixels_per_byte);

	uint8_t* eight_bit_content = (uint8_t*)bitmap.content;

	int pixel_index_in_byte = (bitmapX % pixels_per_byte);
	int bit_shift_count = bits_per_byte - ((pixel_index_in_byte + 1) * bits_per_pixel);

	uint8_t* byte_from_bitmap = eight_bit_content + content_offset_in_bytes;

	uint32_t pixel_data = (uint32_t)((*byte_from_bitmap >> bit_shift_count) & 0b00001111);
	return pixel_data;
}

static uint32_t get_pixel_data_2_bit(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	// rows have a byte size that is a multiple of 4 bytes (32 bits) !!!
	const int bits_per_byte = 8;
	const int bits_per_pixel = 2;
	constexpr int pixels_per_byte = bits_per_byte / bits_per_pixel;
	int bytes_per_row = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int content_offset_in_bytes = (bitmapY * bytes_per_row) + (bitmapX / pixels_per_byte);

	uint8_t* eight_bit_content = (uint8_t*)bitmap.content;

	int pixel_index_in_byte = (bitmapX % pixels_per_byte);
	int bit_shift_count = bits_per_byte - ((pixel_index_in_byte + 1) * bits_per_pixel);

	uint8_t* byte_from_bitmap = eight_bit_content + content_offset_in_bytes;

	uint32_t pixel_data = (uint32_t)((*byte_from_bitmap >> bit_shift_count) & 0b00000011);
	return pixel_data;
}

static uint32_t get_pixel_data_1_bit(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	// rows have a byte size that is a multiple of 4 bytes (32 bits) !!!
	const int bitsPerByte = 8;
	int bytesPerRow = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int contentOffsetInBytes = (bitmapY * bytesPerRow) + (bitmapX / bitsPerByte);

	uint8_t* eightBitContent = (uint8_t*)bitmap.content;

	int bitOffset = bitmapX % bitsPerByte;
	int bitShiftOffset = bitsPerByte - bitOffset - 1;

	uint8_t* byteFromBitmap = eightBitContent + contentOffsetInBytes;

	// 1.shift the bit of interest over to the right most bit
	// 2. AND with a mask to evaluate the right most bit as true/false
	// 3. true --> white, false --> black
	uint32_t pixelDataAsInt = ((*byteFromBitmap  >> bitShiftOffset) & 0b00000001) ? 1 : 0;
	return pixelDataAsInt;
}

static uint32_t GetColorFrom1BitBitmap(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	int pixel_index = get_pixel_data_1_bit(bitmap, bitmapX, bitmapY);
	return bitmap.color_table.content[pixel_index];
}

static uint32_t get_color_from_4_bit_bitmap(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	int pixel_index = get_pixel_data_4_bit(bitmap, bitmapX, bitmapY);
	return bitmap.color_table.content[pixel_index];
}

static uint32_t get_color_from_2_bit_bitmap(const bitmap& bitmap, int bitmapX, int bitmapY)
{
	int pixel_index = get_pixel_data_2_bit(bitmap, bitmapX, bitmapY);
	return bitmap.color_table.content[pixel_index];
}

static GetColorFromBitmap* resolve_color_resolution_function(const bitmap_dibs_header& dibs_header)
{
	switch (dibs_header.bitsPerPixel)
	{
		case 24:
			return &get_pixel_data_24_bit;
		case 4:
			return &get_color_from_4_bit_bitmap;
		case 2:
			return &get_color_from_2_bit_bitmap;
		case 1:
			return &GetColorFrom1BitBitmap;
		default:
			return nullptr;
	}
}

static void resolve_bitmap_render_bounds(
	const RenderBuffer& buffer,
	const Vec2<int>& renderStart,
	const Vec2<int>& renderEnd,
	Vec2<int>& resolvedStart,
	Vec2<int>& resolvedEnd
)
{
	resolvedStart.x = (renderStart.x < 0) ? 0 : renderStart.x;
	resolvedStart.y = (renderStart.y < 0) ? 0 : renderStart.y;
	resolvedEnd.x = (renderEnd.x > buffer.width) ? buffer.width : renderEnd.x;
	resolvedEnd.y = (renderEnd.y > buffer.height) ? buffer.height : renderEnd.y;
}

int bitmap_interface_get_pixel_data(
	const bitmap& bitmap,
	int x,
	int y,
	uint32_t& pixel_data)
{
	get_pixel_data_from_bitmap* data_function = nullptr;
	switch (bitmap.dibs_header.bitsPerPixel)
	{
		case 24:
			data_function = &get_pixel_data_24_bit;
			break;
		case 8:
			data_function = &get_pixel_data_8_bit;
			break;
		case 4:
			data_function = &get_pixel_data_4_bit;
			break;
		case 2:
			data_function = &get_pixel_data_2_bit;
			break;
		case 1:
			data_function = &get_pixel_data_1_bit;
			break;
		default:
			return -1;
	}

	pixel_data = data_function(bitmap, x, y);
	return 0;
}

int bitmap_interface_render(
	const RenderBuffer& buffer,
	const bitmap& bitmap,
	const Vec2<int>& bottomLeftCornerPosition)
{
	if (bitmap.file_header.fileType == 0) return -1;

	GetColorFromBitmap* colorResolutionFunction = resolve_color_resolution_function(bitmap.dibs_header);
	if (colorResolutionFunction == nullptr) return -2;

	Vec2<int> bitmapEnd = {
		bottomLeftCornerPosition.x + bitmap.dibs_header.width,
		bottomLeftCornerPosition.y + bitmap.dibs_header.height
	};
	Vec2<int> start, end;
	resolve_bitmap_render_bounds(buffer, bottomLeftCornerPosition, bitmapEnd, start, end);

	int bitmapY = 0;
	for (int j = start.y; j < end.y; j += 1)
	{
		uint32_t* pixel = buffer.pixels + (buffer.width * j) + start.x;
		int bitmapX = 0;
		for (int i = start.x; i < end.x; i += 1)
		{
			uint32_t pixelColor = (*colorResolutionFunction)(bitmap, bitmapX, bitmapY);
			*pixel = pixelColor;
			pixel++;
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
	if (bitmap.file_header.fileType == 0) return -1;

	GetColorFromBitmap* colorResolutionFunction = resolve_color_resolution_function(bitmap.dibs_header);
	if (colorResolutionFunction == nullptr) return -2;

	Vec2<float> bitmapIncrement = {
		0.5f * (float)bitmap.dibs_header.width / footprint.halfSize.x,
		0.5f * (float)bitmap.dibs_header.height / footprint.halfSize.y
	};
	Vec2<int> bitmapStart = {
		(int)footprint.x_min(),
		(int)footprint.y_min()
	};
	Vec2<int> bitmapEnd = {
		(int)footprint.x_max(),
		(int)footprint.y_max()
	};
	Vec2<int> start, end;
	resolve_bitmap_render_bounds(buffer, bitmapStart, bitmapEnd, start, end);

	float bitmapY = 0.0f;
	for (int j = start.y; j < end.y; j += 1)
	{
		uint32_t* pixel = buffer.pixels + (buffer.width * j) + start.x;
		float bitmapX = 0.0f;
		for (int i = start.x; i < end.x; i += 1)
		{
			uint32_t pixelColor = (*colorResolutionFunction)(bitmap, (int)bitmapX, (int)bitmapY);
			*pixel = pixelColor;
			pixel++;
			bitmapX += bitmapIncrement.x;
		}
		bitmapY += bitmapIncrement.y;
	}
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


	int32_t byteCounter = 0;
	// write the file header
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.fileType, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.file_header.fileSizeInBytes, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.reserved1, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.file_header.reserved2, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.file_header.offsetToPixelDataInBytes, writeTargetAsBytes, byteCounter);

	// write the dibs header
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.headerSizeInBytes, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.width, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.height, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.dibs_header.numberOfColorPlanes, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_uint16_t_value_to_little_endian(bitmap.dibs_header.bitsPerPixel, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.compressionMethod, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.imageSizeInBytes, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.horizontalPixelsPerMeter, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<int32_t>(bitmap.dibs_header.verticalPixelsPerMeter, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.numberOfColorsInPalette, writeTargetAsBytes, byteCounter);
	writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.dibs_header.numberOfImportantColors, writeTargetAsBytes, byteCounter);

	// color table
	for (int i = 0; i < bitmap.color_table.size; i += 1)
	{
		writeTargetAsBytes = write_four_byte_value_to_little_endian<uint32_t>(bitmap.color_table.content[i], writeTargetAsBytes, byteCounter);
	}

	// fill up any remaining space before the pixel data
	while (byteCounter < bitmap.file_header.offsetToPixelDataInBytes)
	{
		*writeTargetAsBytes = (uint8_t)0;
		writeTargetAsBytes++;
		byteCounter += 1;
	}

	// write the content
	uint8_t* readContent = bitmap.content;
	for (uint32_t i = 0; i < bitmap.dibs_header.imageSizeInBytes; i += 1)
	{
		*writeTargetAsBytes = *readContent;
		writeTargetAsBytes++;
		readContent++;
		byteCounter += 1;
	}
	return bitmap_write_success;
}

}
