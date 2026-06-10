#pragma once
#include "../../application/src/tl-input.hpp"
#include "software-rendering.hpp"

namespace tl
{
    typedef int (*UpdateWindowCallback)(const Input& input, int dtInMilliseconds, VideoBuffer& videoBuffer);
}
