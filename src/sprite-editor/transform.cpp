#include "../tl-application.hpp"
#include "../tl-library.hpp"
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

static void WriteColorTo1BitBitmap(uint32_t spriteColor, int bitmapX, int bitmapY, tl::bitmap& bitmap)
{
	const int bitsPerByte = 8;
	int bytesPerRow = bitmap.dibs_header.imageSizeInBytes / bitmap.dibs_header.height;

	int contentOffsetInBytes = (bitmapY * bytesPerRow) + (bitmapX / bitsPerByte);
	uint8_t* eightBitContent = (uint8_t*)bitmap.content;

	int bitOffset = bitmapX % bitsPerByte;
	int bitShiftOffset = bitsPerByte - bitOffset - 1;

	uint8_t* byteFromBitmap = eightBitContent + contentOffsetInBytes;

	bool isBlack = spriteColor == 0x0000FF;
	uint8_t pixel_data = (isBlack) ? 0b00000000 : 0b00000001;

	// 1.shift the bit of interest over to the right most bit
	// 2. OR with a mask to persist preceeding bits and set the right most bit as true/false according to the pixel color
	uint8_t workingByte = ((*byteFromBitmap >> bitShiftOffset) | pixel_data);
	// 3. Shift back and store the byte
	*byteFromBitmap = (workingByte << bitShiftOffset);
}

static ColorToBitmap* ResolveColorToBitmapTransformer(int bitsPerPixel)
{
	switch (bitsPerPixel)
	{
		case 24:
			return &WriteColorTo24BitBitmap;
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
	int color_table_size_in_bytes = (sprite.color_table.length() > 0)
		? sizeof(uint32_t[2])
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

	bitmap.color_table.size = sprite.color_table.length();
	for (int i = 0; i < sprite.color_table.length(); i += 1)
	{
		bitmap.color_table.content[i] = sprite.color_table.get_copy(i);
	}

	bitmap.content = (uint8_t*)tempMemory.content;

	// Bitmap & SpriteC origins are both bottom left
	int pixelIndex = 0;
	for (int pixelY = 0; pixelY < sprite.height; pixelY += 1)
	{
		for (int pixelX = 0; pixelX < sprite.width; pixelX += 1)
		{
			uint32_t pixel_data = sprite.pixels()[pixelIndex];
			(*colorToBitmapTransformer)(pixel_data, pixelX, pixelY, bitmap);
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

	if ((sizeof(uint32_t) * spritePixelCount) > sprite.pixel_memory.sizeInBytes) return -2;

	sprite.width = bitmap.dibs_header.width;
	sprite.height = bitmap.dibs_header.height;

	sprite.color_table.clear();
	for (int i = 0; i < bitmap.color_table.size; i += 1)
	{
		sprite.color_table.append(bitmap.color_table.content[i]);
	}

	// Bitmap & SpriteC origins are both bottom left
	int pixelIndex = 0;
	for (int pixelY = 0; pixelY < sprite.height; pixelY += 1)
	{
		for (int pixelX = 0; pixelX < sprite.width; pixelX += 1)
		{
			uint32_t pixel_data;
			tl::bitmap_interface_get_pixel_data(bitmap, pixelX, pixelY, pixel_data);
			sprite.pixels()[pixelIndex] = pixel_data;
			pixelIndex += 1;
		}
	}

	return 0;
}
