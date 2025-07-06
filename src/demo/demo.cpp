#include <math.h>
#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./file.cpp"

template<typename T>
void TransformAndRenderMesh(
	const tl::RenderBuffer &renderBuffer,
	const tl::array<tl::Triangle4d<T>> &mesh,
	const tl::Camera<T> &camera,
	const tl::Matrix4x4<T>& transformMatrix,
	const tl::Matrix4x4<T>& projectionMatrix,
	const tl::MemorySpace& transient
) {
	const int RED = 0;
	const int GREEN = 255;
	const int BLUE = 0;

	// Camera matrix
	tl::Vec4<T> target = AddVectors(camera.position, camera.direction);
	tl::Matrix4x4<T> cameraMatrix = PointAt(camera.position, target, camera.up);

	// View matrix
	tl::Matrix4x4<T> viewMatrix = LookAt(cameraMatrix);

	tl::array<tl::Triangle4d<T>> trianglesToDrawArray = tl::array<tl::Triangle4d<T>>(transient);

	tl::Plane<T> bottomOfScreen = { (T)0, (T)0, (T)0,						(T)0, (T)1, (T)0 };
	tl::Plane<T> topOfScreen = { (T)0, (T)(renderBuffer.height - 1), (T)0,	(T)0, (T)-1, (T)0 };
	tl::Plane<T> leftOfScreen = { (T)0, (T)0, (T)0,							(T)1, (T)0, (T)0 };
	tl::Plane<T> rightOfScreen = { (T)(renderBuffer.width - 1), (T)0, (T)0,	(T)-1, (T)0, (T)0 };

	for (int h = 0; h < mesh.length(); h += 1)
	{
		tl::Triangle4d<T> tri = mesh.get(h);
		tl::Triangle4d<T> transformed;
		tl::Triangle4d<T> viewed;
		tl::Triangle4d<T> projected;

		// Transform the triangle in the mesh
		MultiplyVectorWithMatrix(tri.p[0], transformed.p[0], transformMatrix);
		MultiplyVectorWithMatrix(tri.p[1], transformed.p[1], transformMatrix);
		MultiplyVectorWithMatrix(tri.p[2], transformed.p[2], transformMatrix);

		// Work out the normal of the triangle
		tl::Vec4<T> line1 = SubtractVectors(transformed.p[1], transformed.p[0]);
		tl::Vec4<T> line2 = SubtractVectors(transformed.p[2], transformed.p[0]);
		tl::Vec4<T> normal = UnitVector(CrossProduct(line1, line2));

		tl::Vec4<T> fromCameraToTriangle = SubtractVectors(transformed.p[0], camera.position);
		T dot = DotProduct(normal, fromCameraToTriangle);

		if (dot >= (T)0)
		{
			tl::Vec4<T> lightDirection = { (T)0, (T)0, (T)1 };
			tl::Vec4<T> normalizedLightDirection = UnitVector(lightDirection);
			T shade = DotProduct(normal, normalizedLightDirection);

			uint32_t triangleColor = tl::GetColorFromRGB(int(RED * shade), int(GREEN * shade), int(BLUE * shade));

			// Convert the triangle position from world space to view space
			MultiplyVectorWithMatrix(transformed.p[0], viewed.p[0], viewMatrix);
			MultiplyVectorWithMatrix(transformed.p[1], viewed.p[1], viewMatrix);
			MultiplyVectorWithMatrix(transformed.p[2], viewed.p[2], viewMatrix);

			// Clip the triangles before they get projected. Define a plane just in fron of the camera to clip against
			tl::Triangle4d<T> clipped[2];
			tl::Plane<T> inFrontOfScreen = { (T)0, (T)0, (T)0.1,	 (T)0, (T)0, (T)1 };
			int clippedTriangleCount = ClipTriangleAgainstPlane(inFrontOfScreen, viewed, clipped[0], clipped[1]);

			for (int i = 0; i < clippedTriangleCount; i += 1)
			{
				// Project each triangle in 3D space onto the 2D space triangle to render
				Project3DPointTo2D(clipped[i].p[0], projected.p[0], projectionMatrix);
				Project3DPointTo2D(clipped[i].p[1], projected.p[1], projectionMatrix);
				Project3DPointTo2D(clipped[i].p[2], projected.p[2], projectionMatrix);

				// Scale to view
				const float sf = 500.0f;
				tl::Triangle4d<T> triToRender = projected;
				triToRender.p[0].x *= sf;
				triToRender.p[0].y *= sf;
				triToRender.p[1].x *= sf;
				triToRender.p[1].y *= sf;
				triToRender.p[2].x *= sf;
				triToRender.p[2].y *= sf;

				const T translateX = (T)0.5 * (T)renderBuffer.width;
				const T translateY = (T)0.5 * (T)renderBuffer.height;
				triToRender.p[0].x += translateX; triToRender.p[0].y += translateY;
				triToRender.p[1].x += translateX; triToRender.p[1].y += translateY;
				triToRender.p[2].x += translateX; triToRender.p[2].y += translateY;

				triToRender.color = triangleColor;

				trianglesToDrawArray.append(triToRender);
			}
		}
	}

	tl::MemorySpace remainingTransient = trianglesToDrawArray.sizeToCurrentLength();

	for (int n = 0; n < trianglesToDrawArray.length(); n += 1)
	{
		tl::Triangle4d<T> triToRender = trianglesToDrawArray.get(n);
		tl::Triangle4d<T> clipped[2];

		tl::queue<tl::Triangle4d<T>> triangleQueue = tl::queue<tl::Triangle4d<T>>(remainingTransient);
		if (triangleQueue.enqueue(triToRender) != 0) throw; // TODO: don't throw, handle gracefully

		int newTriangles = 1;

		// Clip against each screen edge
		for (int edge = 0; edge < 4; edge += 1)
		{
			int trianglesToAdd = 0;
			while (newTriangles > 0)
			{
				tl::Triangle4d<T> test = triangleQueue.dequeue();
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
			tl::Triangle4d<T> draw = triangleQueue.content[i];
			// Vec2<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
			// Vec2<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
			// Vec2<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };
			// DrawTriangleInPixels(renderBuffer, 0xFFFFFF, p0Int, p1Int, p2Int);

			tl::Vec3<int> p0Int = { (int)draw.p[0].x, (int)draw.p[0].y };
			tl::Vec3<int> p1Int = { (int)draw.p[1].x, (int)draw.p[1].y };
			tl::Vec3<int> p2Int = { (int)draw.p[2].x, (int)draw.p[2].y };

			// Super rough, take the depth as the average z value
			// For whatever reason, the z values are inverted for the teapot. i.e. closer triangles have a lower Z value.
			// As an ultra-hack I'm doing 10 minus the z-value to invert them.
			T z = (T)10 - ((draw.p[0].z + draw.p[1].z + draw.p[2].z) / (T)3);
			FillTriangleInPixels(renderBuffer, draw.color, p0Int, p1Int, p2Int, z);
		}
	}
}
tl::Camera<float> camera;
tl::array<tl::Triangle4d<float>> meshArray = tl::array<tl::Triangle4d<float>>();
tl::Matrix4x4<float> projectionMatrix;

float theta = 0.0f;
float cameraYaw = 0.0f;
float positionIncrement = 0.1f;

tl::Vec3<float> max = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> min = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> startPosition = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> startDirection = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> meshCenter = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };

tl::Rect<float> map;
tl::Matrix2x3<float> mapProjectionMatrix;

tl::GameMemory appMemory;

bool isStarted = false;

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

static int Initialize(const tl::GameMemory& gameMemory)
{
	tl::MemorySpace transientMemory = gameMemory.transient;
	tl::MemorySpace permanentMemory = gameMemory.permanent;
	tl::font_interface_initialize();

	meshArray.initialize(permanentMemory);

	// EXE must be run as admin in order to have read permission for the file. need to figure out how to fix this.
	if (!tl::ReadObjFileToArray4("teapot.obj", meshArray, transientMemory))
	{
		// Fall back to cube if file cannot be read
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
		positionIncrement = 0.1f;
	}

	for (int i = 0; i < meshArray.length(); i += 1)
	{
		tl::Triangle4d<float> tri = meshArray.get(i);
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
	depth.z = max.z - min.z;
	depth.y = max.y - min.y;
	depth.x = max.x - min.x;

	meshCenter.x = 0.5f * (max.x + min.x);
	meshCenter.y = 0.5f * (max.y + min.y);
	meshCenter.z = 0.5f * (max.z + min.z);

	// Start position is centered in x & y directions and stepped back in the z direction.
	startPosition.z = min.z - depth.z;
	startPosition.y = meshCenter.y;
	startPosition.x = meshCenter.x;

	startDirection = tl::SubtractVectors(meshCenter, startPosition);

	// set the bounds of the camera
	max.z += 2.0f * depth.z;
	min.z -= 2.0f * depth.z;
	max.y += 2.0f * depth.y;
	min.y -= 2.0f * depth.y;
	max.x += 2.0f * depth.x;
	min.x -= 2.0f * depth.x;

	// Initialize the projection matrix
	projectionMatrix = tl::MakeProjectionMatrix(90.0f, 1.0f, 0.1f, 1000.0f);

	// Initialize the map
	map.position = { 1100.0f, 75.0f };
	map.halfSize = { 100.0f, 50.0f };

	// Initialize the projection matrix for world to map
	tl::Rect<float> topDownWorld = tl::Rect<float>();

	// Using a top down projection for the map view.
	// So depth (z) in the world --> horizontal (x) on the screen map.
	// Left/right in the world (x) --> vertical (y) on the screen map.
	topDownWorld.position = tl::Vec2<float> { meshCenter.z, meshCenter.y };
	topDownWorld.halfSize = tl::Vec2<float> { (0.5f * (max.z - min.z)), (0.5f * (max.y - min.y)) };
	mapProjectionMatrix = GenerateProjectionMatrix(topDownWorld, map);

	return 0;
}

static void ResetCamera()
{
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.position = { startPosition.x, startPosition.y, startPosition.z };
	camera.direction = { startDirection.x, startDirection.y, startDirection.z };
	cameraYaw = 0.0f;
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

		if (input.buttons[tl::KEY_S].isDown)
		{
			isStarted = true;
			ResetCamera();
		}
		return 0;
	}

	if (input.buttons[tl::KEY_R].isDown)
	{
		isStarted = false;
	}

	float yawIncrement = 0.05f;

	// First process any change in yaw and update the camera direction
	if (input.buttons[tl::KEY_D].isDown)
	{
		cameraYaw -= yawIncrement;
	}
	else if (input.buttons[tl::KEY_A].isDown)
	{
		cameraYaw += yawIncrement;
	}

	// Apply the camera yaw to the camera.direction vector
	tl::Vec4<float> target = { 0.0f, 0.0f, 1.0f };
	tl::Matrix4x4<float> cameraYawMatrix = tl::MakeYAxisRotationMatrix(cameraYaw);
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

	if (input.buttons[tl::KEY_C].isDown)
	{
		ResetCamera();
	}

	theta += dt;

	tl::Matrix4x4<float> worldMatrix;
	worldMatrix = tl::MakeIdentityMatrix<float>();

	// Final bounds check on the camera
	camera.position.x = Clamp(min.x, camera.position.x, max.x);
	camera.position.y = Clamp(min.y, camera.position.y, max.y);
	camera.position.z = Clamp(min.z, camera.position.z, max.z);

	tl::MemorySpace transientMemory = gameMemory.transient;

	TransformAndRenderMesh(renderBuffer, meshArray, camera, worldMatrix, projectionMatrix, transientMemory);


	// Show info about z-position
	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 100.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };

	tl::font_interface_render_chars(renderBuffer, "MAX", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)max.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)max.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)max.z, charFoot, 0xAAAAAA);

	charFoot.position = { 200.0f, infoHeight };
	tl::font_interface_render_chars(renderBuffer, "MIN", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)min.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)min.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)min.z, charFoot, 0xAAAAAA);

	charFoot.position = { 300.0f, infoHeight };
	tl::font_interface_render_chars(renderBuffer, "POS", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.position.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.position.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)camera.position.z, charFoot, 0xAAAAAA);

	charFoot.position = { 400.0f, infoHeight };
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

	float pointerScale = 0.1f * (max.z - min.z);
	tl::Vec4<float> pointPosition = tl::AddVectors(camera.position, tl::MultiplyVectorByScalar(camera.direction, pointerScale));
	tl::Vec2<float> topDownPointPosition = {
		pointPosition.z,
		pointPosition.x
	};
	tl::Vec2<float> mapCameraPointPosition = Transform2DVector(topDownPointPosition, mapProjectionMatrix);

	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)pointPosition.x, charFoot, 0xAAAAAA);

	charFoot.position.y -= fontSize;
	tl::font_interface_render_int(renderBuffer, (int)mapCameraPointPosition.y, charFoot, 0xAAAAAA);

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
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Demo";
	settings.width = 1280;
	settings.height = 720;

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::InitializeMemory(
		100,
		100,
		appMemory
	);

	Initialize(appMemory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
