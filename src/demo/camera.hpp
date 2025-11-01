#ifndef DEMO_CAMERA
#define DEMO_CAMERA

#include "../tl-library.hpp"

struct cuboid {
	tl::Vec3<float> position = {0};
	tl::Vec3<float> half_size = {0};
};

struct Camera
{
	tl::Vec3<float> position = {0};
	tl::Vec3<float> direction = {0};
	tl::Vec3<float> up = {0};

	tl::Vec3<float> unit_direction = {0};

	float yaw = 0.0f;
	float field_of_view_deg = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
};


void set_projection_matrix(const Camera& camera);

void reset_camera(const cuboid& world, Camera& camera);

const tl::Matrix4x4<float>& get_projection_matrix();

void yaw_camera(float delta_angle, Camera& camera);

void move_camera_forwards(float delta_z, Camera& camera);

void strafe_camera(float delta_x, Camera& camera);

#endif

