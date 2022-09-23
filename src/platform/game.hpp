#ifndef TOMS_LANE_GAME_H
#define TOMS_LANE_GAME_H

namespace tl
{
	void Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer);
	void UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt);
}

#endif
