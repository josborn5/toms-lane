#include "../tl-library.hpp"

float ShortestDistanceFromPointToPlane(
	const tl::Vec3<float>& point,
	const tl::Vec3<float>& plane_point,
	const tl::Vec3<float>& unit_normal_to_plane
);

struct Plane
{
	tl::Vec3<float> position;
	tl::Vec3<float> normal;
};

struct Triangle4d
{
	tl::Vec3<float> p[3];
	unsigned int color;
};

int ClipTriangleAgainstPlane(
	const Plane& plane,
	const Triangle4d& inputTriangle,
	Triangle4d& outputTriangle1,
	Triangle4d& outputTriangle2
);

