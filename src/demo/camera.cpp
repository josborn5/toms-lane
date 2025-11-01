#include "./camera.hpp"
#include <math.h>

const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;
constexpr float aspect_ratio = (float)screen_width / (float)screen_height;

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

static void update_camera_direction(Camera& camera) {
	float yaw_in_radians = deg_to_rad(-camera.yaw);
	float cos = cosf(yaw_in_radians);
	float sin = sinf(yaw_in_radians);

	matrix3x3 y_axis_rotation_matrix;
	y_axis_rotation_matrix.element[0][0] = cos;
	y_axis_rotation_matrix.element[0][1] = 0.0f;
	y_axis_rotation_matrix.element[0][2] = sin;
	y_axis_rotation_matrix.element[1][0] = 0.0f;
	y_axis_rotation_matrix.element[1][1] = 1.0f;
	y_axis_rotation_matrix.element[1][2] = 0.0f;
	y_axis_rotation_matrix.element[2][0] = -sin;
	y_axis_rotation_matrix.element[2][1] = 0.0f;
	y_axis_rotation_matrix.element[2][2] = cos;

	matrix3x3_dot_vect3(
		y_axis_rotation_matrix,
		tl::Vec3<float>{ 0.0f, 0.0f, 1.0f },
		camera.direction
	);

	camera.unit_direction = tl::UnitVector(camera.direction);
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

void set_projection_matrix(const Camera& camera) {
	projectionMatrix = MakeProjectionMatrix(camera.field_of_view_deg, aspect_ratio, camera.near_plane, camera.far_plane);
}

void reset_camera(const cuboid& world, Camera& camera) {
	camera.up = { 0.0f, 1.0f, 0.0f };
	// Start position is centered in x & y directions and stepped back in the z direction.
	camera.position = {
		world.position.x,
		world.position.y,
		world.position.z - world.half_size.z
	};
	camera.yaw = 0.0f;
	update_camera_direction(camera);
	camera.field_of_view_deg = 75.0f;

	// camera         near   object    far
	// 	 |             |    |------|    |
	//   |-------------|----------------|----> z
	//   0
	camera.far_plane = 1.0f * world.half_size.z;
	camera.near_plane = 0.1f * camera.far_plane;

	set_projection_matrix(camera);
}

const tl::Matrix4x4<float>& get_projection_matrix() {
	return projectionMatrix;
}

static const float yaw_increment_in_degrees = 0.5f;
void increment_camera_yaw(Camera& camera) {
	camera.yaw += yaw_increment_in_degrees;
	if (camera.yaw > 360.0f) {
		camera.yaw -= 360.0f;
	}
	update_camera_direction(camera);
}

void decrement_camera_yaw(Camera& camera) {
	camera.yaw -= yaw_increment_in_degrees;
	if (camera.yaw < 0.0f) {
		camera.yaw += 360.0f;
	}
	update_camera_direction(camera);
}

void move_camera_z(float delta_z, Camera& camera) {
	tl::Vec3<float> cameraPositionForwardBack = MultiplyVectorByScalar(
		camera.direction,
		delta_z);

	camera.position = AddVectors(
		camera.position,
		cameraPositionForwardBack
	);
}

