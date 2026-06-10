#pragma once

namespace tl
{
    struct VideoBuffer {
        unsigned int* pixels; // RGBA
        int width;
        int height;
    };
}
