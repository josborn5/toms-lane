#include "../../platform/src/tl-library.hpp"
#include "./editor.hpp"

struct RGB24Bit
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

typedef void ColorToBitmap (uint32_t spriteColor, int bitmapX, int bitmapY, tl::bitmap& bitmap);

static void WriteColorTo24BitBitmap(uint32_t spriteColor, int bitmapX, int bitmapY, tl::bitmap& bitmap)
{
	int pixelOffset = (bitmapY * bitmap.dibs_header.width) + bitmapX;
	RGB24Bit bitmapPixel;
	bitmapPixel.r = (uint8_t)(spriteColor >> 16 & 0x0000FF);
	bitmapPixel.g = (uint8_t)(spriteColor >> 8 & 0x0000FF);
	bitmapPixel.b = (uint8_t)(spriteColor & 0x0000FF);
	*((RGB24Bit*)bitmap.content + pixelOffset) = bitmapPixel;
}

static void write_color_to_8_bit_bitmap(uint32_t pixel_data, int bitmapX, int bitmapY, tl::bitmap& bitmap)
{
	const int bits_per_byte = 8;
	const int bits_per_pixel = 8;
	constexpr int pixels_per_byte = bits_per_byte / bits_per_pixel;
	int bytes_per_row = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int content_offset_in_bytes = (bitmapY * bytes_per_row) + (bitmapX / pixels_per_byte);

	uint8_t* eight_bit_content = (uint8_t*)bitmap.content;
	uint8_t* byte_from_bitmap = eight_bit_content + content_offset_in_bytes;

	*byte_from_bitmap = (uint8_t)pixel_data;
}

static void write_color_to_4_bit_bitmap(uint32_t pixel_data, int bitmapX, int bitmapY, tl::bitmap& bitmap)
{
	const int bits_per_byte = 8;
	const int bits_per_pixel = 4;
	constexpr int pixels_per_byte = bits_per_byte / bits_per_pixel;
	int bytes_per_row = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int content_offset_in_bytes = (bitmapY * bytes_per_row) + (bitmapX / pixels_per_byte);

	uint8_t* eight_bit_content = (uint8_t*)bitmap.content;
	uint8_t* byte_from_bitmap = eight_bit_content + content_offset_in_bytes;

	uint8_t pixel_byte = (uint8_t)pixel_data;

	if (bitmapX % pixels_per_byte == 0)
	{
		*byte_from_bitmap = pixel_byte << bits_per_pixel;
	}
	else
	{
		*byte_from_bitmap = (*byte_from_bitmap & 0b11110000) | (pixel_byte & 0b00001111);
	}
}

static void write_color_to_2_bit_bitmap(uint32_t pixel_data, int bitmapX, int bitmapY, tl::bitmap& bitmap)
{
	const int bits_per_byte = 8;
	const int bits_per_pixel = 2;
	constexpr int pixels_per_byte = bits_per_byte / bits_per_pixel;
	int bytes_per_row = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int content_offset_in_bytes = (bitmapY * bytes_per_row) + (bitmapX / pixels_per_byte);

	uint8_t* eight_bit_content = (uint8_t*)bitmap.content;
	uint8_t* byte_from_bitmap = eight_bit_content + content_offset_in_bytes;

	uint8_t pixel_byte = (uint8_t)pixel_data;

	unsigned int bit_offset_from_left = bitmapX % pixels_per_byte;

	if (bit_offset_from_left == 0) {
		*byte_from_bitmap = pixel_byte << 6;
	} else if (bit_offset_from_left == 1) {
		uint8_t byte_to_write = pixel_byte << 4;
		*byte_from_bitmap = (*byte_from_bitmap & 0b11001111) | (byte_to_write & 0b00110000);
	} else if (bit_offset_from_left == 2) {
		uint8_t byte_to_write = pixel_byte << 2;
		*byte_from_bitmap = (*byte_from_bitmap & 0b11110011) | (byte_to_write & 0b00001100);
	} else {
		*byte_from_bitmap = (*byte_from_bitmap & 0b11111100) | (pixel_data & 0b00000011);
	}
}

static void WriteColorTo1BitBitmap(uint32_t pixel_data, int bitmapX, int bitmapY, tl::bitmap& bitmap)
{
	const int bitsPerByte = 8;
	int bytesPerRow = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int contentOffsetInBytes = (bitmapY * bytesPerRow) + (bitmapX / bitsPerByte);
	uint8_t* eightBitContent = (uint8_t*)bitmap.content;

	int bitOffset = bitmapX % bitsPerByte;

	uint8_t* byteFromBitmap = eightBitContent + contentOffsetInBytes;

	uint8_t or_on_bit_masks[8] = {
		0b10000000,
		0b01000000,
		0b00100000,
		0b00010000,
		0b00001000,
		0b00000100,
		0b00000010,
		0b00000001
	};

	uint8_t and_off_bit_masks[8] = {
		0b01111111,
		0b10111111,
		0b11011111,
		0b11101111,
		0b11110111,
		0b11111011,
		0b11111101,
		0b11111110
	};

	if (pixel_data == 0)
	{
		*byteFromBitmap = *byteFromBitmap & and_off_bit_masks[bitOffset];
	}
	else
	{
		*byteFromBitmap = *byteFromBitmap | or_on_bit_masks[bitOffset];
	}
}

static ColorToBitmap* ResolveColorToBitmapTransformer(int bitsPerPixel)
{
	switch (bitsPerPixel)
	{
		case 24:
			return &WriteColorTo24BitBitmap;
		case 8:
			return &write_color_to_8_bit_bitmap;
		case 4:
			return &write_color_to_4_bit_bitmap;
		case 2:
			return &write_color_to_2_bit_bitmap;
		case 1:
			return &WriteColorTo1BitBitmap;
	}

	return nullptr;
}

int InitializeBitmapFromSpriteC(
	const SpriteC& sprite,
	tl::bitmap& bitmap,
	const tl::MemorySpace tempMemory)
{
	ColorToBitmap* colorToBitmapTransformer = ResolveColorToBitmapTransformer(sprite.bitsPerPixel);
	if (colorToBitmapTransformer == nullptr) return -2;

	// rows have a byte size that is a multiple of 4 bytes (32 bits) !!!
	const int bitsPerByte = 8;
	constexpr int minimumBitsMultiplePerRow = bitsPerByte * 4;
	int rawBitsPerRow = sprite.bitsPerPixel * sprite.width;
	int thirtyTwoBitMod = rawBitsPerRow % minimumBitsMultiplePerRow;
	int bitsPerRow = (thirtyTwoBitMod == 0)
		? rawBitsPerRow
		: (rawBitsPerRow + minimumBitsMultiplePerRow - thirtyTwoBitMod);

	uint32_t imageSizeInBytes = bitsPerRow * sprite.height / bitsPerByte;

	bitmap.file_header.fileType = 0x4d42;
	bitmap.file_header.reserved1 = 0;
	bitmap.file_header.reserved2 = 0;

	// work out header space requirements
	const int file_header_size_in_bytes = 14;
	const int dibs_header_size_in_bytes = 40;
	bitmap.color_table.size = (sprite.bitsPerPixel == 1) ? 2
		: (sprite.bitsPerPixel == 2) ? 4
		: (sprite.bitsPerPixel == 4) ? 16
			: 0;
	int color_table_size_in_bytes = (sprite.has_color_table())
		? (sizeof(uint32_t) * bitmap.color_table.size)
		: 0;

	bitmap.file_header.offsetToPixelDataInBytes = file_header_size_in_bytes + dibs_header_size_in_bytes + color_table_size_in_bytes;

	bitmap.dibs_header.headerSizeInBytes = dibs_header_size_in_bytes ;
	bitmap.dibs_header.width = sprite.width;
	bitmap.dibs_header.height = sprite.height;
	bitmap.dibs_header.numberOfColorPlanes = 1;
	bitmap.dibs_header.bitsPerPixel = sprite.bitsPerPixel;
	bitmap.dibs_header.compressionMethod = 0;
	bitmap.dibs_header.imageSizeInBytes = imageSizeInBytes;

	bitmap.file_header.fileSizeInBytes = bitmap.file_header.offsetToPixelDataInBytes + bitmap.dibs_header.imageSizeInBytes;

	if (bitmap.file_header.fileSizeInBytes > tempMemory.sizeInBytes) return -1;

	for (int i = 0; i < bitmap.color_table.size; i += 1)
	{
		bitmap.color_table.content[i] = sprite.p_color_table->get_pixel_data(i);
	}

	bitmap.content = (uint8_t*)tempMemory.content;

	// Bitmap & SpriteC origins are both bottom left
	unsigned int pixelIndex = 0;
	for (unsigned int pixelY = 0; pixelY < sprite.height; pixelY += 1)
	{
		for (unsigned int pixelX = 0; pixelX < sprite.width; pixelX += 1)
		{
			uint32_t pixel_data = sprite.get_pixel_data(pixelIndex);
		;	(*colorToBitmapTransformer)(pixel_data, pixelX, pixelY, bitmap);
			pixelIndex += 1;
		}
	}

	return 0;
}

int InitializeSpriteCFromBitmap(
	SpriteC& sprite,
	const tl::bitmap& bitmap
)
{
	uint32_t garbage;
	int get_data_result = tl::bitmap_interface_get_pixel_data(bitmap, 0, 0, garbage);
	if (get_data_result != 0) return -1;

	sprite.bitsPerPixel = bitmap.dibs_header.bitsPerPixel;
	int spritePixelCount = bitmap.dibs_header.width * bitmap.dibs_header.height;

	if ((sizeof(uint32_t) * spritePixelCount) > sprite.capacity_in_bytes()) return -2;

	sprite.width = bitmap.dibs_header.width;
	sprite.height = bitmap.dibs_header.height;

	sprite.p_color_table->width = 1;
	sprite.p_color_table->height = bitmap.color_table.size;
	sprite.p_color_table->bitsPerPixel = 24;
	for (unsigned int i = 0; i < (unsigned int)bitmap.color_table.size; i += 1)
	{
		// TODO: check pixel memory on color table
		sprite.p_color_table->set_pixel_data(i, bitmap.color_table.content[i]);
	}

	// Bitmap & SpriteC origins are both bottom left
	unsigned int pixelIndex = 0;
	for (unsigned int pixelY = 0; pixelY < sprite.height; pixelY += 1)
	{
		for (unsigned int pixelX = 0; pixelX < sprite.width; pixelX += 1)
		{
			uint32_t pixel_data;
			tl::bitmap_interface_get_pixel_data(bitmap, pixelX, pixelY, pixel_data);
			sprite.set_pixel_data(pixelIndex, pixel_data);
			pixelIndex += 1;
		}
	}

	return 0;
}
