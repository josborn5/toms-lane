#ifndef TOMS_LANE_PLATFORM_BITMAP
#define TOMS_LANE_PLATFORM_BITMAP

#include <stdint.h>

namespace tl
{
	struct bitmap_header
	{
		uint16_t fileType;
		uint32_t fileSizeInBytes;
		uint16_t reserved1;
		uint16_t reserved2;

		uint32_t headerSizeInBytes;

		int32_t width;
		int32_t height;
		uint16_t numberOfColorPlanes;
		uint16_t numberOfBitsPerPixel;
		uint32_t compressionMethod;
		uint32_t imageSize;
		int32_t horizontalImagePixelsPerMeter;
		int32_t verticalImagePixelsPerMeter;
		uint32_t numberOfColorsInPalette;
		uint32_t numberOfImportantColors;
	};

	struct bitmap
	{
		bitmap_header header;
		void* content;
	}

	bitmap_interface_render(const RenderBuffer& buffer, const bitmap& bitmap, Vec2<int> topLeftCornerPosition);
}

#endif
