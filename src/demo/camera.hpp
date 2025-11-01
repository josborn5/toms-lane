#ifndef DEMO_CAMERA
#define DEMO_CAMERA

#include "../tl-library.hpp"

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


void camera_reset(
	const tl::Vec3<float>& position,
	float field_of_view_in_deg,
	float near_plane,
	float far_plane);

const tl::Matrix4x4<float>& get_projection_matrix();

void camera_increment_yaw(float delta_angle_in_deg);

void camera_increment_direction(float delta_z);

void camera_increment_strafe(float delta_x);

void camera_increment_up(float delta_up);

void camera_set_fov(float fov_in_deg);

void camera_set_near_plane(float near_plane);

void camera_set_position(const tl::Vec3<float>& position);

const Camera& camera_get();

#endif

