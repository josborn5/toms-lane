#include <assert.h>
#include <math.h>
#include <iostream>
#include "../camera.hpp"
#include "../math.hpp"

static void run_camera_view_frustrum_clip_tests() {
	std::cout << "Running camera view frustrum clip tests..." << std::endl;

	tl::Vec3<float> p0 = { 0.0f, 0.0f, 0.0f };
	tl::Vec3<float> p1 = { 0.0f, 10.0f, 0.0f };
	tl::Vec3<float> p2 = { 10.0f, 10.0f, 0.0f };

	camera_reset(
		100,
		100,
		{ 0.5f, 0.5f, -0.5f },
		90.0f,
		1.0f,
		10.0f,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	Camera camera = camera_get();

	Plane clipping_plane;
	clipping_plane.position = camera.position;
	clipping_plane.normal = camera.view_frustrum.up_plane_normal;

	Triangle4d in;
	in.p[0] = p0;
	in.p[1] = p1;
	in.p[2] = p2;

	Triangle4d clipped_1, clipped_2;

	// clip the triangle against the camera view frustrum
	int clip_count = ClipTriangleAgainstPlane(
		clipping_plane,
		in,
		clipped_1,
		clipped_2
	);

	assert(clip_count == 1);

	assert(clipped_1.p[0].x == 0.0f);
	assert(clipped_1.p[0].y == 0.0f);
	assert(clipped_1.p[0].z == 0.0f);

	assert(clipped_1.p[1].x == 0.0f);
	assert(clipped_1.p[1].y == 1.0f);
	assert(clipped_1.p[1].z == 0.0f);

	assert(clipped_1.p[2].x == 1.0f);
	assert(clipped_1.p[2].y == 1.0f);
	assert(clipped_1.p[2].z == 0.0f);

	tl::Vec3<float> projected_p0, projected_p1, projected_p2 = { 0 };

	// project the clipped triangle on the the camera newar plane
	camera_project_triangle(
		clipped_1.p[0], clipped_1.p[1], clipped_1.p[2],
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x == 0.0f);
	assert(projected_p0.y == 0.0f);
	assert(projected_p0.z == 0.5f);

	assert(projected_p1.x == 0.0f);
	assert(projected_p1.y == 100.0f);
	assert(projected_p1.z == 0.5f);

	assert(projected_p2.x == 100.0f);
	assert(projected_p2.y == 100.0f);
	assert(projected_p2.z == 0.5f);

	std::cout << "...camera view frustrum clip tests complete!" << std::endl;
}

void run_camera_tests() {
	std::cout << "Running camera tests..." << std::endl;

	tl::Vec3<float> p0 = { 0.0f, 0.0f, 0.0f };
	tl::Vec3<float> p1 = { 0.0f, 1.0f, 0.0f };
	tl::Vec3<float> p2 = { 1.0f, 1.0f, 0.0f };
	tl::Vec3<float> projected_p0, projected_p1, projected_p2 = { 0 };

	// looking straight down +ve z-axis
	camera_reset(
		100,
		100,
		{ 0.5f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	camera_project_triangle(
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x > 0);
	assert(projected_p0.x < 100);
	assert(projected_p0.y == projected_p0.x);
	assert(projected_p0.z == 5.0f);

	assert(projected_p1.x == projected_p0.x);
	assert(projected_p1.y > 0);
	assert(projected_p1.y < 100);
	assert(projected_p1.z == projected_p0.z);

	assert(projected_p2.x > 0);
	assert(projected_p2.x < 100);
	assert(projected_p2.y == projected_p1.y);
	assert(projected_p2.z == projected_p0.z);

	// looking toward +ve z & +ve x axes
	camera_reset(
		100,
		100,
		{ -5.0f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		tl::UnitVector(tl::Vec3<float>{ 1.0f, 0.0f, 1.0f }),
		{ 0.0f, 1.0f, 0.0f }
	);

	camera_project_triangle(
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x == 50.0f);
	assert(projected_p0.y > 0);
	assert(projected_p0.y < 100);
	assert(projected_p0.z > 1.0f);
	assert(projected_p0.z < 10.0f);

	assert(projected_p1.x == projected_p0.x);
	assert(projected_p1.y > 0);
	assert(projected_p1.y < 100);
	assert(projected_p1.z == projected_p0.z);

	assert(projected_p2.x > 0);
	assert(projected_p2.x < 100);
	assert(projected_p2.y < projected_p1.y); // p2 will be further away that p0 & p1
	assert(projected_p2.z > projected_p0.z);

	// looking toward +ve z & -ve x axes
	camera_reset(
		100,
		100,
		{ 5.0f, 0.5f, -5.0f },
		45.0f,
		1.0f,
		10.0f,
		tl::UnitVector(tl::Vec3<float>{ -1.0f, 0.0f, 1.0f }),
		{ 0.0f, 1.0f, 0.0f }
	);

	camera_project_triangle(
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p1.y > projected_p0.y);
	assert(projected_p1.x == projected_p0.x);
	assert(projected_p1.z == projected_p0.z);


	assert(projected_p2.y > projected_p1.y); // p2 will be closer than p0 & p1
	assert(projected_p2.z < projected_p0.z);

	// on the near plain
	camera_reset(
		100,
		100,
		{ 0.5f, 0.5f, -0.5f },
		90.0f,
		0.5f,
		1.0f,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	Camera camera = camera_get();
	assert(camera.view_frustrum.up_plane_normal.z > 0);
	assert(camera.view_frustrum.up_plane_normal.y < 0);
	assert(camera.view_frustrum.up_plane_normal.x == 0);
	assert(camera.view_frustrum.up_plane_normal.y == -camera.view_frustrum.up_plane_normal.z);

	assert(camera.view_frustrum.down_plane_normal.z > 0);
	assert(camera.view_frustrum.down_plane_normal.y > 0);
	assert(camera.view_frustrum.down_plane_normal.x == 0);
	assert(camera.view_frustrum.down_plane_normal.y == camera.view_frustrum.up_plane_normal.z);

	assert(camera.view_frustrum.left_plane_normal.x > 0);
	assert(camera.view_frustrum.left_plane_normal.z > 0);
	assert(camera.view_frustrum.left_plane_normal.y == 0);
	assert(camera.view_frustrum.left_plane_normal.x == camera.view_frustrum.up_plane_normal.z);

	assert(camera.view_frustrum.right_plane_normal.x < 0);
	assert(camera.view_frustrum.right_plane_normal.z > 0);
	assert(camera.view_frustrum.right_plane_normal.y == 0);
	assert(camera.view_frustrum.right_plane_normal.x == -camera.view_frustrum.up_plane_normal.z);

	camera_project_triangle(
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x == 0.0f);
	assert(projected_p0.y == 0.0f);

	assert(projected_p1.x == 0.0f);
	assert(projected_p1.y == 100.0f);

	assert(projected_p2.x == 100.0f);
	assert(projected_p2.y == 100.0f);

	// on the far plain
	camera_reset(
		100,
		100,
		{ 0.5f, 0.5f, -0.5f },
		90.0f,
		0.1f,
		0.5f,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	camera = camera_get();
	assert(camera.view_frustrum.up_plane_normal.z > 0);
	assert(camera.view_frustrum.up_plane_normal.y < 0);
	assert(camera.view_frustrum.up_plane_normal.x == 0);
	assert(camera.view_frustrum.up_plane_normal.y == -camera.view_frustrum.up_plane_normal.z);

	assert(camera.view_frustrum.down_plane_normal.z > 0);
	assert(camera.view_frustrum.down_plane_normal.y > 0);
	assert(camera.view_frustrum.down_plane_normal.x == 0);
	assert(camera.view_frustrum.down_plane_normal.y == camera.view_frustrum.up_plane_normal.z);

	assert(camera.view_frustrum.left_plane_normal.x > 0);
	assert(camera.view_frustrum.left_plane_normal.z > 0);
	assert(camera.view_frustrum.left_plane_normal.y == 0);
	assert(camera.view_frustrum.left_plane_normal.x == camera.view_frustrum.up_plane_normal.z);

	assert(camera.view_frustrum.right_plane_normal.x < 0);
	assert(camera.view_frustrum.right_plane_normal.z > 0);
	assert(camera.view_frustrum.right_plane_normal.y == 0);
	assert(camera.view_frustrum.right_plane_normal.x == -camera.view_frustrum.up_plane_normal.z);

	camera_project_triangle(
		p0, p1, p2,
		projected_p0, projected_p1, projected_p2
	);

	assert(projected_p0.x == 0.0f);
	assert(projected_p0.y == 0.0f);

	assert(projected_p1.x == 0.0f);
	assert(projected_p1.y == 100.0f);

	assert(projected_p2.x == 100.0f);
	assert(projected_p2.y == 100.0f);

	std::cout << "...camera tests complete!!!" << std::endl;

	run_camera_view_frustrum_clip_tests();
}
