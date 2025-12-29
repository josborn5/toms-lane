#include "./camera.hpp"
#include <math.h>

static Camera camera;

struct matrix3x3
{
	float element[3][3]; // col then row index (i.e. element[x][y])
};

struct projection_matrix {
	float ax = 0.0f;
	float ay = 0.0f;
};

static void matrix3x3_dot_vect3(const matrix3x3& matrix, const tl::Vec3<float>& input, tl::Vec3<float>& output) {
	output.x = (matrix.element[0][0] * input.x) + (matrix.element[0][1] * input.y) + (matrix.element[0][2] * input.z);
	output.y = (matrix.element[1][0] * input.x) + (matrix.element[1][1] * input.y) + (matrix.element[1][2] * input.z);
	output.z = (matrix.element[2][0] * input.x) + (matrix.element[2][1] * input.y) + (matrix.element[2][2] * input.z);
}



static projection_matrix projection_matrix;
static tl::Matrix4x4<float> view_matrix;

static float deg_to_rad(float degrees) {
	constexpr float pi_over_180 = 3.14159f / 180.0f;
	return degrees * pi_over_180;
}

static void rotate_around_unit_vector(
	const tl::Vec3<float>& unit_vector_axis,
	float angle_in_deg,
	const tl::Vec3<float>& rotate_in,
	tl::Vec3<float>& rotated_out
) {
	// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
	float angle_in_radians = deg_to_rad(angle_in_deg);
	float cos = cosf(angle_in_radians);
	float sin = sinf(angle_in_radians);

	matrix3x3 rotation_matrix;
	rotation_matrix.element[0][0] = (unit_vector_axis.x * unit_vector_axis.x * (1.0f - cos)) + cos;
	rotation_matrix.element[0][1] = (unit_vector_axis.x * unit_vector_axis.y * (1.0f - cos)) - (unit_vector_axis.z * sin);
	rotation_matrix.element[0][2] = (unit_vector_axis.x * unit_vector_axis.z * (1.0f - cos)) + (unit_vector_axis.y * sin);

	rotation_matrix.element[1][0] = (unit_vector_axis.x * unit_vector_axis.y * (1.0f - cos)) + (unit_vector_axis.z * sin);
	rotation_matrix.element[1][1] = (unit_vector_axis.y * unit_vector_axis.y * (1.0f - cos)) + cos;
	rotation_matrix.element[1][2] = (unit_vector_axis.y * unit_vector_axis.z * (1.0f - cos)) - (unit_vector_axis.x * sin);

	rotation_matrix.element[2][0] = (unit_vector_axis.x * unit_vector_axis.z * (1.0f - cos)) - (unit_vector_axis.y * sin);
	rotation_matrix.element[2][1] = (unit_vector_axis.y * unit_vector_axis.z * (1.0f - cos)) + (unit_vector_axis.x * sin);
	rotation_matrix.element[2][2] = (unit_vector_axis.z * unit_vector_axis.z * (1.0f - cos)) + cos;

	matrix3x3_dot_vect3(
		rotation_matrix,
		rotate_in,
		rotated_out
	);
}

/**
* Structure of the projection matrix:
* | A | 0 | 0 | 0 |
* | 0 | B | 0 | 0 |
* | 0 | 0 | C | D |
* | 0 | 0 | 1 | 0 |
*/
// https://www.mauriciopoppe.com/notes/computer-graphics/viewing/projection-transform/
static void set_projection_matrix(const Camera& camera) {

	// r = screen_width
	// l = 0
	// t = screen_height
	// b = 0

	// _nc -> normalized coordinates. normalized to the bounds of the view frustrum -1 -> 1.
	// _screen -> screen coordinates
	// _view -> view coordinates
	//
	// x_screen = x_view * near_plane / z_view
	//
	// x_nc = (2 * x_screen / (r - l)) + ((r + l) / (r - l))
	//
	// substitute for x_screen:
	// x_nc = (2 / (r - l)) * (x_view * near_plane / z_view) - ((r + l) / (r - l))

	// note there's a special simplification we can make since in view space
	// the center of the screen is at x = 0. so r = -l. so (r + l) = 0.
	float r_minus_l = camera.view_space_near_right - camera.view_space_near_left;
	projection_matrix.ax = 2.0f * camera.near_plane / r_minus_l;

	float t_minus_b = camera.view_space_near_top - camera.view_space_near_bottom;
	projection_matrix.ay = 2.0f * camera.near_plane / t_minus_b;
}

static tl::Vec3<float> get_unit_right(const Camera& camera) {
	tl::Vec3<float> camera_right_unit = tl::UnitVector(
		tl::CrossProduct(camera.unit_up, camera.unit_direction) // argument order is important. right unit is in +ve x-axis direction
	);
	return camera_right_unit;
}

/**
* Structure of the PointAt Matrix:
* | Ax | Ay | Az | 0 |
* | Bx | By | Bz | 0 |
* | Cx | Cy | Cz | 0 |
* | Tx | Ty | Tz | 1 |
*/
static void point_at(
	const Camera& camera,
	tl::Matrix4x4<float>& output_matrix
)
{
	tl::Vec3<float> forward_unit = camera.unit_direction;
	tl::Vec3<float> up_unit = camera.unit_up;
	tl::Vec3<float> right_unit = get_unit_right(camera);

	// Construct the new transformation matrix
	output_matrix.m[0][0] = right_unit.x;		output_matrix.m[0][1] = right_unit.y;		output_matrix.m[0][2] = right_unit.z;		output_matrix.m[0][3] = 0.0f;
	output_matrix.m[1][0] = up_unit.x;			output_matrix.m[1][1] = up_unit.y;			output_matrix.m[1][2] = up_unit.z;			output_matrix.m[1][3] = 0.0f;
	output_matrix.m[2][0] = forward_unit.x;		output_matrix.m[2][1] = forward_unit.y;		output_matrix.m[2][2] = forward_unit.z;		output_matrix.m[2][3] = 0.0f;
	output_matrix.m[3][0] = camera.position.x;	output_matrix.m[3][1] = camera.position.y;	output_matrix.m[3][2] = camera.position.z;	output_matrix.m[3][3] = 1.0f;
}

/**
* Structure of the LookAt Matrix:
* |  Ax  |  Bx  |  Cx  | 0 |
* |  Ay  |  By  |  Cy  | 0 |
* |  Az  |  Bz  |  Cz  | 0 |
* | -T.A | -T.B | -T.C | 1 |
*/
static void look_at(const tl::Matrix4x4<float>& pointAt, tl::Matrix4x4<float>& lookAt)
{
	float tDotA = (pointAt.m[3][0] * pointAt.m[0][0]) + (pointAt.m[3][1] * pointAt.m[0][1]) + (pointAt.m[3][2] * pointAt.m[0][2]);
	float tDotB = (pointAt.m[3][0] * pointAt.m[1][0]) + (pointAt.m[3][1] * pointAt.m[1][1]) + (pointAt.m[3][2] * pointAt.m[1][2]);
	float tDotC = (pointAt.m[3][0] * pointAt.m[2][0]) + (pointAt.m[3][1] * pointAt.m[2][1]) + (pointAt.m[3][2] * pointAt.m[2][2]);

	lookAt.m[0][0] = pointAt.m[0][0];	lookAt.m[0][1] = pointAt.m[1][0];	lookAt.m[0][2] = pointAt.m[2][0];	lookAt.m[0][3] = 0.0f;
	lookAt.m[1][0] = pointAt.m[0][1];	lookAt.m[1][1] = pointAt.m[1][1];	lookAt.m[1][2] = pointAt.m[2][1];	lookAt.m[1][3] = 0.0f;
	lookAt.m[2][0] = pointAt.m[0][2];	lookAt.m[2][1] = pointAt.m[1][2];	lookAt.m[2][2] = pointAt.m[2][2];	lookAt.m[2][3] = 0.0f;
	lookAt.m[3][0] = -tDotA;			lookAt.m[3][1] = -tDotB;			lookAt.m[3][2] = -tDotC;			lookAt.m[3][3] = 1.0f;
}

static void set_view_frustrum() {
	tl::Vec3<float> camera_right_unit = get_unit_right(camera);

	float tan_half_fov = tanf(deg_to_rad(0.5f * camera.field_of_view_deg));

	tl::Vec3<float> near_plane_center_from_position = MultiplyVectorByScalar(
		camera.unit_direction,
		camera.near_plane);
	tl::Vec3<float> near_plane_center_position = tl::AddVectors(
		camera.position,
		near_plane_center_from_position);

	float near_opp_horizontal = camera.near_plane * tan_half_fov;
	float near_opp_vertical = near_opp_horizontal / camera.aspect_ratio;

	tl::Vec3<float> near_up_center_position = tl::AddVectors(
		near_plane_center_position,
		tl::MultiplyVectorByScalar(camera.unit_up, near_opp_vertical)
	);
	camera.view_frustrum.near_top_left_corner_position = tl::AddVectors(
		near_up_center_position,
		tl::MultiplyVectorByScalar(camera_right_unit, -near_opp_horizontal)
	);


	camera.view_space_near_left = -near_opp_horizontal;
	camera.view_space_near_right = near_opp_horizontal;
	camera.view_space_near_top = near_opp_vertical;
	camera.view_space_near_bottom = -near_opp_vertical;

	tl::Vec3<float> far_plane_center_from_position = MultiplyVectorByScalar(
		camera.unit_direction,
		camera.far_plane);
	tl::Vec3<float> far_plane_center_position = tl::AddVectors(
		camera.position,
		far_plane_center_from_position);

	float far_opp_horizontal = camera.far_plane * tan_half_fov;
	float far_opp_vertical = far_opp_horizontal / camera.aspect_ratio;

	// calculate positions for the four 'pyramid' sides of the frustrum
	tl::Vec3<float> far_bottom_center_position = tl::AddVectors(
		far_plane_center_position,
		tl::MultiplyVectorByScalar(camera.unit_up, -far_opp_vertical)
	);
	camera.view_frustrum.far_bottom_right_corner_position = tl::AddVectors(
		far_bottom_center_position,
		tl::MultiplyVectorByScalar(camera_right_unit, far_opp_horizontal)
	);

	// calculate normals for the four 'pyramid' sides of the frustrum
	camera.view_frustrum.near_plane_normal = camera.unit_direction;
	camera.view_frustrum.far_plane_normal = tl::Vec3<float>{ -camera.unit_direction.x, -camera.unit_direction.y, -camera.unit_direction.z };

	camera.view_frustrum.up_plane_normal = tl::Vec3<float> {
		-camera.unit_up.x,
		-camera.unit_up.y,
		-camera.unit_up.z
	};
	rotate_around_unit_vector(
		camera_right_unit,
		-0.5f * camera.field_of_view_deg,
		camera.view_frustrum.up_plane_normal,
		camera.view_frustrum.up_plane_normal
	);

	camera.view_frustrum.down_plane_normal = camera.unit_up;
	rotate_around_unit_vector(
		camera_right_unit,
		0.5f * camera.field_of_view_deg,
		camera.view_frustrum.down_plane_normal,
		camera.view_frustrum.down_plane_normal
	);

	camera.view_frustrum.left_plane_normal = camera_right_unit;
	rotate_around_unit_vector(
		camera.unit_up,
		-0.5f * camera.field_of_view_deg,
		camera.view_frustrum.left_plane_normal,
		camera.view_frustrum.left_plane_normal
	);

	camera.view_frustrum.right_plane_normal = tl::MultiplyVectorByScalar(camera_right_unit, -1.0f);
	rotate_around_unit_vector(
		camera.unit_up,
		0.5f * camera.field_of_view_deg,
		camera.view_frustrum.right_plane_normal,
		camera.view_frustrum.right_plane_normal
	);

	// update the view matrix
	tl::Matrix4x4<float> camera_matrix;
	point_at(
		camera,
		camera_matrix
	);
	look_at(camera_matrix, view_matrix);
}

static void camera_set_fov(Camera& camera, float fov_in_deg) {
	camera.field_of_view_deg = fov_in_deg;

	if (camera.field_of_view_deg < 1.0f) {
		camera.field_of_view_deg = 1.0f;
	}
	else if (camera.field_of_view_deg > 179.0f) {
		camera.field_of_view_deg = 179.0f;
	}
}

static void camera_set_near_plane(Camera& camera, float near_plane) {
	camera.near_plane = near_plane;
	if (camera.near_plane < 0.0f) {
		camera.near_plane = 0.0f;
	}
	else if (camera.far_plane < camera.near_plane) {
		camera.near_plane = camera.far_plane;
	}
}

void camera_reset(
	float aspect_ratio,
	const tl::Vec3<float>& position,
	float field_of_view_in_deg,
	float near_plane,
	float far_plane,
	const tl::Vec3<float> unit_direction,
	const tl::Vec3<float> unit_up
) {
	camera.aspect_ratio = aspect_ratio;

	camera.unit_direction = unit_direction;
	camera.unit_up = unit_up;

	camera.position = position;

	camera_set_fov(camera, field_of_view_in_deg);
	camera.far_plane = far_plane;
	camera_set_near_plane(camera, near_plane);

	set_projection_matrix(camera);
	set_view_frustrum();
}

void camera_increment_yaw(float delta_angle_in_deg) {
	rotate_around_unit_vector(
		camera.unit_up,
		delta_angle_in_deg,
		camera.unit_direction,
		camera.unit_direction
	);

	rotate_around_unit_vector(
		camera.unit_up,
		delta_angle_in_deg,
		camera.unit_up,
		camera.unit_up
	);

	set_view_frustrum();
}

void camera_increment_pitch(float delta_angle_in_deg) {
	tl::Vec3<float> right_unit = get_unit_right(camera);

	rotate_around_unit_vector(
		right_unit,
		delta_angle_in_deg,
		camera.unit_direction,
		camera.unit_direction
	);

	rotate_around_unit_vector(
		right_unit,
		delta_angle_in_deg,
		camera.unit_up,
		camera.unit_up
	);

	set_view_frustrum();
}


void camera_increment_roll(float delta_angle_in_deg) {
	rotate_around_unit_vector(
		camera.unit_direction,
		delta_angle_in_deg,
		camera.unit_direction,
		camera.unit_direction
	);

	rotate_around_unit_vector(
		camera.unit_direction,
		delta_angle_in_deg,
		camera.unit_up,
		camera.unit_up
	);

	set_view_frustrum();
}

void camera_increment_direction(float delta_z) {
	tl::Vec3<float> cameraPositionForwardBack = MultiplyVectorByScalar(
		camera.unit_direction,
		delta_z);

	camera.position = AddVectors(
		camera.position,
		cameraPositionForwardBack
	);
	set_view_frustrum();
}

void camera_increment_strafe(float delta_x) {
	// Strafing - use the cross product between the camera direction and up to get a normal vector to the direction being faced
	tl::Vec3<float> right_unit = get_unit_right(camera);

	tl::Vec3<float> cameraPositionStrafe = MultiplyVectorByScalar(
		right_unit,
		delta_x);

	camera.position = AddVectors(
		camera.position,
		cameraPositionStrafe
	);
	set_view_frustrum();
}

void camera_increment_up(float delta_up) {
	tl::Vec3<float> camera_position_delta = MultiplyVectorByScalar(
		camera.unit_up,
		delta_up);

	camera.position = AddVectors(
		camera.position,
		camera_position_delta
	);
	set_view_frustrum();
}

const Camera& camera_get() {
	return camera;
}

void camera_project_triangle(
	float screen_width,
	const tl::Vec3<float>& in_p0,
	const tl::Vec3<float>& in_p1,
	const tl::Vec3<float>& in_p2,
	tl::Vec3<float>& out_p0,
	tl::Vec3<float>& out_p1,
	tl::Vec3<float>& out_p2
) {
	tl::Vec4<float> viewed_p0, viewed_p1, viewed_p2;
	tl::Vec4<float> projected_p0, projected_p1, projected_p2;

	MultiplyVectorWithMatrix({ in_p0.x, in_p0.y, in_p0.z, 1.0f }, viewed_p0, view_matrix);
	MultiplyVectorWithMatrix({ in_p1.x, in_p1.y, in_p1.z, 1.0f }, viewed_p1, view_matrix);
	MultiplyVectorWithMatrix({ in_p2.x, in_p2.y, in_p2.z, 1.0f }, viewed_p2, view_matrix);

	// _nc -> normalized coordinates. normalized to the bounds of the view frustrum -1 -> 1.
	// _screen -> screen coordinates
	// _view -> view coordinates
	//
	// x_screen = x_view * near_plane / z_view
	//
	// x_nc = (2 * x_screen / (r - l)) + ((r + l) / (r - l))
	//
	// substitute for x_screen:
	// x_nc = (2 / (r - l)) * (x_view * near_plane / z_view) - ((r + l) / (r - l))

	// note there's a special simplification we can make since in view space
	// the center of the screen is at x = 0. so r = -l. so (r + l) = 0.

	// projected_p is normalized to the view frustrum space.
	// i.e.
	// projected_pn.x is in the range -1 -> 1 for points inside the view frustrum
	// projected_pn.y is in the range -1 -> 1 for points inside the view frustrum
	// projected_pn.z is in the range -1 -> 1 for points inside the view frustrum
	projected_p0.x = projection_matrix.ax * (viewed_p0.x / viewed_p0.z);
	projected_p1.x = projection_matrix.ax * (viewed_p1.x / viewed_p1.z);
	projected_p2.x = projection_matrix.ax * (viewed_p2.x / viewed_p2.z);

	projected_p0.y = projection_matrix.ay * (viewed_p0.y / viewed_p0.z);
	projected_p1.y = projection_matrix.ay * (viewed_p1.y / viewed_p1.z);
	projected_p2.y = projection_matrix.ay * (viewed_p2.y / viewed_p2.z);

	// need to map normalized screen space to pixel screen space
	// i.e.
	// x: -1 -> 0, 1 -> screen_width
	// y: -1 -> 0, 1 -> screen_height

	float half_screen_width = 0.5f * (float)screen_width;
	out_p0.x = half_screen_width * projected_p0.x + half_screen_width;
	out_p1.x = half_screen_width * projected_p1.x + half_screen_width;
	out_p2.x = half_screen_width * projected_p2.x + half_screen_width;

	float half_screen_height = half_screen_width / camera.aspect_ratio;
	out_p0.y = half_screen_height * projected_p0.y + half_screen_height;
	out_p1.y = half_screen_height * projected_p1.y + half_screen_height;
	out_p2.y = half_screen_height * projected_p2.y + half_screen_height;

	// for now just output z value in view space.
	// might want to normalize it in the future? if it will help with clipping?
	out_p0.z = viewed_p0.z;
	out_p1.z = viewed_p1.z;
	out_p2.z = viewed_p2.z;
}

