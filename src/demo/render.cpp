#include "../tl-library.hpp"

void DrawTriangleInPixels(const tl::RenderBuffer& renderBuffer, uint32_t color, const tl::Vec2<int>& p0, const tl::Vec2<int>& p1, const tl::Vec2<int>& p2)
{
	tl::DrawLineInPixels(renderBuffer, color, p0, p1);
	tl::DrawLineInPixels(renderBuffer, color, p1, p2);
	tl::DrawLineInPixels(renderBuffer, color, p2, p0);
}

