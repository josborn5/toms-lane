#include <assert.h>
#include "./software-rendering.hpp"
#include "../tl-application.hpp"

void RunDrawRectTests()
{
	tl::MemorySpace renderBufferPixels;
	tl::MemorySpace renderBufferDepth;
	tl::RenderBuffer renderBuffer;

	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;

	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferDepth.sizeInBytes = sizeof(float) * renderBuffer.width * renderBuffer.height;
	tl::memory_interface_initialize(renderBufferPixels);
	tl::memory_interface_initialize(renderBufferDepth);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;
	renderBuffer.depth = (float*)renderBufferDepth.content;

	tl::Rect<float> wholeBufferRect;
	wholeBufferRect.position = { 6.0f, 4.0f };
	wholeBufferRect.halfSize = { 6.0f, 4.0f };

	tl::DrawRect(renderBuffer, 0xFF0000, wholeBufferRect);

	assert(*renderBuffer.pixels == 0xFF0000);
	assert(*(renderBuffer.pixels + 20) == 0xFF0000);

	wholeBufferRect.halfSize = { 8.0f, 6.0f };

	tl::DrawRect(renderBuffer, 0xFF0000, wholeBufferRect);

	assert(*renderBuffer.pixels == 0xFF0000);
	assert(*(renderBuffer.pixels + 20) == 0xFF0000);
}
