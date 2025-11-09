#include "./camera.hpp"
#include <math.h>

const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;
constexpr float aspect_ratio = (float)screen_width / (float)screen_height;

static Camera camera;

struct matrix3x3
{
	float element[3][3]; // row then col index
};

static void matrix3x3_dot_vect3(const matrix3x3& matrix, const tl::Vec3<float>& input, tl::Vec3<float>& output) {
	output.x = (matrix.element[0][0] * input.x) + (matrix.element[0][1] * input.y) + (matrix.element[0][2] * input.z);
	output.y = (matrix.element[1][0] * input.x) + (matrix.element[1][1] * input.y) + (matrix.element[1][2] * input.z);
	output.z = (matrix.element[2][0] * input.x) + (matrix.element[2][1] * input.y) + (matrix.element[2][2] * input.z);
}



static tl::Matrix4x4<float> projectionMatrix;


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


static void rotate_around_x_axis(const tl::Vec3<float>& in, float angle_in_deg, tl::Vec3<float>& out) {
	float pitch_in_radians = deg_to_rad(angle_in_deg);
	float cos = cosf(pitch_in_radians);
	float sin = sinf(pitch_in_radians);

	matrix3x3 x_axis_rotation_matrix;
	x_axis_rotation_matrix.element[0][0] = 1.0f;
	x_axis_rotation_matrix.element[0][1] = 0.0f;
	x_axis_rotation_matrix.element[0][2] = 0.0f;
	x_axis_rotation_matrix.element[1][0] = 0.0f;
	x_axis_rotation_matrix.element[1][1] = cos;
	x_axis_rotation_matrix.element[1][2] = -sin;
	x_axis_rotation_matrix.element[2][0] = 0.0f;
	x_axis_rotation_matrix.element[2][1] = sin;
	x_axis_rotation_matrix.element[2][2] = cos;

	matrix3x3_dot_vect3(
		x_axis_rotation_matrix,
		in,
		out
	);
}


static tl::Matrix4x4<float> MakeProjectionMatrix(
	float fieldOfVewDeg,
	float aspectRatio,
	float nearPlane,
	float farPlane
)
{
	float inverseTangent = 1.0f / tanf(deg_to_rad(0.5f * fieldOfVewDeg));

	tl::Matrix4x4<float> matrix;
	matrix.m[0][0] = inverseTangent;
	matrix.m[1][1] = aspectRatio * inverseTangent;
	matrix.m[2][2] = farPlane / (farPlane - nearPlane);
	matrix.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;

	return matrix;
}

static void set_projection_matrix(const Camera& camera) {
	projectionMatrix = MakeProjectionMatrix(camera.field_of_view_deg, aspect_ratio, camera.near_plane, camera.far_plane);
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
	tl::Vec3<float> up_unit = tl::UnitVector(camera.up);
	tl::Vec3<float> right_unit = tl::CrossProduct(up_unit, forward_unit);

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

static void get_camera_near_plane_position(const Camera& camera, tl::Vec3<float>& position) {
	tl::Vec3<float> near_plane_center_from_position = MultiplyVectorByScalar(
		camera.unit_direction,
		camera.near_plane);
	position = tl::AddVectors(
		camera.position,
		near_plane_center_from_position);
}

static void get_camera_far_plane_position(const Camera& camera, tl::Vec3<float>& position) {
	tl::Vec3<float> near_plane_center_from_position = MultiplyVectorByScalar(
		camera.unit_direction,
		camera.far_plane);
	position = tl::AddVectors(
		camera.position,
		near_plane_center_from_position);
}



static void set_view_frustrum() {
	get_camera_near_plane_position(camera, camera.view_frustrum.near_plane_position);
	camera.view_frustrum.near_plane_normal = camera.unit_direction;

	tl::Vec3<float> far_plane_position;
	get_camera_far_plane_position(camera, camera.view_frustrum.far_plane_position);
	camera.view_frustrum.far_plane_normal = tl::Vec3<float>{ -camera.unit_direction.x, -camera.unit_direction.y, -camera.unit_direction.z };

	tl::Vec3<float> unit_up = tl::UnitVector(camera.up);

	float tan_half_fov = tanf(deg_to_rad(0.5f * camera.field_of_view_deg));
	float near_opp = camera.near_plane * tan_half_fov;
	float far_opp = camera.far_plane * tan_half_fov;

	camera.view_frustrum.up_plane_position = tl::AddVectors(
		camera.view_frustrum.near_plane_position,
		tl::MultiplyVectorByScalar(unit_up, near_opp)
	);

	tl::Vec3<float> camera_right = CrossProduct(
		camera.up,
		camera.unit_direction
	);

	camera.view_frustrum.up_plane_normal = camera.up;
	rotate_around_x_axis(
		camera_right,
		0.5f * camera.field_of_view_deg,
		camera.view_frustrum.up_plane_normal
	);

	camera.view_frustrum.down_plane_position = tl::AddVectors(
		camera.view_frustrum.near_plane_position,
		tl::MultiplyVectorByScalar(unit_up, -near_opp)
	);

	camera.view_frustrum.down_plane_normal = tl::Vec3<float> {
		-camera.up.x,
		-camera.up.y,
		-camera.up.z
	};
	rotate_around_x_axis(
		camera_right,
		-0.5f * camera.field_of_view_deg,
		camera.view_frustrum.down_plane_normal
	);

}


void camera_reset(
	const tl::Vec3<float>& position,
	float field_of_view_in_deg,
	float near_plane,
	float far_plane
) {
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.position = position;

	camera.unit_direction = { 0.0f, 0.0f, 1.0f };

	camera.field_of_view_deg = field_of_view_in_deg;
	camera.near_plane = near_plane;
	camera.far_plane = far_plane;

	set_projection_matrix(camera);
	set_view_frustrum();
}

const tl::Matrix4x4<float>& get_projection_matrix() {
	return projectionMatrix;
}

void camera_increment_yaw(float delta_angle_in_deg) {
	rotate_around_unit_vector(
		tl::UnitVector(camera.up),
		delta_angle_in_deg,
		camera.unit_direction,
		camera.unit_direction
	);

	rotate_around_unit_vector(
		tl::UnitVector(camera.up),
		delta_angle_in_deg,
		camera.up,
		camera.up
	);

	set_view_frustrum();
}

void camera_increment_pitch(float delta_angle_in_deg) {
	tl::Vec3<float> forward_unit = camera.unit_direction;
	tl::Vec3<float> up_unit = tl::UnitVector(camera.up);
	tl::Vec3<float> right_unit = tl::CrossProduct(up_unit, forward_unit);
	
	rotate_around_unit_vector(
		right_unit,
		delta_angle_in_deg,
		camera.unit_direction,
		camera.unit_direction
	);

	rotate_around_unit_vector(
		right_unit,
		delta_angle_in_deg,
		camera.up,
		camera.up
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
	tl::Vec3<float> rawCameraPositionStrafe = CrossProduct(
		camera.up,
		camera.unit_direction
	);
	tl::Vec3<float> cameraPositionStrafe = MultiplyVectorByScalar(
		tl::UnitVector(rawCameraPositionStrafe),
		delta_x);

	camera.position = AddVectors(
		camera.position,
		cameraPositionStrafe
	);
	set_view_frustrum();
}

void camera_increment_up(float delta_up) {
	tl::Vec3<float> camera_position_delta = MultiplyVectorByScalar(
		camera.up,
		delta_up);

	camera.position = AddVectors(
		camera.position,
		camera_position_delta
	);
	set_view_frustrum();
}


void camera_set_fov(float fov_in_deg) {
	camera.field_of_view_deg = fov_in_deg;

	if (camera.field_of_view_deg < 1.0f) {
		camera.field_of_view_deg = 1.0f;
	}
	else if (camera.field_of_view_deg > 179.0f) {
		camera.field_of_view_deg = 179.0f;
	}

	set_projection_matrix(camera);
	set_view_frustrum();
}

void camera_set_near_plane(float near_plane) {
	camera.near_plane = near_plane;
	if (camera.near_plane < 0.0f) {
		camera.near_plane = 0.0f;
	}
	else if (camera.far_plane < camera.near_plane) {
		camera.near_plane = camera.far_plane;
	}

	set_projection_matrix(camera);
	set_view_frustrum();
}

void camera_set_position(const tl::Vec3<float>& position) {
	camera.position = position;
	set_view_frustrum();
}


void camera_fill_view_matrix(tl::Matrix4x4<float>& view_matrix) {
	tl::Matrix4x4<float> camera_matrix;
	point_at(
		camera,
		camera_matrix
	);

	look_at(camera_matrix, view_matrix);
}

const Camera& camera_get() {
	return camera;
}

