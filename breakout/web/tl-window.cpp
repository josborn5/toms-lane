#include "../../application/src/tl-input.hpp"

namespace tl
{
	struct RenderBuffer
	{
		unsigned int* pixels = nullptr;
		int width = 0;
		int height = 0;
	};

	// Static pixel buffer — no malloc needed
	static unsigned int s_pixels[1280 * 960]; // max 1280x960
	static RenderBuffer s_vb;
	static Input        s_input;
}

// sin/cos provided by JS via import (see index.html)
extern "C" float sinf(float);
extern "C" float cosf(float);
extern "C" float sqrtf(float);

static float s_t = 0.0f;

int myUpdate(const tl::Input& input, int dt, tl::RenderBuffer& vb)
{
    s_t += dt * 0.001f;

    unsigned int* p = vb.pixels;
    for (int y = 0; y < vb.height; ++y) {
        for (int x = 0; x < vb.width; ++x) {
            float dx = (float)(x - input.mouse.x) / vb.width;
            float dy = (float)(y - input.mouse.y) / vb.height;
            float d  = sqrtf(dx*dx + dy*dy);
			unsigned int red = (unsigned int)(255.0f * sinf(s_t + d * 10.0f));
			*p = (red << 24) | 0x000000FF;
			p += 1;
        }
    }

    return 0;
}



// ---- Exports called by JS ----

extern "C" int tl_tick(int dt)
{
	return myUpdate(tl::s_input, dt, tl::s_vb);
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
