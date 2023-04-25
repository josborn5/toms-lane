#ifndef TOMS_LANE_SOFTWARE_RENDERING_H
#define TOMS_LANE_SOFTWARE_RENDERING_H

#include "./platform.hpp"
#include "./math.hpp"
#include "./geometry.hpp"
#include <stdint.h>

namespace tl
{
	struct Sprite
	{
		char* content;
		int height;
		int width;
	};

	Vec2<int> GetContentDimensions(char* content);
	Sprite LoadSprite(char* content);

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
	void LoadSpriteC(char* content, MemorySpace& space, SpriteC& sprite);
	uint64_t GetSpriteSpaceInBytes(const SpriteC& sprite);

	/**
	 *	|---|---|---|
	 *	| 0 | 1 | 2 |	pixel ordinals
	 *	|---|---|---|
	 *	0   1   2   3	position ordinals
	 *
	 * x & y parameters are the pixel and NOT the position ordinals
	 */
	void PlotPixel(const RenderBuffer &renderBuffer, uint32_t color, int x, int y);

	/**
	 *	|---|---|---|
	 *	| 0 | 1 | 2 |	pixel ordinals
	 *	|---|---|---|
	 *	0   1   2   3	position ordinals
	 *
	 * p0 & p1 are pixel and NOT position ordinals
	 */
	// Implemented with Bresenham's algorithm
	void DrawLineInPixels(
		const RenderBuffer &renderBuffer,
		uint32_t color,
		const Vec2<int> &p0,
		const Vec2<int> &p1
	);
	void DrawLineInPixels(
		const RenderBuffer &renderBuffer,
		uint32_t color,
		const Vec2<float> &p0,
		const Vec2<float> &p1
	);

	// Rects
	void DrawRect(const RenderBuffer &renderBuffer, uint32_t color, const Rect<float> &rect);

	// Draw a sprite of a size determined by the given sprite string and blockHalfSize value
	void DrawSprite(
		const RenderBuffer &renderBuffer,
		const Sprite &sprite,
		const Rect<float> &footprint,
		uint32_t color
	);
	// Draw a sprite of a size determined by the given sprite string and blockHalfSize value
	void DrawSpriteC(
		const RenderBuffer &renderBuffer,
		const SpriteC &sprite,
		const Rect<float> &footprint
	);

	void DrawAlphabetCharacters(
		const RenderBuffer &renderBuffer,
		char *text,
		const tl::Rect<float> &footprint,
		uint32_t color
	);

	void DrawNumber(
		const RenderBuffer &renderBuffer,
		int number,
		const tl::Rect<float> &firstCharFootprint,
		uint32_t color
	);

	// Triangles
	void FillTriangleInPixels(const RenderBuffer &renderBuffer, uint32_t color, const Vec3<int> &p0, const Vec3<int> &p1, const Vec3<int> &p2, float z);


	void DrawTriangleInPixels(const RenderBuffer &renderBuffer, uint32_t color, const Vec2<int> &p0, const Vec2<int> &p1, const Vec2<int> &p2);

	void ClearScreen(const RenderBuffer &renderBuffer, uint32_t color);

	uint32_t GetColorFromRGB(int red, int green, int blue);

	template<typename T>
	void TransformAndRenderMesh(
		const RenderBuffer &renderBuffer,
		const MeshArray<T> &mesh,
		const Camera<T> &camera,
		const Matrix4x4<T>& transformMatrix,
		const Matrix4x4<T>& projectionMatrix,
		const MemorySpace& transient
	);
}

#endif
