#include <strstream>
#include <string>
#include <limits>
#include <math.h>
#include "../../application/src/tl-application.hpp"
#include "../../platform/src/tl-library.hpp"
#include "./render.hpp"
#include "./file.hpp"
#include "./camera.hpp"
#include "./math.hpp"
#include "./demo.hpp"

struct cuboid {
	tl::Vec3<float> position = {0};
	tl::Vec3<float> half_size = {0};
};

static const unsigned int screen_width = 1280;
static const unsigned int screen_height = 720;
static unsigned int actual_height = 0;
static unsigned int actual_width = 0;
static constexpr unsigned int pixel_count = screen_width * screen_height;
static z_buffer depth_buffer;
static float depth_array[pixel_count] = { 0 };

static bool wireframe = false;
static bool is_teapot = true;

static cuboid world;
static cuboid mesh;

static Plane top_clipping_plane;

static tl::array<Triangle4d> meshArray = tl::array<Triangle4d>();

static float positionIncrement = 0.0f;

static tl::Rect<float> map;
static float world_to_map_scale_factor = 0.0f;
static tl::Matrix2x3<float> mapProjectionMatrix;

static tl::MemorySpace persistent_memory;
static tl::MemorySpace transient_memory;

static bool isStarted = false;

static unsigned int rendered_triangle_count = 0;
static unsigned int projected_triangle_count = 0;
static unsigned int viewed_triangle_count = 0;

static void depth_buffer_clear() {
	for (unsigned int i = 0; i < pixel_count; i += 1) {
		// smaller z is closer
		depth_array[i] = std::numeric_limits<float>::max();
	}
}

static float deg_to_rad(float degrees) {
	constexpr float pi_over_180 = 3.14159f / 180.0f;
	return degrees * pi_over_180;
}


static void get_camera_plane_map_coords(tl::Vec2<float>& near_1, tl::Vec2<float>& near_2, tl::Vec2<float>& far_1, tl::Vec2<float>& far_2) {
	const Camera& camera = camera_get();
	tl::Vec3<float> unit_right_to_direction = tl::UnitVector(
		tl::CrossProduct(
			camera.unit_up,
			camera.unit_direction
		)
	);
	float tan_half_fov = tanf(deg_to_rad(0.5f * camera.field_of_view_deg));

	float near_opp = camera.near_plane * tan_half_fov;
	tl::Vec3<float> near_plane_left = camera.view_frustrum.near_top_left_corner_position;
	tl::Vec3<float> near_plane_right_from_left = MultiplyVectorByScalar(unit_right_to_direction, (2.0f * near_opp));
	tl::Vec3<float> near_plane_right = tl::AddVectors(near_plane_left, near_plane_right_from_left);

	float far_opp = camera.far_plane * tan_half_fov;
	tl::Vec3<float> far_plane_right = camera.view_frustrum.far_bottom_right_corner_position;
	tl::Vec3<float> far_plane_left_from_right = MultiplyVectorByScalar(unit_right_to_direction, (-2.0f * far_opp));
	tl::Vec3<float> far_plane_left = tl::AddVectors(far_plane_right, far_plane_left_from_right);

	near_1 = Transform2DVector(tl::Vec2<float>{ near_plane_left.z, near_plane_left.x }, mapProjectionMatrix);
	near_2 = Transform2DVector(tl::Vec2<float>{ near_plane_right.z, near_plane_right.x }, mapProjectionMatrix);
	far_1 = Transform2DVector(tl::Vec2<float>{ far_plane_left.z, far_plane_left.x }, mapProjectionMatrix);
	far_2 = Transform2DVector(tl::Vec2<float>{ far_plane_right.z, far_plane_right.x }, mapProjectionMatrix);
}



static int compare_triangle_depth(const void* a, const void* b) {
	float a_depth_sum = ((Triangle4d*)a)->p[0].z + ((Triangle4d*)a)->p[1].z + ((Triangle4d*)a)->p[2].z;
	float b_depth_sum = ((Triangle4d*)b)->p[0].z + ((Triangle4d*)b)->p[1].z + ((Triangle4d*)b)->p[2].z;

	if (a_depth_sum > b_depth_sum) {
		return -1;
	}

	if (a_depth_sum < b_depth_sum) {
		return 1;
	}

	return 0;
}


static void TransformAndRenderMesh(
	const tl::RenderBuffer& renderBuffer,
	const tl::array<Triangle4d>& mesh,
	const Camera& camera,
	const tl::MemorySpace& transient
) {
	rendered_triangle_count = 0;
	projected_triangle_count = 0;
	viewed_triangle_count = 0;

	depth_buffer_clear();

	tl::array<Triangle4d> trianglesToDrawArray = tl::array<Triangle4d>(transient);

	for (int h = 0; h < mesh.length(); h += 1)
	{
		Triangle4d tri = mesh.get(h);

		// Skip any triangles angled away from the camera
		tl::Vec3<float> triangle_edge_1 = SubtractVectors(
			tl::Vec3<float>{ tri.p[1].x, tri.p[1].y, tri.p[1].z },
			tl::Vec3<float>{ tri.p[0].x, tri.p[0].y, tri.p[0].z }
		);
		tl::Vec3<float> triangle_edge_2 = SubtractVectors(
			tl::Vec3<float>{ tri.p[2].x, tri.p[2].y, tri.p[2].z },
			tl::Vec3<float>{ tri.p[0].x, tri.p[0].y, tri.p[0].z }
		);
		tl::Vec3<float> unit_triangle_normal = UnitVector(
			CrossProduct(triangle_edge_1, triangle_edge_2)
		);

		tl::Vec3<float> camera_to_triangle = SubtractVectors(
			tl::Vec3<float>{ tri.p[0].x, tri.p[0].y, tri.p[0].z },
			camera.position
		);
		float dot = tl::DotProduct(unit_triangle_normal, camera_to_triangle);
		if (dot > 0.0f)
		{
			continue;
		}

		const tl::Vec3<float> inverse_unit_light_direction = { 0.0f, 0.0f, -1.0f };
		// range of triangle_face_to_light is from +1 (directly facing the light) to -1 (facing directly away from the light)
		float triangle_face_to_light = tl::DotProduct(unit_triangle_normal, inverse_unit_light_direction);
		float normalized_triangle_face_to_light = 0.5f * (triangle_face_to_light + 1.0f);
		float shade = 0.2f + (0.8f * normalized_triangle_face_to_light);

		uint32_t triangleColor = (uint32_t)((float)tri.color * shade);

		Plane near_plane;
		near_plane.position = camera.view_frustrum.near_top_left_corner_position;
		near_plane.normal = camera.view_frustrum.near_plane_normal;

		Plane far_plane;
		far_plane.position = camera.view_frustrum.far_bottom_right_corner_position;
		far_plane.normal = camera.view_frustrum.far_plane_normal;

		Plane top_plane;
		top_plane.position = camera.position;
		top_plane.normal = camera.view_frustrum.up_plane_normal;

		Plane bottom_plane;
		bottom_plane.position = camera.position;
		bottom_plane.normal = camera.view_frustrum.down_plane_normal;

		Plane left_plane;
		left_plane.position = camera.position;
		left_plane.normal = camera.view_frustrum.left_plane_normal;

		Plane right_plane;
		right_plane.position = camera.position;
		right_plane.normal = camera.view_frustrum.right_plane_normal;

		const int plane_clip_count = 6;
		constexpr int triangle_queue_size = plane_clip_count * 6; // 3 * 2 sides of view frustrum to clip
		Triangle4d queue_data[triangle_queue_size];
		tl::MemorySpace queue_data_space;
		queue_data_space.content = queue_data;
		queue_data_space.sizeInBytes = triangle_queue_size * sizeof(Triangle4d);
		tl::queue<Triangle4d> triangle_queue = tl::queue<Triangle4d>(queue_data_space);
		triangle_queue.enqueue(tri);

		Triangle4d clipped[2];
		int new_triangles = 1;

		// TODO: take a rect that fills the rendered screen at the near plane z-position
		// reverse calculate the projection then view transformation of that rect so it
		// is in world space. then use that as a clipping surface for top/bottom/left/right

		for (int plane_index = 0; plane_index < plane_clip_count; plane_index += 1)
		{
			int triangles_to_add = 0;
			while (new_triangles > 0)
			{
				new_triangles -= 1;
				tl::operation<Triangle4d> dequeue_op = triangle_queue.dequeue();
				if (dequeue_op.result != 0)
				{
					return;
				}
				Triangle4d to_clip = dequeue_op.value;

				switch (plane_index)
				{
					case 0:
					{
						triangles_to_add = ClipTriangleAgainstPlane(near_plane, to_clip, clipped[0], clipped[1]);
						break;
					}
					case 1:
					{
						triangles_to_add = ClipTriangleAgainstPlane(far_plane, to_clip, clipped[0], clipped[1]);
						break;
					}
					case 2: {
						triangles_to_add = ClipTriangleAgainstPlane(top_plane, to_clip, clipped[0], clipped[1]);
					} break;
					case 3: {
						triangles_to_add = ClipTriangleAgainstPlane(bottom_plane, to_clip, clipped[0], clipped[1]);
					} break;
					case 4: {
						triangles_to_add = ClipTriangleAgainstPlane(left_plane, to_clip, clipped[0], clipped[1]);
					} break;
					case 5: {
						triangles_to_add = ClipTriangleAgainstPlane(right_plane, to_clip, clipped[0], clipped[1]);
					} break;
				}

				for (int i = 0; i < triangles_to_add; i += 1)
				{
					int enqueue_result = triangle_queue.enqueue(clipped[i]);
					if (enqueue_result != 0)
					{
						return;
					}
				}
			}

			new_triangles = triangle_queue.length();
		}

		for (int i = 0; i < triangle_queue.length(); i += 1)
		{
			Triangle4d to_transform = triangle_queue.content[i];

			viewed_triangle_count += 1;
			projected_triangle_count += 1;

			Triangle4d triToRender;
			camera_project_triangle(
				to_transform.p[0],
				to_transform.p[1],
				to_transform.p[2],
				triToRender.p[0],
				triToRender.p[1],
				triToRender.p[2]
			);
			triToRender.color = triangleColor;

			trianglesToDrawArray.append(triToRender);
		}
	}

	for (int n = 0; n < trianglesToDrawArray.length(); n += 1)
	{
		rendered_triangle_count += 1;
		Triangle4d draw = trianglesToDrawArray.get(n);

		if (wireframe) {
			tl::Vec2<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
			tl::Vec2<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
			tl::Vec2<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };
			DrawTriangleInPixels(renderBuffer, 0xFFFFFF, p0Int, p1Int, p2Int);
		} else {
			tl::Vec3<float> p0 = { draw.p[0].x, draw.p[0].y, draw.p[0].z };
			tl::Vec3<float> p1 = { draw.p[1].x, draw.p[1].y, draw.p[1].z };
			tl::Vec3<float> p2 = { draw.p[2].x, draw.p[2].y, draw.p[2].z };
			triangle_fill(renderBuffer, depth_buffer, draw.color, p0, p1, p2);
		}
	}
}

static tl::Matrix2x3<float> GenerateProjectionMatrix(const tl::Rect<float> &from, const tl::Rect<float> &to)
{
	float toMaxY = to.y_max();
	float toMinY = to.y_min();

	float toMaxX = to.x_max();
	float toMinX = to.x_min();

	float fromMaxY = from.y_max();
	float fromMinY = from.y_min();

	float fromMaxX = from.x_max();
	float fromMinX = from.x_min();

	float toYGrad = (toMinY - toMaxY) / (fromMinY - fromMaxY);
	float toYOffset = toMaxY - (toYGrad * fromMaxY);

	float toXGrad = (toMinX - toMaxX) / (fromMinX - fromMaxX);
	float toXOffset = toMaxX - (toXGrad * fromMaxX);

	tl::Matrix2x3<float> transformMatrix = tl::Matrix2x3<float>();
	transformMatrix.m[0][0] = toXGrad;
	transformMatrix.m[0][2] = toXOffset;
	transformMatrix.m[1][1] = -toYGrad;
	transformMatrix.m[1][2] = toYOffset;

	return transformMatrix;
}

template <typename T>
static T Clamp(T min, T value, T max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
template float Clamp(float min, float value, float max);

static void reset_camera_in_world() {
	// Start position is centered in x & y directions and stepped back in the z direction.
	tl::Vec3<float> position = {
		world.position.x,
		world.position.y,
		world.position.z - world.half_size.z
	};

	float field_of_view_in_deg = 35.0f;

	// camera         near   object    far
	// 	 |             |    |------|    |
	//   |-------------|----------------|----> z
	//   0
	float far_plane = 2.5f * world.half_size.z;
	float near_plane = 0.1f * far_plane;
	camera_reset(
		actual_width,
		actual_height,
		position,
		field_of_view_in_deg,
		near_plane,
		far_plane,
		tl::Vec3<float> { 0.0, 0.0f, 1.0f },
		tl::Vec3<float> { 0.0, 1.0f, 0.0f }
	);
	Camera camera = camera_get();
	top_clipping_plane.position = camera.view_frustrum.near_top_left_corner_position;
	top_clipping_plane.normal = camera.view_frustrum.up_plane_normal;
}

static void reset_world_to_mesh() {
	tl::Vec3<float> first_triangle_vertice = meshArray.get(0).p[0];
	tl::Vec3<float> max = first_triangle_vertice;
	tl::Vec3<float> min = first_triangle_vertice;

	for (int i = 0; i < meshArray.length(); i += 1)
	{
		Triangle4d tri = meshArray.get(i);
		if (tri.p[0].x > max.x) max.x = tri.p[0].x;
		if (tri.p[0].x < min.x) min.x = tri.p[0].x;
		if (tri.p[0].y > max.y) max.y = tri.p[0].y;
		if (tri.p[0].y < min.y) min.y = tri.p[0].y;
		if (tri.p[0].z > max.z) max.z = tri.p[0].z;
		if (tri.p[0].z < min.z) min.z = tri.p[0].z;

		if (tri.p[1].x > max.x) max.x = tri.p[1].x;
		if (tri.p[1].x < min.x) min.x = tri.p[1].x;
		if (tri.p[1].y > max.y) max.y = tri.p[1].y;
		if (tri.p[1].y < min.y) min.y = tri.p[1].y;
		if (tri.p[1].z > max.z) max.z = tri.p[1].z;
		if (tri.p[1].z < min.z) min.z = tri.p[1].z;

		if (tri.p[2].x > max.x) max.x = tri.p[2].x;
		if (tri.p[2].x < min.x) min.x = tri.p[2].x;
		if (tri.p[2].y > max.y) max.y = tri.p[2].y;
		if (tri.p[2].y < min.y) min.y = tri.p[2].y;
		if (tri.p[2].z > max.z) max.z = tri.p[2].z;
		if (tri.p[2].z < min.z) min.z = tri.p[2].z;
	}

	tl::Vec3<float> depth = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
	mesh.half_size.x = 0.5f * (max.x - min.x);
	mesh.half_size.y = 0.5f * (max.y - min.y);
	mesh.half_size.z = 0.5f * (max.z - min.z);

	mesh.position.x = 0.5f * (max.x + min.x);
	mesh.position.y = 0.5f * (max.y + min.y);
	mesh.position.z = 0.5f * (max.z + min.z);

	// set the bounds of the camera
	world.position.x = mesh.position.x;
	world.position.y = mesh.position.y;
	world.position.z = mesh.position.z;
	const float space_around_mesh_scale_factor = 8.0f;

	float max_mesh_half_size = (mesh.half_size.x > mesh.half_size.y)
		? ((mesh.half_size.x > mesh.half_size.z) ? mesh.half_size.x : mesh.half_size.z)
		: ((mesh.half_size.y > mesh.half_size.z) ? mesh.half_size.y : mesh.half_size.z);

	world.half_size.x = space_around_mesh_scale_factor * max_mesh_half_size;
	world.half_size.y = space_around_mesh_scale_factor * max_mesh_half_size;
	world.half_size.z = space_around_mesh_scale_factor * max_mesh_half_size;

	float max_world_x = world.position.x + world.half_size.x;
	float min_world_x = world.position.x - world.half_size.x;
	float max_world_y = world.position.y + world.half_size.y;
	float min_world_y = world.position.y - world.half_size.y;
	float max_world_z = world.position.z + world.half_size.z;
	float min_world_z = world.position.z - world.half_size.z;

	// add triangles for the world border
	meshArray.append({min_world_x, min_world_y, max_world_z,	min_world_x, max_world_y, max_world_z,	max_world_x, max_world_y, max_world_z,	0x000055});
	meshArray.append({min_world_x, min_world_y, max_world_z,	max_world_x, max_world_y, max_world_z,	max_world_x, min_world_y, max_world_z,	0x000055 });

	positionIncrement = 0.01f * world.half_size.x;

	// Initialize the map
	float map_half_width = 0.05f * (float)screen_width;
	map.halfSize = {
		map_half_width,
		map_half_width * world.half_size.x / world.half_size.z
	};
	const float map_margin_in_pixels = 50.0f;
	map.position = {
		(float)screen_width - map.halfSize.x - map_margin_in_pixels,
		map.halfSize.y + map_margin_in_pixels
	};

	// Initialize the projection matrix for world to map
	tl::Rect<float> top_down_world = tl::Rect<float>();

	// Using a top down projection for the map view.
	// So depth (z) in the world --> horizontal (x) on the screen map.
	// Left/right in the world (x) --> vertical (y) on the screen map.
	top_down_world.position = tl::Vec2<float> { world.position.z, world.position.x };
	top_down_world.halfSize = tl::Vec2<float> { world.half_size.z, world.half_size.x };
	world_to_map_scale_factor = map.halfSize.x / top_down_world.halfSize.x;
	mapProjectionMatrix = GenerateProjectionMatrix(top_down_world, map);

	reset_camera_in_world();
}

static void load_asset_to_array(const char* filename, tl::array<Triangle4d>& triangles, tl::MemorySpace& transient)
{
	void* asset = asset_interface_open(filename);
	if (asset == nullptr) {
		return;
	}

	const unsigned int string_buffer_size = 256;
	char string_buffer[string_buffer_size];

	tl::array<tl::Vec3<float>> heapVertices = tl::array<tl::Vec3<float>>(transient);
	while (asset_interface_read_line(asset, string_buffer, string_buffer_size)) {

		std::string line = std::string(string_buffer);

		char junk;

		std::strstream stringStream;
		stringStream << line;

		if (line[0] == 'v' && line[1] == ' ')
		{
			tl::Vec3<float> vertex;
			// expect line to have syntax 'v x y z' where x, y & z are the ordinals of the point position
			stringStream >> junk >> vertex.x >> vertex.y >> vertex.z;
			heapVertices.append(vertex);
		}

		if (line[0] == 'f' && line[1] == ' ')
		{
			int points[3];
			stringStream >> junk >> points[0] >> points[1] >> points[2];
			// expect line to have syntax 'f 1 2 3' where 1, 2 & 3 are the 1-indexed positions of the points in the file
			Triangle4d newTriangle = {
				heapVertices.get(points[0] - 1),
				heapVertices.get(points[1] - 1),
				heapVertices.get(points[2] - 1)
			};
			newTriangle.color = 0xAAAAAA;

			triangles.append(newTriangle);
		}
	}

	asset_interface_close(asset);
}

static void reset_mesh_to_teapot() {
	meshArray.clear();
	tl::MemorySpace temp = transient_memory; // make a copy of the transient memory so it can be modified
	load_asset_to_array("teapot.obj", meshArray, temp);

	reset_world_to_mesh();
}

static void reset_mesh_to_cube() {
	meshArray.clear();
	// Using a clockwise winding convention
	// -ve z face
	meshArray.append({ 0.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f, 0.0f });
	meshArray.append({ 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 2.0f,		1.0f, 1.0f, -1.0f });

	// +ve x face
//	meshArray.append({ 1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f });
//	meshArray.append({ 1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f });

	// +ve z face
//	meshArray.append({ 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f });
//	meshArray.append({ 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f });

	// -ve x face
//	meshArray.append({ 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f });
//	meshArray.append({ 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f });

	// +ve y face
//	meshArray.append({ 0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f });
//	meshArray.append({ 0.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f });

	// -ve y face
//	meshArray.append({ 1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f });
//	meshArray.append({ 1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f });

	reset_world_to_mesh();
}

static int Initialize(const tl::MemorySpace& persistent)
{
	tl::font_interface_initialize();

	meshArray.initialize(persistent);

	depth_buffer.width = screen_width;
	depth_buffer.height = screen_height;
	depth_buffer.depths = depth_array;

	reset_mesh_to_teapot();

	return 0;
}

static void keep_camera_in_bounds(const Camera& camera) {
	// Final bounds check on the camera
	tl::Vec3<float> camera_position = camera.position;
	camera_position.x = Clamp(world.position.x - world.half_size.x, camera_position.x, world.position.x + world.half_size.x);
	camera_position.y = Clamp(world.position.y - world.half_size.y, camera_position.y, world.position.y + world.half_size.y);
	camera_position.z = Clamp(world.position.z - world.half_size.z, camera_position.z, world.position.z + world.half_size.z);

	camera_reset(
		actual_width,
		actual_height,
		camera_position,
		camera.field_of_view_deg,
		camera.near_plane,
		camera.far_plane,
		camera.unit_direction,
		camera.unit_up
	);
}

static void process_input_for_camera(
	const tl::Input& input,
	const Camera& camera
) {
	static const float rotation_increment_in_degrees = 0.75f;
	if (input.buttons[tl::KEY_D].isDown) {
		camera_increment_yaw(rotation_increment_in_degrees);
	}
	else if (input.buttons[tl::KEY_A].isDown) {
		camera_increment_yaw(-rotation_increment_in_degrees);
	}

	if (input.buttons[tl::KEY_SHIFT].isDown && input.buttons[tl::KEY_UP].isDown) {
		camera_increment_pitch(rotation_increment_in_degrees);
	}
	else if (input.buttons[tl::KEY_SHIFT].isDown && input.buttons[tl::KEY_DOWN].isDown) {
		camera_increment_pitch(-rotation_increment_in_degrees);
	}

	if (input.buttons[tl::KEY_Q].isDown) {
		camera_increment_roll(rotation_increment_in_degrees);
	}
	else if (input.buttons[tl::KEY_E].isDown) {
		camera_increment_roll(-rotation_increment_in_degrees);
	}

	if (input.buttons[tl::KEY_U].isDown) {
		const float clip_increment = 0.05f;
		if (input.buttons[tl::KEY_SHIFT].isDown) {
			top_clipping_plane.position.y -= clip_increment;
		}
		else {
			top_clipping_plane.position.y += clip_increment;
		}
		return;
	}

	// Next process any forwards or backwards movement
	if (input.buttons[tl::KEY_S].isDown)
	{
		camera_increment_direction(-positionIncrement);
		keep_camera_in_bounds(camera);
	}
	else if (input.buttons[tl::KEY_W].isDown)
	{
		camera_increment_direction(positionIncrement);
		keep_camera_in_bounds(camera);
	}

	if (input.buttons[tl::KEY_LEFT].isDown)
	{
		camera_increment_strafe(-positionIncrement);
		keep_camera_in_bounds(camera);
	}
	else if (input.buttons[tl::KEY_RIGHT].isDown)
	{
		camera_increment_strafe(positionIncrement);
		keep_camera_in_bounds(camera);
	}

	// Simply move the camera position vertically with up/down keypress
	if (input.buttons[tl::KEY_DOWN].isDown && !input.buttons[tl::KEY_SHIFT].isDown)
	{
		camera_increment_up(-positionIncrement);
		keep_camera_in_bounds(camera);
	}
	else if (input.buttons[tl::KEY_UP].isDown && !input.buttons[tl::KEY_SHIFT].isDown)
	{
		camera_increment_up(positionIncrement);
		keep_camera_in_bounds(camera);
	}

	if (input.buttons[tl::KEY_V].isDown) {
		float camera_fov_delta = (input.buttons[tl::KEY_SHIFT].isDown)
			? -0.25f
			: 0.25f;

		camera_reset(
			actual_width,
			actual_height,
			camera.position,
			camera.field_of_view_deg + camera_fov_delta,
			camera.near_plane,
			camera.far_plane,
			camera.unit_direction,
			camera.unit_up
		);
		return;
	}

	if (input.buttons[tl::KEY_N].isDown) {
		float near_plane_increment = (input.buttons[tl::KEY_SHIFT].isDown)
			? -0.1f
			: 0.1f;

		camera_reset(
			actual_width,
			actual_height,
			camera.position,
			camera.field_of_view_deg,
			camera.near_plane + near_plane_increment,
			camera.far_plane,
			camera.unit_direction,
			camera.unit_up
		);

		return;
	}

	if (input.buttons[tl::KEY_M].isDown) {
		float far_plane_increment = (input.buttons[tl::KEY_SHIFT].isDown)
			? -0.1f
			: 0.1f;

		camera_reset(
			actual_width,
			actual_height,
			camera.position,
			camera.field_of_view_deg,
			camera.near_plane,
			camera.far_plane + far_plane_increment,
			camera.unit_direction,
			camera.unit_up
		);

		return;
	}

	if (input.buttons[tl::KEY_C].keyUp)
	{
		reset_camera_in_world();
	}
}

static int UpdateAndRender1(
	const tl::MemorySpace& transient,
	const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	const Camera& camera = camera_get();
	const uint32_t BACKGROUND_COLOR = 0x000000;
	tl::ClearScreen(renderBuffer, BACKGROUND_COLOR);
	if (!isStarted)
	{
		tl::Rect<float> titleCharRect = tl::Rect<float>();
		titleCharRect.position = { 100.0f, 350.0f };
		titleCharRect.halfSize = { 20.0f, 30.0f };
		tl::font_interface_render_chars(
			renderBuffer,
			"TL DEMO",
			titleCharRect,
			0x999999
		);

		tl::Rect<float> subtitleCharRect;
		subtitleCharRect.position = { 100.0f, 250.0f };
		subtitleCharRect.halfSize = MultiplyVectorByScalar(titleCharRect.halfSize, 0.5f);
		tl::font_interface_render_chars(
			renderBuffer,
			"RUN AS ADMIN FOR TEAPOT",
			subtitleCharRect,
			0x999999
		);

		subtitleCharRect.position.y -= 100.0f;
		tl::font_interface_render_chars(
			renderBuffer,
			"PRESS S TO START",
			subtitleCharRect,
			0x999999
		);

		if (input.buttons[tl::KEY_S].keyUp)
		{
			isStarted = true;
			reset_camera_in_world();
		}
		return 0;
	}

	if (input.buttons[tl::KEY_R].keyUp)
	{
		isStarted = false;
		return 0;
	}

	if (input.buttons[tl::KEY_F].keyUp) {
		wireframe = !wireframe;
	}

	if (input.buttons[tl::KEY_G].keyUp) {
		is_teapot = !is_teapot;
		if (is_teapot) {
			reset_mesh_to_teapot();
		}
		else {
			reset_mesh_to_cube();
		}
		return 0;
	}

	process_input_for_camera(input, camera);

	TransformAndRenderMesh(renderBuffer, meshArray, camera, transient);


	// Show info about z-position
	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 100.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };

	tl::font_interface_render_chars(renderBuffer, "FOV", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.field_of_view_deg, charFoot, 0xAAAAAA);

	charFoot.position.y -= fontSize;
	tl::font_interface_render_chars(renderBuffer, "NEAR PLANE", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)(10.0f * camera.near_plane), charFoot, 0xAAAAAA);

	charFoot.position = { 200.0f, infoHeight };
	tl::font_interface_render_chars(renderBuffer, "POS", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.position.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.position.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.position.z, charFoot, 0xAAAAAA);

	charFoot.position = { 300.0f, infoHeight };
	tl::font_interface_render_chars(renderBuffer, "MESH", charFoot, 0xAAAAAA);
	charFoot.position.x = 400.0f;
	tl::font_interface_render_int(renderBuffer, meshArray.length(), charFoot, 0xAAAAAA);

	charFoot.position.y -= fontSize;
	charFoot.position.x = 300.0f;
	tl::font_interface_render_chars(renderBuffer, "VIEWED", charFoot, 0xAAAAAA);
	charFoot.position.x = 400.0f;
	tl::font_interface_render_int(renderBuffer, viewed_triangle_count, charFoot, 0xAAAAAA);

	charFoot.position.y -= fontSize;
	charFoot.position.x = 300.0f;
	tl::font_interface_render_chars(renderBuffer, "PROJECTED", charFoot, 0xAAAAAA);
	charFoot.position.x = 400.0f;
	tl::font_interface_render_int(renderBuffer, projected_triangle_count, charFoot, 0xAAAAAA);

	charFoot.position.y -= fontSize;
	charFoot.position.x = 300.0f;
	tl::font_interface_render_chars(renderBuffer, "RENDERED", charFoot, 0xAAAAAA);
	charFoot.position.x = 400.0f;
	tl::font_interface_render_int(renderBuffer, rendered_triangle_count, charFoot, 0xAAAAAA);

	// Draw the map
	tl::DrawRect(renderBuffer, 0x333399, map);

	tl::Rect<float> mesh_footprint;
	mesh_footprint.position = Transform2DVector(tl::Vec2<float> { mesh.position.z, mesh.position.x }, mapProjectionMatrix);
	mesh_footprint.halfSize = { world_to_map_scale_factor * mesh.half_size.z, world_to_map_scale_factor * mesh.half_size.x };
	tl::DrawRect(renderBuffer, 0x00AA00, mesh_footprint);


	tl::Vec2<float> topDownCameraPosition = {
		camera.position.z,
		camera.position.x
	};

	tl::Vec2<float> camera_near_plane_right, camera_near_plane_left, camera_far_plane_right, camera_far_plane_left;
	get_camera_plane_map_coords(camera_near_plane_right, camera_near_plane_left, camera_far_plane_right, camera_far_plane_left);

	tl::Vec2<float> mapCameraPosition;
	mapCameraPosition = Transform2DVector(topDownCameraPosition, mapProjectionMatrix);
	tl::DrawCircle(renderBuffer, 0x993333, mapCameraPosition, 5.0f);

	tl::DrawLineInPixels(
		renderBuffer,
		0x993333,
		mapCameraPosition,
		camera_far_plane_left
	);

	tl::DrawLineInPixels(
		renderBuffer,
		0x993333,
		mapCameraPosition,
		camera_far_plane_right
	);

	tl::DrawLineInPixels(
		renderBuffer,
		0x993333,
		camera_near_plane_left,
		camera_near_plane_right
	);

	tl::DrawLineInPixels(
		renderBuffer,
		0x993333,
		camera_far_plane_left,
		camera_far_plane_right
	);

	return 0;
}

static int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	actual_width = renderBuffer.width - 4;
	actual_height = renderBuffer.height - 4;
	float dt = (float)dtInMilliseconds / 1000.0f;
	return UpdateAndRender1(transient_memory, input, renderBuffer, dt);
}

int demo_main()
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Demo";
	settings.width = screen_width;
	settings.height = screen_height;

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	persistent_memory.sizeInBytes = 10 * 1024 * 1024;
	transient_memory.sizeInBytes = 10 * 1024 * 1024;

	persistent_memory.content = malloc(persistent_memory.sizeInBytes);
	transient_memory.content = malloc(transient_memory.sizeInBytes);

	Initialize(persistent_memory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
