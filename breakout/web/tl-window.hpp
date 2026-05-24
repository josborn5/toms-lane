#pragma once
#include "tl-input.hpp"
#include "software-rendering.hpp"

namespace tl
{
    struct WindowSettings {
        int width;
        int height;
        const char* title;
    };

    typedef int (*UpdateWindowCallback)(const Input& input, int dtInMilliseconds, VideoBuffer& videoBuffer);

    int OpenWindow(const WindowSettings& settings);
    int RunWindowUpdateLoop(int targetFPS, UpdateWindowCallback callback);
}
