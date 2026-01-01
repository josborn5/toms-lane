#ifndef DEMO_DEMO
#define DEMO_DEMO

#include "../tl-library.hpp"

int demo_main();

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
	Triangle4d& inputTriangle,
	Triangle4d& outputTriangle1,
	Triangle4d& outputTriangle2
);

#endif

