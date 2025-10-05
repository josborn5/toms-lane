#ifndef TOMS_LANE_SOFTWARE_RENDERING_H
#define TOMS_LANE_SOFTWARE_RENDERING_H

#include "./software-rendering.sprite.hpp"
#include "./math.hpp"
#include "./geometry.hpp"
#include <stdint.h>

namespace tl
{
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
	void DrawRect(
		const RenderBuffer &renderBuffer,
		uint32_t color,
		const Rect<float> &rect
	);

	void render_interface_fill_rect_rgba(const RenderBuffer& render_buffer, uint32_t color, const Rect<float> footprint);

	// Circles
	void DrawCircle(const RenderBuffer &renderBuffer, uint32_t color, const Vec2<float>& center, float radius);

	// Triangles
	void FillTriangleInPixels(const RenderBuffer &renderBuffer, uint32_t color, const Vec3<int> &p0, const Vec3<int> &p1, const Vec3<int> &p2);

	void ClearScreen(const RenderBuffer &renderBuffer, uint32_t color);

	uint32_t GetColorFromRGB(int red, int green, int blue);
}

#endif
