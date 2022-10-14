#include <math.h>

#include "../platform/toms-lane-platform.hpp"
#include "demo-win32.cpp"

tl::Camera<float> camera;
tl::Mesh<float> mesh;
tl::Matrix4x4<float> projectionMatrix;

float theta = 0.0f;
float cameraYaw = 0.0f;

bool isTeapot = false;

tl::Vec3<float> max = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> min = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> startPosition = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> startDirection = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> meshCenter = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };

tl::Rect<float> map;

bool isStarted = false;

template <typename T>
static T Clamp(T min, T value, T max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
template float Clamp(float min, float value, float max);

void tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{
	tl::ReadObjFileToVec4("teapot.obj", mesh.triangles);
	
	// Using a clockwise winding convention
	if (!isTeapot)
	{
		mesh.triangles = {
			// SOUTH
			{ 0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f },

			// EAST
			{ 1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f },

			// NORTH
			{ 1.0f, 0.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f },

			// WEST
			{ 0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f },

			// TOP
			{ 0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f, 1.0f },

			// BOTTOM
			{ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f }
		};
	}

	for (Triangle4d<float> tri : mesh.triangles)
	{
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
	map.position = { 900.0f, 200.0f };
	map.halfSize = { 100.0f, 50.0f };
}

static void ResetCamera()
{
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.position = { startPosition.x, startPosition.y, startPosition.z };
	camera.direction = { startDirection.x, startDirection.y, startDirection.z };
	cameraYaw = 0.0f;
}

void tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	const uint32_t BACKGROUND_COLOR = 0x000000;
	tl::ClearScreen(renderBuffer, BACKGROUND_COLOR);
	if (!isStarted)
	{
		tl::Rect<float> titleCharRect = tl::Rect<float>();
		titleCharRect.position = { 100.0f, 350.0f };
		titleCharRect.halfSize = { 20.0f, 30.0f };
		tl::DrawAlphabetCharacters(renderBuffer, "TL DEMO", titleCharRect, 0x999999);

		// TODO: Plot the max & min x, y, z values of the mesh

		if (input.buttons[KEY_S].isDown)
		{
			isStarted = true;
			ResetCamera();
		}
		return;
	}

	if (input.buttons[KEY_R].isDown)
	{
		isStarted = false;
	}

	float positionIncrement = 1.0f;
	if (!isTeapot) positionIncrement = 0.1f;
	float yawIncrement = 0.05f;

	// First process any change in yaw and update the camera direction
	if (input.buttons[KEY_D].isDown)
	{
		cameraYaw -= yawIncrement;
	}
	else if (input.buttons[KEY_A].isDown)
	{
		cameraYaw += yawIncrement;
	}

	// Apply the camera yaw to the camera.direction vector
	tl::Vec4<float> target = { 0.0f, 0.0f, 1.0f };
	tl::Matrix4x4<float> cameraYawMatrix = tl::MakeYAxisRotationMatrix(cameraYaw);
	tl::MultiplyVectorWithMatrix(target, camera.direction, cameraYawMatrix);

	// Next process any forwards or backwards movement
	tl::Vec4<float> cameraPositionForwardBack = MultiplyVectorByScalar(camera.direction, positionIncrement);
	if (input.buttons[KEY_S].isDown)
	{
		camera.position = SubtractVectors(camera.position, cameraPositionForwardBack);
	}
	else if (input.buttons[KEY_W].isDown)
	{
		camera.position = AddVectors(camera.position, cameraPositionForwardBack);
	}

	// Strafing - use the cross product between the camera direction and up to get a normal vector to the direction being faced
	tl::Vec4<float> cameraPositionStrafe = CrossProduct(camera.up, camera.direction);
	if (input.buttons[KEY_LEFT].isDown)
	{
		camera.position = SubtractVectors(camera.position, cameraPositionStrafe);
	}
	else if (input.buttons[KEY_RIGHT].isDown)
	{
		camera.position = AddVectors(camera.position, cameraPositionStrafe);
	}

	// Simply move the camera position vertically with up/down keypress
	if (input.buttons[KEY_DOWN].isDown)
	{
		camera.position.y -= positionIncrement;
	}
	else if (input.buttons[KEY_UP].isDown)
	{
		camera.position.y += positionIncrement;
	}

	if (input.buttons[KEY_C].isDown)
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

	tl::TransformAndRenderMesh(renderBuffer, mesh, camera, worldMatrix, projectionMatrix);


	// Show info about z-position
	tl::Vec2<float> charHalfSize = { 4.0f, 8.0f };
	tl::Vec2<float> charPos = { 400.0f, 400.0f };
	tl::Rect<float> charFoot;
	charFoot.position = charPos;
	charFoot.halfSize = charHalfSize;
	tl::DrawNumber(renderBuffer, (int)camera.position.z, charFoot, 0xAAAAAA);

	// Draw the map
	tl::DrawRect(renderBuffer, 0x333399, map);

	float mapMaxY = map.position.y + map.halfSize.y;
	float mapMinY = map.position.y - map.halfSize.y;

	float mapYGrad = (mapMinY - mapMaxY) / (min.x - max.x);
	float mapYOffset = mapMaxY - (mapYGrad * max.x);

	float mapMaxX = map.position.x + map.halfSize.x;
	float mapMinX = map.position.x - map.halfSize.x;

	float mapXGrad = (mapMinX - mapMaxX) / (min.z - max.z);
	float mapXOffset = mapMaxX - (mapXGrad * max.z);

	tl::Rect<float> mapCamera;
	mapCamera.halfSize = tl::Vec2<float>{ 4.0f, 4.0f };
	float mapCameraX = (mapXGrad * camera.position.z) + mapXOffset;
	float mapCameraY = (mapYGrad * camera.position.x) + mapYOffset;
	mapCamera.position = { mapCameraX, mapCameraY };
	tl::DrawRect(renderBuffer, 0x993333, mapCamera);
}


