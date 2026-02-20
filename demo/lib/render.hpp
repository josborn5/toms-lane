#include "../../platform/lib/tl-library.hpp"

void DrawTriangleInPixels(
	const tl::RenderBuffer& renderBuffer,
	uint32_t color,
	const tl::Vec2<int>& p0,
	const tl::Vec2<int>& p1,
	const tl::Vec2<int>& p2
);

struct z_buffer {
	unsigned int width = 0;
	unsigned int height = 0;
	float* depths = nullptr;
};

void triangle_fill(
	const tl::RenderBuffer& render_buffer,
	z_buffer& depth_buffer,
	uint32_t color,
	const tl::Vec3<float>& p0,
	const tl::Vec3<float>& p1,
	const tl::Vec3<float>& p2
);

struct plane_coeff
{
	float a = 0.0f;
	float b = 0.0f;
	float c = 0.0f;
	float d = 0.0f;
};

void fill_triangle_plane_coeff(
	const tl::Vec3<float>& p0,
	const tl::Vec3<float>& p1,
	const tl::Vec3<float>& p2,
	plane_coeff& coefficients
);

float plane_z_value_get(
	float x,
	float y,
	const plane_coeff& coefficients
);
