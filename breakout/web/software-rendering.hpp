#pragma once

namespace tl
{
    struct VideoBuffer {
        unsigned char* pixels; // RGBA
        int width;
        int height;
    };
}
