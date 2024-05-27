#ifndef TOMS_LANE_PLATFORM_BITMAP
#define TOMS_LANE_PLATFORM_BITMAP

#include <stdint.h>
#include "../tl-application.hpp"
#include "./math.hpp"

namespace tl
{
	struct bitmap_header
	{
		uint16_t fileType;

		int16_t fileSizeInBytes;
		uint16_t reserved1;
		uint16_t reserved2;
		uint16_t spacer1;

		uint16_t offsetToPixelDataInBytes;
		uint16_t spacer2;
		uint16_t headerSizeInBytes;
		uint16_t spacer3;

		int16_t width;
		uint16_t spacer4;

		int16_t height;
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
		bitmap_header* header;
		void* content;
	};

	int bitmap_interface_initialize(
		bitmap& bitmap,
		const MemorySpace& memory);

	int bitmap_interface_render(
		const RenderBuffer& buffer,
		const bitmap& bitmap,
		Vec2<int> topLeftCornerPosition);
}

#endif
