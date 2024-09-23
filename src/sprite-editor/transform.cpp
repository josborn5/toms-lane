#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

struct RGB24Bit
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

typedef void ColorToBitmap (const Color& spriteColor, int bitmapPixelIndex, tl::bitmap& bitmap);

static void WriteColorTo24BitBitmap(const Color& spriteColor, int bitmapPixelIndex, tl::bitmap& bitmap)
{
	RGB24Bit bitmapPixel;
	bitmapPixel.r = (uint8_t)(255.0f * spriteColor.r);
	bitmapPixel.g = (uint8_t)(255.0f * spriteColor.g);
	bitmapPixel.b = (uint8_t)(255.0f * spriteColor.b);
	*((RGB24Bit*)bitmap.content + bitmapPixelIndex) = bitmapPixel;
}

static ColorToBitmap* ResolveColorToBitmapTransformer(int bitsPerPixel)
{
	switch (bitsPerPixel)
	{
		case 24:
			return &WriteColorTo24BitBitmap;
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

	uint16_t bitsPerPixel = sprite.bitsPerPixel;
	uint32_t imageSizeInBytes = bitsPerPixel * sprite.height * sprite.width / 8;

	bitmap.file_header.fileType = 0x4d42;
	bitmap.file_header.reserved1 = 0;
	bitmap.file_header.reserved2 = 0;
	bitmap.file_header.offsetToPixelDataInBytes = 54;

	bitmap.dibs_header.headerSizeInBytes = 40;
	bitmap.dibs_header.width = sprite.width;
	bitmap.dibs_header.height = sprite.height;
	bitmap.dibs_header.numberOfColorPlanes = 1;
	bitmap.dibs_header.bitsPerPixel = bitsPerPixel;
	bitmap.dibs_header.compressionMethod = 0;
	bitmap.dibs_header.imageSizeInBytes = imageSizeInBytes;

	bitmap.file_header.fileSizeInBytes = bitmap.file_header.offsetToPixelDataInBytes + bitmap.dibs_header.imageSizeInBytes;

	if (bitmap.file_header.fileSizeInBytes > tempMemory.sizeInBytes) return -1;

	bitmap.content = (uint8_t*)tempMemory.content;
	int bitmapPixelIndex = 0;
	int spritePixelCount = sprite.width * sprite.height;

	// SpriteC origin is top left
	// Bitmap origin is bottom left
	int bottomLeftSpritePixelIndex =  spritePixelCount - sprite.width;
	for (int startRowPixelIndex = bottomLeftSpritePixelIndex; startRowPixelIndex >= 0; startRowPixelIndex -= sprite.width)
	{
		for (int columnIndex = 0; columnIndex < sprite.width; columnIndex += 1)
		{
			int spritePixelIndex = startRowPixelIndex + columnIndex;
			Color spriteColor = sprite.content[spritePixelIndex];
			(*colorToBitmapTransformer)(spriteColor, bitmapPixelIndex, bitmap);
			bitmapPixelIndex += 1;
		}
	}

	return 0;
}

typedef Color BitmapToColor (const tl::bitmap& bitmap, int bitmapPixelIndex);

static Color GetColorFrom24BitBitmap(const tl::bitmap& bitmap, int bitmapPixelIndex)
{
	RGB24Bit bitmapPixel = *((RGB24Bit*)bitmap.content + bitmapPixelIndex);

	Color spriteColor;
	spriteColor.r = (float)bitmapPixel.r / 255.0f;
	spriteColor.g = (float)bitmapPixel.g / 255.0f;
	spriteColor.b = (float)bitmapPixel.b / 255.0f;

	return spriteColor;
}

static Color GetColorFrom1BitBitmap(const tl::bitmap& bitmap, int bitmapPixelIndex)
{
	int pixelsPerByte = (bitmap.dibs_header.width * bitmap.dibs_header.height) / bitmap.dibs_header.imageSizeInBytes;
	uint8_t* eightBitContent = (uint8_t*)bitmap.content;
	Color spriteColor;

	const float white = 1.0f;
	const float black = 0.0f;
	int byteOffset = bitmapPixelIndex / pixelsPerByte;
	int bitOffset = bitmapPixelIndex  % pixelsPerByte;
	int bitShiftOffset = 8 - bitOffset - 1;

	uint8_t* byteFromBitmap = eightBitContent + byteOffset;

	// 1.shift the bit of interest over to the right most bit
	// 2. AND with a mask to evaluate the right most bit as true/false
	// 3. true --> white, false --> black
	float floatColor =  ((*byteFromBitmap  >> bitShiftOffset) & 0b00000001) ? white : black;
	spriteColor.r = floatColor;
	spriteColor.g = floatColor;
	spriteColor.b = floatColor;
	return spriteColor;
}

static BitmapToColor* ResolveBitmapToColorTransformer(int bitsPerPixel)
{
	switch (bitsPerPixel)
	{
		case 24:
			return &GetColorFrom24BitBitmap;
		case 1:
			return &GetColorFrom1BitBitmap;
	}

	return nullptr;
}

int InitializeSpriteCFromBitmap(
	SpriteC& sprite,
	const tl::bitmap& bitmap,
	const tl::MemorySpace spriteMemory
)
{
	BitmapToColor* bitmapToColorTransformer = ResolveBitmapToColorTransformer(bitmap.dibs_header.bitsPerPixel);
	if (bitmapToColorTransformer  == nullptr) return -1;

	sprite.bitsPerPixel = bitmap.dibs_header.bitsPerPixel;
	int spritePixelCount = bitmap.dibs_header.width * bitmap.dibs_header.height;

	if ((sizeof(Color) * spritePixelCount) > spriteMemory.sizeInBytes) return -2;

	sprite.width = bitmap.dibs_header.width;
	sprite.height = bitmap.dibs_header.height;

	// SpriteC origin is top left
	// Bitmap origin is bottom left
	int bottomLeftSpritePixelIndex =  spritePixelCount - sprite.width;
	int bitmapPixelIndex = 0;
	for (int startRowPixelIndex = bottomLeftSpritePixelIndex; startRowPixelIndex >= 0; startRowPixelIndex -= sprite.width)
	{
		for (int columnIndex = 0; columnIndex < sprite.width; columnIndex += 1)
		{
			int spritePixelIndex = startRowPixelIndex + columnIndex;
			sprite.content[spritePixelIndex] = (*bitmapToColorTransformer)(bitmap, bitmapPixelIndex);
			bitmapPixelIndex += 1;
		}
	}

	return 0;
}
