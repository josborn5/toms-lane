#include "tl-window.hpp"

#define EXPORT __attribute__((visibility("default"))) extern "C"

namespace tl
{
    // Static pixel buffer — no malloc needed
    static unsigned int s_pixels[1280 * 960]; // max 1280x960
    static VideoBuffer  s_vb;
    static Input        s_input;
    static UpdateWindowCallback s_callback;

    int OpenWindow(const WindowSettings& settings)
    {
        s_vb.pixels = s_pixels;
        s_vb.width  = settings.width;
        s_vb.height = settings.height;
        return 0;
    }

    int RunWindowUpdateLoop(int /*targetFPS*/, UpdateWindowCallback callback)
    {
        s_callback = callback;
        return 0;
        // JS drives the loop via tl_tick()
    }
}

// ---- Exports called by JS ----

EXPORT int tl_tick(int dt)
{
    if (!tl::s_callback) return 0;
    return tl::s_callback(tl::s_input, dt, tl::s_vb);
}

EXPORT unsigned int* tl_pixels()  { return tl::s_vb.pixels; }
EXPORT int           tl_width()   { return tl::s_vb.width;  }
EXPORT int           tl_height()  { return tl::s_vb.height; }

EXPORT void tl_set_mouse(int x, int y, int left)
{
	tl::s_input.mouse.x    = x;
	tl::s_input.mouse.y    = y;
	tl::s_input.mouse.buttons[0].isDown = (left != 0);
}

