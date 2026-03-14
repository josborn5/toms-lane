#include "../../platform/lib/tl-library.hpp"

float ShortestDistanceFromPointToPlane(
	const tl::Vec3<float>& point,
	const tl::Vec3<float>& plane_point,
	const tl::Vec3<float>& unit_normal_to_plane
);

struct Plane
{
	tl::Vec3<float> position = {0};
	tl::Vec3<float> normal = {0};
};

struct Triangle4d
{
	tl::Vec3<float> p[3] = {0};
	uint8_t color_r = 0;
	uint8_t color_g = 0;
	uint8_t color_b = 0;
};

int ClipTriangleAgainstPlane(
	const Plane& plane,
	const Triangle4d& inputTriangle,
	Triangle4d& outputTriangle1,
	Triangle4d& outputTriangle2
);

