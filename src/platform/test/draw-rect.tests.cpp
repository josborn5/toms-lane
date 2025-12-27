#include <assert.h>
#include "../software-rendering.hpp"
#include "../../tl-application.hpp"

static void run_fill_rgba_rect_tests()
{
	tl::MemorySpace renderBufferPixels;
	tl::RenderBuffer renderBuffer;

	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;

	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferPixels.content = malloc(renderBufferPixels.sizeInBytes);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;

	tl::Rect<float> wholeBufferRect;
	wholeBufferRect.position = { 6.0f, 4.0f };
	wholeBufferRect.halfSize = { 6.0f, 4.0f };

	tl::render_interface_fill_rect_rgba(renderBuffer, 0xFFFFFFFF, wholeBufferRect);
}

void RunDrawRectTests()
{
	tl::MemorySpace renderBufferPixels;
	tl::RenderBuffer renderBuffer;

	renderBuffer.width = 12;
	renderBuffer.height = 8;
	renderBuffer.bytesPerPixel = 4;

	renderBufferPixels.sizeInBytes = sizeof(unsigned int) * renderBuffer.width * renderBuffer.height;
	renderBufferPixels.content = malloc(renderBufferPixels.sizeInBytes);
	renderBuffer.pixels = (unsigned int*)renderBufferPixels.content;

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

	run_fill_rgba_rect_tests();
}
