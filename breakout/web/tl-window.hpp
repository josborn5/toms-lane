#pragma once
#include "../../application/src/tl-input.hpp"

namespace tl
{
    struct VideoBuffer {
        unsigned int* pixels; // RGBA
        int width;
        int height;
    };

    typedef int (*UpdateWindowCallback)(const Input& input, int dtInMilliseconds, VideoBuffer& videoBuffer);
}
