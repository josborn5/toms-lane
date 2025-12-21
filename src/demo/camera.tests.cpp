#include <assert.h>
#include "./camera.hpp"

void run_camera_tests() {
	tl::Vec3<float> p0 = { 0.0f, 0.0f, 0.0f };
	tl::Vec3<float> p1 = { 0.0f, 1.0f, 0.0f };
	tl::Vec3<float> p2 = { 1.0f, 1.0f, 0.0f };
	tl::Vec3<float> projected_p0, projected_p1, projected_p2;

	// looking straight down +ve z-axis
	camera_reset(
		1.0f,
		{ 0.5f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	camera_project_triangle(
		100,
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x > 25.0f);
	assert(projected_p0.x < 26.0f);
	assert(projected_p0.y == projected_p0.x);
	assert(projected_p0.z == 100.0f);

	assert(projected_p1.x == projected_p0.x);
	assert(projected_p1.y > 74.0f);
	assert(projected_p1.y < 75.0f);
	assert(projected_p1.z == projected_p0.z);

	assert(projected_p2.x > 74.0f);
	assert(projected_p2.x < 75.0f);
	assert(projected_p2.y == projected_p1.y);
	assert(projected_p2.z == projected_p0.z);

	// looking toward +ve z & +ve x axes
	camera_reset(
		1.0f,
		{ -5.0f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		tl::UnitVector(tl::Vec3<float>{ 1.0f, 0.0f, 1.0f }),
		{ 0.0f, 1.0f, 0.0f }
	);

	camera_project_triangle(
		100,
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x == 50.0f);
	assert(projected_p0.y > 32.0f);
	assert(projected_p0.y < 33.0f);
	assert(projected_p0.z > 95.0f);
	assert(projected_p0.z < 96.0f);

	assert(projected_p1.x == projected_p0.x);
	assert(projected_p1.y > 67.0f);
	assert(projected_p1.y < 68.0f);
	assert(projected_p1.z == projected_p0.z);

	assert(projected_p2.x > 71.0f);
	assert(projected_p2.x < 72.0f);
	assert(projected_p2.y < projected_p1.y); // p2 will be further away that p0 & p1
	assert(projected_p2.z > projected_p0.z);

	// looking toward +ve z & -ve x axes
	camera_reset(
		1.0f,
		{ 5.0f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		tl::UnitVector(tl::Vec3<float>{ -1.0f, 0.0f, 1.0f }),
		{ 0.0f, 1.0f, 0.0f }
	);

	camera_project_triangle(
		100,
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p1.y > projected_p0.y);
	assert(projected_p1.x == projected_p0.x);
	assert(projected_p1.z == projected_p0.z);


	assert(projected_p2.y > projected_p1.y); // p2 will be closer than p0 & p1
	assert(projected_p2.z < projected_p0.z);
}
