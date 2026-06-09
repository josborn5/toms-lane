#pragma once
#include <stdint.h>

namespace tl
{
    struct VideoBuffer {
        uint32_t* pixels; // RGBA
        int width;
        int height;
    };
}
