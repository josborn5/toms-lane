#ifndef TOMS_LANE_APPLICATION_H
#define TOMS_LANE_APPLICATION_H

#include "./platform.hpp"

namespace tl
{
	void Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer);
	void UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt);
}

#endif
