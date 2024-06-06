#ifndef TOMS_LANE_PLATFORM_BITMAP
#define TOMS_LANE_PLATFORM_BITMAP

#include <stdint.h>
#include "../tl-application.hpp"
#include "./math.hpp"

namespace tl
{
	struct bitmap_file_header
	{
		uint16_t fileType;

		int32_t fileSizeInBytes;
		uint16_t reserved1;
		uint16_t reserved2;

		int32_t offsetToPixelDataInBytes;
	};

	struct bitmap_dibs_header
	{
		uint32_t headerSizeInBytes;

		int32_t width;
		int32_t height;

		uint16_t numberOfColorPlanes;
		uint16_t bitsPerPixel;
		uint32_t compressionMethod;
		uint32_t imageSizeInBytes;

		int32_t horizontalPixelsPerMeter;
		int32_t verticalPixelsPerMeter;

		uint32_t numberOfColorsInPalette;
		uint32_t numberOfImportantColors;
	};


	struct bitmap
	{
		bitmap_file_header file_header = {0};
		bitmap_dibs_header dibs_header = {0};
		void* content = nullptr; // number of bits per pixel can vary
	};

	int bitmap_interface_initialize(
		bitmap& bitmap,
		const MemorySpace& memory);

	int bitmap_interface_render(
		const RenderBuffer& buffer,
		const bitmap& bitmap,
		Vec2<int> bottomLeftCornerPosition);
}

#endif
