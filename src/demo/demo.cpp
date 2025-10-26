#include <stdlib.h>
#include <strstream>
#include <string>
#include <math.h>
#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./render.hpp"
#include "./file.hpp"


struct cuboid {
	tl::Vec3<float> position = {0};
	tl::Vec3<float> half_size = {0};
};

struct Camera
{
	tl::Vec3<float> position = {0};
	tl::Vec3<float> direction = {0};
	tl::Vec3<float> up = {0};

	float yaw = 0.0f;
	float field_of_view_deg = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
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

struct matrix3x3
{
	float element[3][3]; // row then col index
};

static void matrix3x3_dot_vect3(const matrix3x3& matrix, const tl::Vec3<float>& input, tl::Vec3<float>& output) {
	output.x = (matrix.element[0][0] * input.x) + (matrix.element[0][1] * input.y) + (matrix.element[0][2] * input.z);
	output.y = (matrix.element[1][0] * input.x) + (matrix.element[1][1] * input.y) + (matrix.element[1][2] * input.z);
	output.z = (matrix.element[2][0] * input.x) + (matrix.element[2][1] * input.y) + (matrix.element[2][2] * input.z);
}

const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;
constexpr float aspect_ratio = (float)screen_width / (float)screen_height;

static bool wireframe = false;
static bool is_teapot = true;

static cuboid world;
static cuboid mesh;

static Camera camera;
static tl::array<Triangle4d> meshArray = tl::array<Triangle4d>();

static tl::Matrix4x4<float> projectionMatrix;

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
	return tl::AddVectors(lineStart, lineToIntersect);
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

static void get_camera_near_plane_position(const Camera& camera, tl::Vec3<float>& position) {
	tl::Vec3<float> near_plane_center_from_position = MultiplyVectorByScalar(
		tl::UnitVector(
			tl::Vec3<float>{ camera.direction.x, camera.direction.y, camera.direction.z }
		),
		camera.near_plane);
	position = tl::AddVectors(
		tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
		near_plane_center_from_position);
}

static void get_camera_near_plane_map_coords(tl::Vec2<float>& p1, tl::Vec2<float>& p2) {
	tl::Vec3<float> near_plane_center;
	get_camera_near_plane_position(camera, near_plane_center);

	tl::Vec3<float> unit_normal_to_direction = tl::UnitVector(
		tl::CrossProduct(
			tl::Vec3<float>{ camera.direction.x, camera.direction.y, camera.direction.z },
			tl::Vec3<float>{ camera.up.x, camera.up.y, camera.up.z }
		)
	);
	float opp = camera.near_plane * tanf(deg_to_rad(0.5f * camera.field_of_view_deg));
	tl::Vec3<float> near_plane_right_from_center = MultiplyVectorByScalar(unit_normal_to_direction, opp);
	tl::Vec3<float> near_plane_right = tl::AddVectors(near_plane_center, near_plane_right_from_center);
	tl::Vec3<float> near_plane_left = tl::SubtractVectors(near_plane_center, near_plane_right_from_center);

	p1 = Transform2DVector(tl::Vec2<float>{ near_plane_left.z, near_plane_left.x }, mapProjectionMatrix);
	p2 = Transform2DVector(tl::Vec2<float>{ near_plane_right.z, near_plane_right.x }, mapProjectionMatrix);
}

static void get_camera_plane_map_coords(tl::Vec2<float>& near_1, tl::Vec2<float>& near_2, tl::Vec2<float>& far_1, tl::Vec2<float>& far_2) {
	tl::Vec3<float> unit_direction = tl::UnitVector(
		tl::Vec3<float>{ camera.direction.x, camera.direction.y, camera.direction.z });
	tl::Vec3<float> unit_normal_to_direction = tl::UnitVector(
		tl::CrossProduct(
			tl::Vec3<float>{ camera.direction.x, camera.direction.y, camera.direction.z },
			tl::Vec3<float>{ camera.up.x, camera.up.y, camera.up.z }
		)
	);
	float tan_half_fov = tanf(deg_to_rad(0.5f * camera.field_of_view_deg));

	tl::Vec3<float> near_plane_center_from_position = MultiplyVectorByScalar(unit_direction, camera.near_plane);
	tl::Vec3<float> near_plane_center = tl::AddVectors(
		tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
		near_plane_center_from_position);

	float near_opp = camera.near_plane * tan_half_fov;
	tl::Vec3<float> near_plane_right_from_center = MultiplyVectorByScalar(unit_normal_to_direction, near_opp);
	tl::Vec3<float> near_plane_right = tl::AddVectors(near_plane_center, near_plane_right_from_center);
	tl::Vec3<float> near_plane_left = tl::SubtractVectors(near_plane_center, near_plane_right_from_center);


	tl::Vec3<float> far_plane_center_from_position = MultiplyVectorByScalar(
		unit_direction, camera.far_plane);
	tl::Vec3<float> far_plane_center = tl::AddVectors(
		tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
		far_plane_center_from_position);

	float far_opp = camera.far_plane * tan_half_fov;
	tl::Vec3<float> far_plane_right_from_center = MultiplyVectorByScalar(unit_normal_to_direction, far_opp);
	tl::Vec3<float> far_plane_right = tl::AddVectors(far_plane_center, far_plane_right_from_center);
	tl::Vec3<float> far_plane_left = tl::SubtractVectors(far_plane_center, far_plane_right_from_center);

	near_1 = Transform2DVector(tl::Vec2<float>{ near_plane_left.z, near_plane_left.x }, mapProjectionMatrix);
	near_2 = Transform2DVector(tl::Vec2<float>{ near_plane_right.z, near_plane_right.x }, mapProjectionMatrix);
	far_1 = Transform2DVector(tl::Vec2<float>{ far_plane_left.z, far_plane_left.x }, mapProjectionMatrix);
	far_2 = Transform2DVector(tl::Vec2<float>{ far_plane_right.z, far_plane_right.x }, mapProjectionMatrix);
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

/**
* Structure of the PointAt Matrix:
* | Ax | Ay | Az | 0 |
* | Bx | By | Bz | 0 |
* | Cx | Cy | Cz | 0 |
* | Tx | Ty | Tz | 1 |
*/
static void point_at(
	const tl::Vec3<float>& position,
	const tl::Vec3<float>& target,
	const tl::Vec3<float>& up,
	tl::Matrix4x4<float>& output_matrix
)
{
	// Vector from the position to the target is the new forward direction
	tl::Vec3<float> forward_unit = tl::SubtractVectors(target, position);
	forward_unit = tl::UnitVector(forward_unit);

	// Calculate the new up direction of the new forward direction
	float newUpScalar = tl::DotProduct(up, forward_unit);
	tl::Vec3<float> newUpTemp = tl::MultiplyVectorByScalar(forward_unit, newUpScalar);
	tl::Vec3<float> upUnit = tl::SubtractVectors(up, newUpTemp);
	upUnit = tl::UnitVector(upUnit);

	// Calculate the new right direction for the new up & forward directions
	tl::Vec3<float> rightUnit = tl::CrossProduct(upUnit, forward_unit);

	// Construct the new transformation matrix
	output_matrix.m[0][0] = rightUnit.x;	output_matrix.m[0][1] = rightUnit.y;	output_matrix.m[0][2] = rightUnit.z;	output_matrix.m[0][3] = 0;
	output_matrix.m[1][0] = upUnit.x;		output_matrix.m[1][1] = upUnit.y;		output_matrix.m[1][2] = upUnit.z;		output_matrix.m[1][3] = 0;
	output_matrix.m[2][0] = forward_unit.x;	output_matrix.m[2][1] = forward_unit.y;	output_matrix.m[2][2] = forward_unit.z;	output_matrix.m[2][3] = 0;
	output_matrix.m[3][0] = position.x;		output_matrix.m[3][1] = position.y;		output_matrix.m[3][2] = position.z;		output_matrix.m[3][3] = 1;
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

	lookAt.m[0][0] = pointAt.m[0][0];	lookAt.m[0][1] = pointAt.m[1][0];	lookAt.m[0][2] = pointAt.m[2][0];	lookAt.m[0][3] = 0;
	lookAt.m[1][0] = pointAt.m[0][1];	lookAt.m[1][1] = pointAt.m[1][1];	lookAt.m[1][2] = pointAt.m[2][1];	lookAt.m[1][3] = 0;
	lookAt.m[2][0] = pointAt.m[0][2];	lookAt.m[2][1] = pointAt.m[1][2];	lookAt.m[2][2] = pointAt.m[2][2];	lookAt.m[2][3] = 0;
	lookAt.m[3][0] = -tDotA;			lookAt.m[3][1] = -tDotB;			lookAt.m[3][2] = -tDotC;			lookAt.m[3][3] = 1;
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

	// Camera matrix
	tl::Vec3<float> target = AddVectors(
		tl::Vec3<float> { camera.position.x, camera.position.y, camera.position.z },
		tl::Vec3<float> { camera.direction.x, camera.direction.y, camera.direction.z }
	);

	tl::Matrix4x4<float> camera_matrix;
	point_at(
		tl::Vec3<float> { camera.position.x, camera.position.y, camera.position.z },
		tl::Vec3<float> { target.x, target.y, target.z },
		tl::Vec3<float> { camera.up.x, camera.up.y, camera.up.z },
		camera_matrix
	);
	// View matrix
	tl::Matrix4x4<float> viewMatrix;
	look_at(camera_matrix, viewMatrix);

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
			tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z }
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

		// TODO: Clip here before doing any more triangle operations
		Triangle4d near_plane_clipped[2];
		tl::Vec3<float> near_plane_position;
		get_camera_near_plane_position(camera, near_plane_position);
		Plane near_plane;
		near_plane.position = { near_plane_position.x, near_plane_position.y, near_plane_position.z };
		near_plane.normal = { camera.direction.x, camera.direction.y, camera.direction.z };

		viewed_triangle_count += 1;
		// Convert the triangle position from world space to view space
		Triangle4d viewed;
		MultiplyVectorWithMatrix(tri.p[0], viewed.p[0], viewMatrix);
		MultiplyVectorWithMatrix(tri.p[1], viewed.p[1], viewMatrix);
		MultiplyVectorWithMatrix(tri.p[2], viewed.p[2], viewMatrix);

		// Clip the triangles before they get projected. Define a plane just in fron of the camera to clip against
		Plane near_clipping_plane;
		near_clipping_plane.position = { 0.0f, 0.0f, camera.near_plane };
		near_clipping_plane.normal = { 0.0f, 0.0f, 1.0f };
		int near_plane_clipped_triangle_count = ClipTriangleAgainstPlane(near_clipping_plane, viewed, near_plane_clipped[0], near_plane_clipped[1]);

		for (int i = 0; i < near_plane_clipped_triangle_count; i += 1)
		{
			projected_triangle_count += 1;
			Triangle4d projected;
			// Project each triangle in 3D space onto the 2D space triangle to render
			Project3DPointTo2D(near_plane_clipped[i].p[0], projected.p[0], projectionMatrix);
			Project3DPointTo2D(near_plane_clipped[i].p[1], projected.p[1], projectionMatrix);
			Project3DPointTo2D(near_plane_clipped[i].p[2], projected.p[2], projectionMatrix);

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
	transformMatrix.m[1][1] = toYGrad;
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

static void set_projection_matrix() {
	projectionMatrix = MakeProjectionMatrix(camera.field_of_view_deg, aspect_ratio, camera.near_plane, camera.far_plane);
}

static void update_camera_direction() {
	// Apply the camera yaw to the camera.direction vector
	tl::Vec4<float> target = { 0.0f, 0.0f, 1.0f };

	float yaw_in_radians = deg_to_rad(camera.yaw);
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

	tl::Vec3<float> temp;
	matrix3x3_dot_vect3(
		y_axis_rotation_matrix,
		tl::Vec3<float>{ 0.0f, 0.0f, 1.0f },
		temp
	);
	camera.direction.x = temp.x;
	camera.direction.y = temp.y;
	camera.direction.z = temp.z;
}

static void ResetCamera()
{
	camera.up = { 0.0f, 1.0f, 0.0f };
	// Start position is centered in x & y directions and stepped back in the z direction.
	camera.position = {
		world.position.x,
		world.position.y,
		world.position.z - world.half_size.z
	};
	camera.yaw = 0.0f;
	update_camera_direction();
	camera.field_of_view_deg = 75.0f;

	// camera         near   object    far
	// 	 |             |    |------|    |
	//   |-------------|----------------|----> z
	//   0
	camera.far_plane = 1.0f * world.half_size.z;
	camera.near_plane = 0.1f * camera.far_plane;

	set_projection_matrix();
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
	world.half_size.x = space_around_mesh_scale_factor * mesh.half_size.x;
	world.half_size.y = space_around_mesh_scale_factor * mesh.half_size.y;
	world.half_size.z = space_around_mesh_scale_factor * mesh.half_size.z;

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

	ResetCamera();
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


static const float yaw_increment_in_degrees = 0.5f;
static void increment_camera_yaw() {
	camera.yaw += yaw_increment_in_degrees;
	if (camera.yaw > 360.0f) {
		camera.yaw -= 360.0f;
	}
	update_camera_direction();
}

static void decrement_camera_yaw() {
	camera.yaw -= yaw_increment_in_degrees;
	if (camera.yaw < 0.0f) {
		camera.yaw += 360.0f;
	}
	update_camera_direction();
}

static int UpdateAndRender1(const tl::GameMemory& gameMemory, const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
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
			ResetCamera();
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


	// First process any change in yaw and update the camera direction
	if (input.buttons[tl::KEY_D].isDown) {
		decrement_camera_yaw();
	}
	else if (input.buttons[tl::KEY_A].isDown) {
		increment_camera_yaw();
	}

	// Next process any forwards or backwards movement
	tl::Vec3<float> cameraPositionForwardBack = MultiplyVectorByScalar(
		tl::Vec3<float>{ camera.direction.x, camera.direction.y, camera.direction.z },
		positionIncrement);
	if (input.buttons[tl::KEY_S].isDown)
	{
		tl::Vec3<float> temp = SubtractVectors(
			tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
			cameraPositionForwardBack
		);
		camera.position.x = temp.x;
		camera.position.y = temp.y;
		camera.position.z = temp.z;
	}
	else if (input.buttons[tl::KEY_W].isDown)
	{
		tl::Vec3<float> temp = AddVectors(
			tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
			cameraPositionForwardBack
		);
		camera.position.x = temp.x;
		camera.position.y = temp.y;
		camera.position.z = temp.z;
	}

	// Strafing - use the cross product between the camera direction and up to get a normal vector to the direction being faced
	tl::Vec3<float> rawCameraPositionStrafe = CrossProduct(
		tl::Vec3<float>{ camera.up.x, camera.up.y, camera.up.z },
		tl::Vec3<float>{ camera.direction.x, camera.direction.y, camera.direction.z }
	);
	tl::Vec3<float> cameraPositionStrafe = MultiplyVectorByScalar(rawCameraPositionStrafe, positionIncrement);
	if (input.buttons[tl::KEY_LEFT].isDown)
	{
		tl::Vec3<float> temp = SubtractVectors(
			tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
			cameraPositionStrafe
		);
		camera.position.x = temp.x;
		camera.position.y = temp.y;
		camera.position.z = temp.z;
	}
	else if (input.buttons[tl::KEY_RIGHT].isDown)
	{
		tl::Vec3<float> temp = AddVectors(
			tl::Vec3<float>{ camera.position.x, camera.position.y, camera.position.z },
			cameraPositionStrafe
		);
		camera.position.x = temp.x;
		camera.position.y = temp.y;
		camera.position.z = temp.z;
	}

	// Simply move the camera position vertically with up/down keypress
	if (input.buttons[tl::KEY_DOWN].isDown)
	{
		camera.position.y -= positionIncrement;
	}
	else if (input.buttons[tl::KEY_UP].isDown)
	{
		camera.position.y += positionIncrement;
	}

	else if (input.buttons[tl::KEY_J].isDown) {
		camera.field_of_view_deg += 0.25f;
		if (camera.field_of_view_deg > 170.0f) {
			camera.field_of_view_deg = 170.0f;
		}
		set_projection_matrix();
	}
	else if (input.buttons[tl::KEY_K].isDown) {
		camera.field_of_view_deg -= 0.25f;
		if (camera.field_of_view_deg < 10.0f) {
			camera.field_of_view_deg = 10.0f;
		}
		set_projection_matrix();
	}

	else if (input.buttons[tl::KEY_V].isDown) {
		camera.near_plane += 0.1f;
		set_projection_matrix();
	}
	else if (input.buttons[tl::KEY_B].isDown) {
		camera.near_plane -= 0.1f;
		if (camera.near_plane < 0.0f) {
			camera.near_plane = 0.0f;
		}
		set_projection_matrix();
	}

	if (input.buttons[tl::KEY_C].keyUp)
	{
		ResetCamera();
	}

	// Final bounds check on the camera
	camera.position.x = Clamp(world.position.x - world.half_size.x, camera.position.x, world.position.x + world.half_size.x);
	camera.position.y = Clamp(world.position.y - world.half_size.y, camera.position.y, world.position.y + world.half_size.y);
	camera.position.z = Clamp(world.position.z - world.half_size.z, camera.position.z, world.position.z + world.half_size.z);

	tl::MemorySpace transientMemory = gameMemory.transient;

	TransformAndRenderMesh(renderBuffer, meshArray, camera, projectionMatrix, transientMemory);


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
