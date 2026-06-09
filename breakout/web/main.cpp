#include "tl-window.hpp"
#include <stdint.h>

// sin/cos provided by JS via import (see index.html)
extern "C" float sinf(float);
extern "C" float cosf(float);
extern "C" float sqrtf(float);

static float s_t = 0.0f;

int myUpdate(const tl::Input& input, int dt, tl::VideoBuffer& vb)
{
    s_t += dt * 0.001f;

    uint32_t* p = (uint32_t*)vb.pixels;
    for (int y = 0; y < vb.height; ++y) {
        for (int x = 0; x < vb.width; ++x) {
            float dx = (float)(x - input.mouse.x) / vb.width;
            float dy = (float)(y - input.mouse.y) / vb.height;
            float d  = sqrtf(dx*dx + dy*dy);
			uint32_t red = (uint32_t)(255.0f * sinf(s_t + d * 10.0f));
			*p = (red << 24) | 0x000000FF;
			p += 1;
        }
    }

    return 0;
}

int main()
{
    tl::WindowSettings settings = { 640, 480, "TL Window" };
    tl::OpenWindow(settings);
    tl::RunWindowUpdateLoop(60, myUpdate);
    return 0;
}
