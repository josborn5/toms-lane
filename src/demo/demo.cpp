#include <math.h>
#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./file.cpp"


struct cuboid {
	tl::Vec3<float> position = {0};
	tl::Vec3<float> half_size = {0};
};

struct Camera
{
	tl::Vec4<float> position = {0};
	tl::Vec4<float> direction = {0};
	tl::Vec4<float> up = {0};

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

static float theta = 0.0f;
static float positionIncrement = 0.0f;

static tl::Rect<float> map;
static float world_to_map_scale_factor = 0.0f;
static tl::Matrix2x3<float> mapProjectionMatrix;

static tl::GameMemory appMemory;

static bool isStarted = false;



tl::Vec4<float> IntersectPlane(
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

float ShortestDistanceFromPointToPlane(
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

int ClipTriangleAgainstPlane(
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


tl::Matrix4x4<float> MakeProjectionMatrix(
	float fieldOfVewDeg,
	float aspectRatio,
	float nearPlane,
	float farPlane
)
{
	float inverseTangent = 1.0f / tanf(fieldOfVewDeg * 0.5f * 3.14159f / 180.0f);

	tl::Matrix4x4<float> matrix;
	matrix.m[0][0] = inverseTangent;
	matrix.m[1][1] = aspectRatio * inverseTangent;
	matrix.m[2][2] = farPlane / (farPlane - nearPlane);
	matrix.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;

	return matrix;
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

	// Camera matrix
	tl::Vec4<float> target = AddVectors(camera.position, camera.direction);
	tl::Matrix4x4<float> cameraMatrix = PointAt(camera.position, target, camera.up);

	// View matrix
	tl::Matrix4x4<float> viewMatrix = LookAt(cameraMatrix);

	tl::array<Triangle4d> trianglesToDrawArray = tl::array<Triangle4d>(transient);

	Plane bottomOfScreen = { 0.0f, 0.0f, 0.0f,                             0.0f,  1.0f, 0.0f };
	Plane topOfScreen =    { 0.0f, (float)(renderBuffer.height - 1), 0.0f, 0.0f, -1.0f, 0.0f };
	Plane leftOfScreen =   { 0.0f, 0.0f, 0.0f,                             1.0f,  0.0f, 0.0f };
	Plane rightOfScreen =  { (float)(renderBuffer.width - 1), 0.0f, 0.0f, -1.0f,  0.0f, 0.0f };

	for (int h = 0; h < mesh.length(); h += 1)
	{
		Triangle4d tri = mesh.get(h);
		Triangle4d viewed;
		Triangle4d projected;

		// Skip any triangles angled away from the camera
		tl::Vec4<float> line1 = SubtractVectors(tri.p[1], tri.p[0]);
		tl::Vec4<float> line2 = SubtractVectors(tri.p[2], tri.p[0]);
		tl::Vec4<float> normal = UnitVector(CrossProduct(line1, line2));

		tl::Vec4<float> fromCameraToTriangle = SubtractVectors(tri.p[0], camera.position);
		float dot = DotProduct(normal, fromCameraToTriangle);
		if (dot > 0.0f)
		{
			continue;
		}

		// Convert the triangle position from world space to view space
		MultiplyVectorWithMatrix(tri.p[0], viewed.p[0], viewMatrix);
		MultiplyVectorWithMatrix(tri.p[1], viewed.p[1], viewMatrix);
		MultiplyVectorWithMatrix(tri.p[2], viewed.p[2], viewMatrix);

		const tl::Vec4<float> lightDirection = { 0.0f, 0.0f, -1.0f, 0.0f };
		tl::Vec4<float> normalizedLightDirection = UnitVector(lightDirection);
		float shade = 0.5f + (0.5f * DotProduct(normal, normalizedLightDirection));

		uint32_t triangleColor = tl::GetColorFromRGB(int(RED * shade), int(GREEN * shade), int(BLUE * shade));

		// Clip the triangles before they get projected. Define a plane just in fron of the camera to clip against
		Triangle4d clipped[2];

		// TODO: remove these hard coded z values - they need to relate to
		// near & far values used to generate the projection matric
		Plane inFrontOfScreen;
		inFrontOfScreen.position = { 0.0f, 0.0f, camera.near_plane };
		inFrontOfScreen.normal = { 0.0f, 0.0f, 1.0f };
		int clippedTriangleCount = ClipTriangleAgainstPlane(inFrontOfScreen, viewed, clipped[0], clipped[1]);

		for (int i = 0; i < clippedTriangleCount; i += 1)
		{
			// Project each triangle in 3D space onto the 2D space triangle to render
			Project3DPointTo2D(clipped[i].p[0], projected.p[0], projectionMatrix);
			Project3DPointTo2D(clipped[i].p[1], projected.p[1], projectionMatrix);
			Project3DPointTo2D(clipped[i].p[2], projected.p[2], projectionMatrix);

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

	for (int n = 0; n < trianglesToDrawArray.length(); n += 1)
	{
		Triangle4d triToRender = trianglesToDrawArray.get(n);
		Triangle4d clipped[2];

		tl::queue<Triangle4d> triangleQueue = tl::queue<Triangle4d>(remainingTransient);
		if (triangleQueue.enqueue(triToRender) != 0) throw; // TODO: don't throw, handle gracefully

		int newTriangles = 1;

		// Clip against each screen edge
		for (int edge = 0; edge < 4; edge += 1)
		{
			int trianglesToAdd = 0;
			while (newTriangles > 0)
			{
				Triangle4d test = triangleQueue.dequeue();
				newTriangles -= 1;

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

static void ResetCamera()
{
	camera.up = { 0.0f, 1.0f, 0.0f, 0.0f };
	// Start position is centered in x & y directions and stepped back in the z direction.
	camera.position = {
		world.position.x,
		world.position.y,
		world.position.z - world.half_size.z,
		0.0f
	};
	camera.direction = tl::SubtractVectors(tl::Vec4<float> { mesh.position.x, mesh.position.y, mesh.position.z, 0.0f }, camera.position);
	camera.yaw = 0.0f;
	camera.field_of_view_deg = 75.0f;
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

	// camera         near   object    far
	// 	 |             |    |------|    |
	//   |-------------|----------------|----> z
	//   0
	camera.far_plane = 2.0f * world.half_size.z; // should pick maximum length dimension
	camera.near_plane = 0.1f * camera.far_plane;


	set_projection_matrix();

	// Initialize the map
	world_to_map_scale_factor = 0.05f * (float)screen_width / world.half_size.z;
	map.halfSize = { world_to_map_scale_factor * world.half_size.z, world_to_map_scale_factor * world.half_size.x };
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

static void reset_mesh_to_teapot() {
	meshArray.clear();
	tl::MemorySpace temp = appMemory.transient; // make a copy of the transient memory so it can be modified
	ReadObjFileToArray4("teapot.obj", meshArray, temp);

	reset_world_to_mesh();
}

static void reset_mesh_to_cube() {
	meshArray.clear();
	// Using a clockwise winding convention
	// SOUTH
	meshArray.append({ 0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f });
	meshArray.append({ 0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f });

	// EAST
	meshArray.append({ 1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f });
	meshArray.append({ 1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f });

	// NORTH
	meshArray.append({ 1.0f, 0.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f });
	meshArray.append({ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f });

	// WEST
	meshArray.append({ 0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f });
	meshArray.append({ 0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f });

	// TOP
	meshArray.append({ 0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f });
	meshArray.append({ 0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f });

	// BOTTOM
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

	const float yawIncrement = 0.02f;

	// First process any change in yaw and update the camera direction
	if (input.buttons[tl::KEY_D].isDown)
	{
		camera.yaw -= yawIncrement;
		if (camera.yaw < 0.0f) {
			camera.yaw += 360.0f;
		}
	}
	else if (input.buttons[tl::KEY_A].isDown)
	{
		camera.yaw += yawIncrement;
		if (camera.yaw > 360.0f) {
			camera.yaw -= 360.0f;
		}
	}

	// Apply the camera yaw to the camera.direction vector
	tl::Vec4<float> target = { 0.0f, 0.0f, 1.0f };
	tl::Matrix4x4<float> cameraYawMatrix = tl::MakeYAxisRotationMatrix(camera.yaw);
	tl::MultiplyVectorWithMatrix(target, camera.direction, cameraYawMatrix);

	// Next process any forwards or backwards movement
	tl::Vec4<float> cameraPositionForwardBack = MultiplyVectorByScalar(camera.direction, positionIncrement);
	if (input.buttons[tl::KEY_S].isDown)
	{
		camera.position = SubtractVectors(camera.position, cameraPositionForwardBack);
	}
	else if (input.buttons[tl::KEY_W].isDown)
	{
		camera.position = AddVectors(camera.position, cameraPositionForwardBack);
	}

	// Strafing - use the cross product between the camera direction and up to get a normal vector to the direction being faced
	tl::Vec4<float> rawCameraPositionStrafe = CrossProduct(camera.up, camera.direction);
	tl::Vec4<float> cameraPositionStrafe = MultiplyVectorByScalar(rawCameraPositionStrafe, positionIncrement);
	if (input.buttons[tl::KEY_LEFT].isDown)
	{
		camera.position = SubtractVectors(camera.position, cameraPositionStrafe);
	}
	else if (input.buttons[tl::KEY_RIGHT].isDown)
	{
		camera.position = AddVectors(camera.position, cameraPositionStrafe);
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

	theta += dt;

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
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, meshArray.length(), charFoot, 0xAAAAAA);

	// Draw the map
	tl::DrawRect(renderBuffer, 0x333399, map);


	tl::Vec2<float> mapCameraPosition;
	tl::Vec2<float> topDownCameraPosition = {
		camera.position.z,
		camera.position.x
	};

	float pointerScale = 0.1f * world.half_size.z;
	tl::Vec4<float> pointPosition = tl::AddVectors(camera.position, tl::MultiplyVectorByScalar(camera.direction, pointerScale));
	tl::Vec2<float> topDownPointPosition = {
		pointPosition.z,
		pointPosition.x
	};
	tl::Vec2<float> mapCameraPointPosition = Transform2DVector(topDownPointPosition, mapProjectionMatrix);

	tl::Rect<float> mesh_footprint;
	mesh_footprint.position = Transform2DVector(tl::Vec2<float> { mesh.position.z, mesh.position.x }, mapProjectionMatrix);
	mesh_footprint.halfSize = { world_to_map_scale_factor * mesh.half_size.z, world_to_map_scale_factor * mesh.half_size.x };

	tl::DrawRect(renderBuffer, 0x00AA00, mesh_footprint);

	mapCameraPosition = Transform2DVector(topDownCameraPosition, mapProjectionMatrix);
	tl::DrawCircle(renderBuffer, 0x993333, mapCameraPosition, 10.0f);
	tl::DrawLineInPixels(
		renderBuffer,
		0x993333,
		mapCameraPosition,
		mapCameraPointPosition
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
	const int targetFPS = 120;

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
