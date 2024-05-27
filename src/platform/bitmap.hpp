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
		uint16_t spacer5;

		uint16_t numberOfColorPlanes;
		uint16_t numberOfBitsPerPixel;
		uint16_t compressionMethod;
		uint16_t spacer6;

		uint16_t imageSizeInBytes;
		uint16_t spacer7;

		int16_t horizontalPixelsPerMeter;
		uint16_t spacer8;

		int16_t verticalPixelsPerMeter;
		uint16_t spacer9;

		uint16_t numberOfColorsInPalette;
		uint16_t spacer10;

		uint16_t numberOfImportantColors;
		uint16_t spacer11;
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
