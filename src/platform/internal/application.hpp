#ifndef TOMS_LANE_APPLICATION_H
#define TOMS_LANE_APPLICATION_H

#include "./platform.hpp"

namespace tl
{
	int Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer);
	int UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt);
	int UpdateSound(const SoundBuffer& soundBuffer);
}

#endif
