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


	// full-color sprite
	struct Color
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};

	struct SpriteC
	{
		int width;
		int height;
		Color* content;
	};

	char* ParseColorFromCharArray(char* content, MemorySpace& space, Color& color);
	uint64_t GetSpriteSpaceInBytes(const SpriteC& sprite);
	void LoadSpriteC(char* content, MemorySpace& space, SpriteC& sprite);

}

#endif
