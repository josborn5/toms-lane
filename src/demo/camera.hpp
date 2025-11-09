#ifndef DEMO_CAMERA
#define DEMO_CAMERA

#include "../tl-library.hpp"

struct view_frustrum {
	tl::Vec3<float> near_plane_normal = {0};
	tl::Vec3<float> near_plane_position = {0};

	tl::Vec3<float> far_plane_normal = {0};
	tl::Vec3<float> far_plane_position = {0};

	tl::Vec3<float> up_plane_normal = {0};
	tl::Vec3<float> up_plane_position = {0};

	tl::Vec3<float> down_plane_normal = {0};
	tl::Vec3<float> down_plane_position = {0};

	tl::Vec3<float> left_plane_normal = {0};
	tl::Vec3<float> left_plane_position = {0};

	tl::Vec3<float> right_plane_normal = {0};
	tl::Vec3<float> right_plane_position = {0};
};

struct Camera
{
	tl::Vec3<float> position = {0};

	tl::Vec3<float> unit_up = {0};
	tl::Vec3<float> unit_direction = {0};

	float field_of_view_deg = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;

	view_frustrum view_frustrum = {0};
};


void camera_reset(
	const tl::Vec3<float>& position,
	float field_of_view_in_deg,
	float near_plane,
	float far_plane);

const tl::Matrix4x4<float>& get_projection_matrix();

void camera_increment_yaw(float delta_angle_in_deg);

void camera_increment_pitch(float delta_angle_in_deg);

void camera_increment_roll(float delta_angle_in_deg);

void camera_increment_direction(float delta_z);

void camera_increment_strafe(float delta_x);

void camera_increment_up(float delta_up);

void camera_set_fov(float fov_in_deg);

void camera_set_near_plane(float near_plane);

void camera_set_position(const tl::Vec3<float>& position);

void camera_fill_view_matrix(tl::Matrix4x4<float>& view_matrix);

const Camera& camera_get();

#endif

