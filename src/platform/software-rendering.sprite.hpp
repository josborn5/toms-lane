#ifndef TOMS_LANE_SOFTWARE_RENDERING_SPRITE_H
#define TOMS_LANE_SOFTWARE_RENDERING_SPRITE_H

#include "./math.hpp"
#include "./geometry.hpp"
#include "../tl-application.hpp"

namespace tl
{
	// monochrome sprite
	struct Sprite
	{
		char* content;
		int height;
		int width;
	};

	Sprite LoadSprite(char* content);
	Vec2<int> GetContentDimensions(char* content);

	// Draw a sprite of a size determined by the given sprite string and blockHalfSize value
	void DrawSprite(
		const RenderBuffer &renderBuffer,
		const Sprite &sprite,
		const Rect<float> &footprint,
		uint32_t color
	);

}

#endif
