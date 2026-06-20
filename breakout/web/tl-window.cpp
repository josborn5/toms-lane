#include "../lib/breakout.hpp"

namespace tl
{
	// Static pixel buffer — no malloc needed
	static unsigned int s_pixels[1280 * 960]; // max 1280x960
	static RenderBuffer s_vb;
	static Input        s_input;
}

// sin/cos provided by JS via import (see index.html)
extern "C" float sinf(float);
extern "C" float cosf(float);
extern "C" float sqrtf(float);


// ---- Exports called by JS ----

extern "C" int tl_tick(int dt)
{
	return updateWindowCallback(tl::s_input, dt, tl::s_vb);
}

extern "C" unsigned int* tl_pixels()  { return tl::s_vb.pixels; }

extern "C" void tl_set_mouse(int x, int y, int left)
{
	tl::s_input.mouse.x    = x;
	tl::s_input.mouse.y    = y;
}

extern "C" int tl_main(int width, int height)
{
	tl::s_vb.pixels = tl::s_pixels;
	tl::s_vb.width  = width;
	tl::s_vb.height = height;

	return 0;
}
