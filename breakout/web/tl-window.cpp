#include "tl-window.hpp"
#include "main.hpp"

namespace tl
{
	// Static pixel buffer — no malloc needed
	static unsigned int s_pixels[1280 * 960]; // max 1280x960
	static VideoBuffer  s_vb;
	static Input        s_input;
	static UpdateWindowCallback s_callback;
}

// ---- Exports called by JS ----

extern "C" int tl_tick(int dt)
{
	if (!tl::s_callback) return 0;
	return tl::s_callback(tl::s_input, dt, tl::s_vb);
}

extern "C" unsigned int* tl_pixels()  { return tl::s_vb.pixels; }

extern "C" void tl_set_mouse(int x, int y, int left)
{
	tl::s_input.mouse.x    = x;
	tl::s_input.mouse.y    = y;
	tl::s_input.mouse.buttons[0].isDown = (left != 0);
}

extern "C" int tl_main(int width, int height)
{
	tl::s_vb.pixels = tl::s_pixels;
	tl::s_vb.width  = width;
	tl::s_vb.height = height;
	tl::s_callback = myUpdate;

	return 0;
}
