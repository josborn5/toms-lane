#include <stdlib.h>
#include <strstream>
#include <string>
#include <math.h>
#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./render.hpp"
#include "./file.hpp"
#include "./camera.hpp"


struct cuboid {
	tl::Vec3<float> position = {0};
	tl::Vec3<float> half_size = {0};
};

struct Plane
{
	tl::Vec3<float> position;
	tl::Vec3<float> normal;
};

struct Triangle4d
{
	tl::Vec4<float> p[3];
	unsigned int color;
};

const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;

static bool wireframe = false;
static bool is_teapot = true;

static cuboid world;
static cuboid mesh;

static tl::array<Triangle4d> meshArray = tl::array<Triangle4d>();

static float positionIncrement = 0.0f;

static tl::Rect<float> map;
static float world_to_map_scale_factor = 0.0f;
static tl::Matrix2x3<float> mapProjectionMatrix;

static tl::GameMemory appMemory;

static bool isStarted = false;

static unsigned int rendered_triangle_count = 0;
static unsigned int projected_triangle_count = 0;
static unsigned int viewed_triangle_count = 0;

static float deg_to_rad(float degrees) {
	constexpr float pi_over_180 = 3.14159f / 180.0f;
	return degrees * pi_over_180;
}

static tl::Vec4<float> IntersectPlane(
	const Plane& plane,
	const tl::Vec4<float>& lineStart,
	const tl::Vec4<float>& lineEnd
)
{
	tl::Vec3<float> normalizedPlaneN = tl::UnitVector(plane.normal);
	tl::Vec4<float> temp_normalized_plane;
	temp_normalized_plane.z = normalizedPlaneN.z;
	temp_normalized_plane.x = normalizedPlaneN.x;
	temp_normalized_plane.y = normalizedPlaneN.y;
	temp_normalized_plane.w = 0.0f;

	float planeD = tl::DotProduct(normalizedPlaneN, plane.position);
	float ad = tl::DotProduct(temp_normalized_plane, lineStart);
	float bd = tl::DotProduct(temp_normalized_plane, lineEnd);
	float t = (planeD - ad) / (bd - ad);
	tl::Vec4<float> lineStartToEnd = tl::SubtractVectors(lineEnd, lineStart);
	tl::Vec4<float> lineToIntersect = tl::MultiplyVectorByScalar(lineStartToEnd, t);
	tl::Vec4<float> return_temp = tl::AddVectors(lineStart, lineToIntersect);
	return_temp.w = 1.0f;
	return return_temp;
}

static float ShortestDistanceFromPointToPlane(
	const tl::Vec4<float>& point,
	const tl::Vec3<float>& planeP,
	const tl::Vec3<float>& unitNormalToPlane
)
{
	tl::Vec3<float> temp_point;
	temp_point.x = point.x;
	temp_point.y = point.y;
	temp_point.z = point.z;
	float distance = tl::DotProduct(unitNormalToPlane, temp_point) - tl::DotProduct(unitNormalToPlane, planeP);
	return distance;
}

static int ClipTriangleAgainstPlane(
	const Plane& plane,
	Triangle4d& inputTriangle,
	Triangle4d& outputTriangle1,
	Triangle4d& outputTriangle2
)
{
	tl::Vec3<float> unitNormalToPlane = UnitVector(plane.normal);

	// Two baskets to store points that are inside the plane and points that are outside
	tl::Vec4<float>* insidePoints[3];
	tl::Vec4<float>* outsidePoints[3];
	int insidePointCount = 0;
	int outsidePointCount = 0;

	// Work out the distance between the plane and each point on the triangle and put it in the relevant basket
	for (int i = 0; i < 3; i += 1)
	{
		float distance = ShortestDistanceFromPointToPlane(inputTriangle.p[i], plane.position, unitNormalToPlane);
		if (distance >= 0)
		{
			insidePoints[insidePointCount] = &inputTriangle.p[i];
			insidePointCount += 1;
		}
		else
		{
			outsidePoints[outsidePointCount] = &inputTriangle.p[i];
			outsidePointCount += 1;
		}
	}

	// All the points in the triangle are outside the plane
	if (insidePointCount == 0)
	{
		// inputTriangle is not valid.
		return 0;
	}

	// All the points on the triangle are inside the plane
	if (insidePointCount == 3)
	{
		// inputTriangle is valid.
		outputTriangle1 = inputTriangle;
		return 1;
	}

	// Two points lie outside the plane
	if (insidePointCount == 1 && outsidePointCount == 2)
	{
		// keep the one point inside the place in the output triangle
		outputTriangle1.p[0] = *insidePoints[0];

		// for the other two points, work out where the triangleintersects the plane and use those points in hte output
		outputTriangle1.p[1] = IntersectPlane(plane, *insidePoints[0], *outsidePoints[0]);
		outputTriangle1.p[2] = IntersectPlane(plane, *insidePoints[0], *outsidePoints[1]);
		outputTriangle1.color = inputTriangle.color;
		return 1;
	}

	// One point lies outside the plane
	if (insidePointCount == 2 && outsidePointCount == 1)
	{
		// Create two new triangles from the two points inside the plane and the two points where the triangle intersects the plane
		outputTriangle1.p[0] = *insidePoints[0];
		outputTriangle1.p[1] = *insidePoints[1];
		outputTriangle1.p[2] = IntersectPlane(plane, *insidePoints[0], *outsidePoints[0]);
		outputTriangle1.color = inputTriangle.color;

		outputTriangle2.p[0] = *insidePoints[1];
		outputTriangle2.p[1] = outputTriangle1.p[2];
		outputTriangle2.p[2] = IntersectPlane(plane, *insidePoints[1], *outsidePoints[0]);
		outputTriangle2.color = inputTriangle.color;
		return 2;
	}

	return 0;
}


static void get_camera_plane_map_coords(tl::Vec2<float>& near_1, tl::Vec2<float>& near_2, tl::Vec2<float>& far_1, tl::Vec2<float>& far_2) {
	const Camera& camera = camera_get();
	tl::Vec3<float> unit_normal_to_direction = tl::UnitVector(
		tl::CrossProduct(
			camera.unit_direction,
			camera.unit_up
		)
	);
	float tan_half_fov = tanf(deg_to_rad(0.5f * camera.field_of_view_deg));

	float near_opp = camera.near_plane * tan_half_fov;
	tl::Vec3<float> near_plane_left = camera.view_frustrum.near_top_left_corner_position;
	tl::Vec3<float> near_plane_right_from_left = MultiplyVectorByScalar(unit_normal_to_direction, (2.0f * near_opp));
	tl::Vec3<float> near_plane_right = tl::AddVectors(near_plane_left, near_plane_right_from_left);

	float far_opp = camera.far_plane * tan_half_fov;
	tl::Vec3<float> far_plane_right = camera.view_frustrum.far_bottom_right_corner_position;
	tl::Vec3<float> far_plane_left_from_right = MultiplyVectorByScalar(unit_normal_to_direction, (-2.0f * far_opp));
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
	const tl::Matrix4x4<float>& projectionMatrix,
	const tl::MemorySpace& transient
) {
	const int RED = 0xFF;
	const int GREEN = 0xAA;
	const int BLUE = 0x88;

	rendered_triangle_count = 0;
	projected_triangle_count = 0;
	viewed_triangle_count = 0;

	tl::Matrix4x4<float> viewMatrix;
	camera_fill_view_matrix(viewMatrix);

	tl::array<Triangle4d> trianglesToDrawArray = tl::array<Triangle4d>(transient);

	Plane bottomOfScreen = { 0.0f, 0.0f, 0.0f,                             0.0f,  1.0f, 0.0f };
	Plane topOfScreen =    { 0.0f, (float)(renderBuffer.height - 1), 0.0f, 0.0f, -1.0f, 0.0f };
	Plane leftOfScreen =   { 0.0f, 0.0f, 0.0f,                             1.0f,  0.0f, 0.0f };
	Plane rightOfScreen =  { (float)(renderBuffer.width - 1), 0.0f, 0.0f, -1.0f,  0.0f, 0.0f };

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

		uint32_t triangleColor = tl::GetColorFromRGB(int(RED * shade), int(GREEN * shade), int(BLUE * shade));

		Plane near_plane;
		near_plane.position = camera.view_frustrum.near_top_left_corner_position;
		near_plane.normal = camera.view_frustrum.near_plane_normal;

		Plane far_plane;
		far_plane.position = camera.view_frustrum.far_bottom_right_corner_position;
		far_plane.normal = camera.view_frustrum.far_plane_normal;

		Triangle4d queue_data[18]; // 3 * 6 sides of view frustrum to clip
		tl::MemorySpace queue_data_space;
		queue_data_space.content = queue_data;
		queue_data_space.sizeInBytes = 18 * sizeof(Triangle4d);
		tl::queue<Triangle4d> triangle_queue = tl::queue<Triangle4d>(queue_data_space);
		triangle_queue.enqueue(tri);

		Triangle4d clipped[2];
		int new_triangles = 1;
		for (int plane_index = 0; plane_index < 2; plane_index += 1)
		{
			int triangles_to_add = 0;
			while (new_triangles > 0)
			{
				new_triangles -= 1;
				tl::operation<Triangle4d> dequeue_op = triangle_queue.dequeue();
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
				}

				for (int i = 0; i < triangles_to_add; i += 1)
				{
					triangle_queue.enqueue(clipped[i]);
				}
			}

			new_triangles = triangle_queue.length();
		}

		for (int i = 0; i < triangle_queue.length(); i += 1)
		{
			Triangle4d to_transform = triangle_queue.content[i];

			viewed_triangle_count += 1;
			// Convert the triangle position from world space to view space
			Triangle4d viewed;
			MultiplyVectorWithMatrix(to_transform.p[0], viewed.p[0], viewMatrix);
			MultiplyVectorWithMatrix(to_transform.p[1], viewed.p[1], viewMatrix);
			MultiplyVectorWithMatrix(to_transform.p[2], viewed.p[2], viewMatrix);

			projected_triangle_count += 1;
			Triangle4d projected;
			// Project each triangle in 3D space onto the 2D space triangle to render
			Project3DPointTo2D(viewed.p[0], projected.p[0], projectionMatrix);
			Project3DPointTo2D(viewed.p[1], projected.p[1], projectionMatrix);
			Project3DPointTo2D(viewed.p[2], projected.p[2], projectionMatrix);

			// Scale to view
			Triangle4d triToRender = projected;
			triToRender.p[0].x *= (float)screen_width;
			triToRender.p[0].y *= (float)screen_height;
			triToRender.p[1].x *= (float)screen_width;
			triToRender.p[1].y *= (float)screen_height;
			triToRender.p[2].x *= (float)screen_width;
			triToRender.p[2].y *= (float)screen_height;

			const float translateX = (float)0.5 * (float)renderBuffer.width;
			const float translateY = (float)0.5 * (float)renderBuffer.height;
			triToRender.p[0].x += translateX; triToRender.p[0].y += translateY;
			triToRender.p[1].x += translateX; triToRender.p[1].y += translateY;
			triToRender.p[2].x += translateX; triToRender.p[2].y += translateY;

			triToRender.color = triangleColor;

			trianglesToDrawArray.append(triToRender);
		}
	}

	tl::MemorySpace remainingTransient = trianglesToDrawArray.sizeToCurrentLength();

	// sort the triangles back to front
	qsort(trianglesToDrawArray.head_pointer(), trianglesToDrawArray.length(), sizeof(Triangle4d), compare_triangle_depth);

	for (int n = 0; n < trianglesToDrawArray.length(); n += 1)
	{
		Triangle4d triToRender = trianglesToDrawArray.get(n);
		Triangle4d clipped[2];

		tl::queue<Triangle4d> triangleQueue = tl::queue<Triangle4d>(remainingTransient);
		if (triangleQueue.enqueue(triToRender) != 0) continue;

		int newTriangles = 1;

		// Clip against each screen edge
		for (int edge = 0; edge < 4; edge += 1)
		{
			int trianglesToAdd = 0;
			while (newTriangles > 0)
			{
				newTriangles -= 1;
				tl::operation<Triangle4d> dequeue_op = triangleQueue.dequeue();
				Triangle4d test = dequeue_op.value;

				switch (edge)
				{
					case 0:
					{
						trianglesToAdd = ClipTriangleAgainstPlane(bottomOfScreen, test, clipped[0], clipped[1]);
						break;
					}
					case 1:
					{
						trianglesToAdd = ClipTriangleAgainstPlane(topOfScreen, test, clipped[0], clipped[1]);
						break;
					}
					case 2:
					{
						trianglesToAdd = ClipTriangleAgainstPlane(leftOfScreen, test, clipped[0], clipped[1]);
						break;
					}
					case 3:
					{
						trianglesToAdd = ClipTriangleAgainstPlane(rightOfScreen, test, clipped[0], clipped[1]);
						break;
					}
				}

				for (int i = 0; i < trianglesToAdd; i += 1)
				{
					triangleQueue.enqueue(clipped[i]);
				}
			}

			newTriangles = triangleQueue.length();
		}

		for (int i = 0; i < triangleQueue.length(); i += 1)
		{
			rendered_triangle_count += 1;
			Triangle4d draw = triangleQueue.content[i];

			if (wireframe) {
				tl::Vec2<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
				tl::Vec2<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
				tl::Vec2<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };
				DrawTriangleInPixels(renderBuffer, 0xFFFFFF, p0Int, p1Int, p2Int);

			} else {
				tl::Vec3<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
				tl::Vec3<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
				tl::Vec3<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };

				FillTriangleInPixels(renderBuffer, draw.color, p0Int, p1Int, p2Int);
			}
		}
	}
}

tl::Matrix2x3<float> GenerateProjectionMatrix(const tl::Rect<float> &from, const tl::Rect<float> &to)
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

	float field_of_view_in_deg = 75.0f;

	// camera         near   object    far
	// 	 |             |    |------|    |
	//   |-------------|----------------|----> z
	//   0
	float far_plane = world.half_size.z;
	float near_plane = 0.1f * far_plane;
	camera_reset(
		(float)screen_width / (float)screen_height,
		position,
		field_of_view_in_deg,
		near_plane,
		far_plane
	);
}

static void reset_world_to_mesh() {
	tl::Vec4<float> first_triangle_vertice = meshArray.get(0).p[0];
	tl::Vec3<float> max = {
		first_triangle_vertice.x,
		first_triangle_vertice.y,
		first_triangle_vertice.z
	};
	tl::Vec3<float> min = {
		first_triangle_vertice.x,
		first_triangle_vertice.y,
		first_triangle_vertice.z
	};

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

	tl::array<tl::Vec4<float>> heapVertices = tl::array<tl::Vec4<float>>(transient);
	while (asset_interface_read_line(asset, string_buffer, string_buffer_size)) {

		std::string line = std::string(string_buffer);

		char junk;

		std::strstream stringStream;
		stringStream << line;

		if (line[0] == 'v' && line[1] == ' ')
		{
			tl::Vec4<float> vertex;
			// expect line to have syntax 'v x y z' where x, y & z are the ordinals of the point position
			stringStream >> junk >> vertex.x >> vertex.y >> vertex.z;
			vertex.w = 1.0f;
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

			triangles.append(newTriangle);
		}
	}

	asset_interface_close(asset);
}

static void reset_mesh_to_teapot() {
	meshArray.clear();
	tl::MemorySpace temp = appMemory.transient; // make a copy of the transient memory so it can be modified
	load_asset_to_array("teapot.obj", meshArray, temp);

	reset_world_to_mesh();
}

static void reset_mesh_to_cube() {
	meshArray.clear();
	// Using a clockwise winding convention
	// -ve z face
	meshArray.append({ 0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f });
	meshArray.append({ 0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f });

	// +ve x face
	meshArray.append({ 1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f });
	meshArray.append({ 1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f });

	// +ve z face
	meshArray.append({ 1.0f, 0.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f });
	meshArray.append({ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f });

	// -ve x face
	meshArray.append({ 0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f });
	meshArray.append({ 0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f });

	// +ve y face
	meshArray.append({ 0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f });
	meshArray.append({ 0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f });

	// -ve y face
	meshArray.append({ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f });
	meshArray.append({ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f });

	reset_world_to_mesh();
}

static int Initialize(const tl::GameMemory& gameMemory)
{
	tl::MemorySpace transientMemory = gameMemory.transient;
	tl::font_interface_initialize();

	meshArray.initialize(gameMemory.permanent);

	reset_mesh_to_teapot();

	return 0;
}

static void keep_camera_in_bounds(const Camera& camera) {
	// Final bounds check on the camera
	tl::Vec3<float> camera_position = camera.position;
	camera_position.x = Clamp(world.position.x - world.half_size.x, camera_position.x, world.position.x + world.half_size.x);
	camera_position.y = Clamp(world.position.y - world.half_size.y, camera_position.y, world.position.y + world.half_size.y);
	camera_position.z = Clamp(world.position.z - world.half_size.z, camera_position.z, world.position.z + world.half_size.z);
	camera_set_position(camera_position);
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

	else if (input.buttons[tl::KEY_J].isDown) {
		camera_set_fov(camera.field_of_view_deg + 0.25f);
	}
	else if (input.buttons[tl::KEY_K].isDown) {
		camera_set_fov(camera.field_of_view_deg - 0.25f);
	}

	else if (input.buttons[tl::KEY_V].isDown) {
		camera_set_near_plane(camera.near_plane + 0.1f);
	}
	else if (input.buttons[tl::KEY_B].isDown) {
		camera_set_near_plane(camera.near_plane - 0.1f);
	}

	if (input.buttons[tl::KEY_C].keyUp)
	{
		reset_camera_in_world();
	}
}

static int UpdateAndRender1(const tl::GameMemory& gameMemory, const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
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

	tl::MemorySpace transientMemory = gameMemory.transient;

	TransformAndRenderMesh(renderBuffer, meshArray, camera, get_projection_matrix(), transientMemory);


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

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return UpdateAndRender1(appMemory, input, renderBuffer, dt);
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

	tl::InitializeMemory(
		10,
		10,
		appMemory
	);

	Initialize(appMemory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
