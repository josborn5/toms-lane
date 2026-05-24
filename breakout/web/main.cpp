#include "tl-window.hpp"

// sin/cos provided by JS via import (see index.html)
extern "C" float sinf(float);
extern "C" float cosf(float);
extern "C" float sqrtf(float);

static float s_t = 0.0f;

int myUpdate(const tl::Input& input, int dt, tl::VideoBuffer& vb)
{
    s_t += dt * 0.001f;

    unsigned char* p = vb.pixels;
    for (int y = 0; y < vb.height; ++y) {
        for (int x = 0; x < vb.width; ++x) {
            float dx = (float)(x - input.mouseX) / vb.width;
            float dy = (float)(y - input.mouseY) / vb.height;
            float d  = sqrtf(dx*dx + dy*dy);
            p[0] = (unsigned char)(128 + 127 * sinf(s_t + d * 10.0f));
            p[1] = (unsigned char)(128 + 127 * sinf(s_t + d * 10.0f + 2.0f));
            p[2] = (unsigned char)(128 + 127 * cosf(s_t + d * 8.0f));
            p[3] = 255;
            p += 4;
        }
    }

    return input.keys[27]; // Escape to quit
}

int main()
{
    tl::WindowSettings settings = { 640, 480, "TL Window" };
    tl::OpenWindow(settings);
    tl::RunWindowUpdateLoop(60, myUpdate);
    return 0;
}
