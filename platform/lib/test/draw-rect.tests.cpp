#include <assert.h>
#include "../software-rendering.hpp"
#include "../memory.hpp"
#include <stdio.h>

static void run_fill_rgba_rect_tests()
{
	tl::MemorySpace renderBufferPixels;
	tl::RenderBuffer renderBuffer;

	renderBufferPixels.sizeInBytes = sizeof(uint32_t) * 12 * 8;
	renderBufferPixels.content = malloc(renderBufferPixels.sizeInBytes);

	renderBuffer.init(
		(uint32_t*)renderBufferPixels.content,
		12,
		8,
		tl::frame_buffer_origin_bottom_left
	);

	tl::Rect<float> wholeBufferRect;
	wholeBufferRect.position = { 6.0f, 4.0f };
	wholeBufferRect.halfSize = { 6.0f, 4.0f };

	tl::render_interface_fill_rect_rgba(renderBuffer, 0xFFFFFFFF, wholeBufferRect);
}

void RunDrawRectTests()
{
	printf("Start RunDrawRectTests...\n");
	tl::MemorySpace renderBufferPixels;
	tl::RenderBuffer renderBuffer;

	renderBufferPixels.sizeInBytes = sizeof(uint32_t) * 12 * 8;
	renderBufferPixels.content = malloc(renderBufferPixels.sizeInBytes);

	renderBuffer.init(
		(uint32_t*)renderBufferPixels.content,
		12,
		8,
		tl::frame_buffer_origin_bottom_left
	);

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

	printf("... RunDrawRectTests complete!\n");
}
