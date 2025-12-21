#include <assert.h>
#include "./camera.hpp"

void run_camera_tests() {
	tl::Vec3<float> p0 = { 0.0f, 0.0f, 0.0f };
	tl::Vec3<float> p1 = { 0.0f, 1.0f, 0.0f };
	tl::Vec3<float> p2 = { 1.0f, 1.0f, 0.0f };

	camera_reset(
		1.0f,
		{ 0.5f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	tl::Vec3<float> projected_p0, projected_p1, projected_p2;

	camera_project_triangle(
		100,
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x > 25.0f);
	assert(projected_p0.x < 26.0f);

	assert(projected_p0.y > 25.0f);
	assert(projected_p0.y < 26.0f);
}
