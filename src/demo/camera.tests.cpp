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
	
	tl::Matrix4x4<float> view_matrix;
	camera_fill_view_matrix(view_matrix);
}
