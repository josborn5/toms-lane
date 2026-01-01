#ifndef DEMO_CAMERA
#define DEMO_CAMERA

#include "../../src/tl-library.hpp"

struct view_frustrum {
	tl::Vec3<float> near_top_left_corner_position = {0};
	tl::Vec3<float> far_bottom_right_corner_position = {0};


	tl::Vec3<float> near_plane_normal = {0};
	tl::Vec3<float> far_plane_normal = {0};
	tl::Vec3<float> up_plane_normal = {0};
	tl::Vec3<float> down_plane_normal = {0};
	tl::Vec3<float> left_plane_normal = {0};
	tl::Vec3<float> right_plane_normal = {0};
};

struct Camera
{
	tl::Vec3<float> position = {0};

	tl::Vec3<float> unit_up = {0};
	tl::Vec3<float> unit_direction = {0};

	float view_space_near_left;
	float view_space_near_right;
	float view_space_near_top;
	float view_space_near_bottom;

	float field_of_view_deg = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
	float aspect_ratio = 1.0f;

	unsigned int screen_width = 0;
	unsigned int screen_height = 0;

	view_frustrum view_frustrum = {0};
};

void camera_reset(
	unsigned int screen_width,
	unsigned int screen_height,
	const tl::Vec3<float>& position,
	float field_of_view_in_deg,
	float near_plane,
	float far_plane,
	const tl::Vec3<float> unit_direction,
	const tl::Vec3<float> unit_up);

void camera_increment_yaw(float delta_angle_in_deg);

void camera_increment_pitch(float delta_angle_in_deg);

void camera_increment_roll(float delta_angle_in_deg);

void camera_increment_direction(float delta_z);

void camera_increment_strafe(float delta_x);

void camera_increment_up(float delta_up);

const Camera& camera_get();

void camera_project_triangle(
	const tl::Vec3<float>& in_p0,
	const tl::Vec3<float>& in_p1,
	const tl::Vec3<float>& in_p2,
	tl::Vec3<float>& out_p0,
	tl::Vec3<float>& out_p1,
	tl::Vec3<float>& out_p2
);

#endif

