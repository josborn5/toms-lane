#include <math.h>

#include "../platform/toms-lane-platform.hpp"
#include "demo-win32.cpp"

tl::Camera<float> camera;
tl::Mesh<float> mesh;
tl::Matrix4x4<float> projectionMatrix;

float theta = 0.0f;
float cameraYaw = 0.0f;

bool isTeapot = false;

float maxX = 0.0f;
float minX = 0.0f;
float maxY = 0.0f;
float minY = 0.0f;
float maxZ = 0.0f;
float minZ = 0.0f;

bool isStarted = false;

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
		if (tri.p[0].x > maxX) maxX = tri.p[0].x;
		if (tri.p[0].x < minX) minX = tri.p[0].x;
		if (tri.p[0].y > maxY) maxY = tri.p[0].y;
		if (tri.p[0].y < minY) minY = tri.p[0].y;
		if (tri.p[0].z > maxZ) maxZ = tri.p[0].z;
		if (tri.p[0].z < minZ) minZ = tri.p[0].z;

		if (tri.p[1].x > maxX) maxX = tri.p[1].x;
		if (tri.p[1].x < minX) minX = tri.p[1].x;
		if (tri.p[1].y > maxY) maxY = tri.p[1].y;
		if (tri.p[1].y < minY) minY = tri.p[1].y;
		if (tri.p[1].z > maxZ) maxZ = tri.p[1].z;
		if (tri.p[1].z < minZ) minZ = tri.p[1].z;

		if (tri.p[2].x > maxX) maxX = tri.p[2].x;
		if (tri.p[2].x < minX) minX = tri.p[2].x;
		if (tri.p[2].y > maxY) maxY = tri.p[2].y;
		if (tri.p[2].y < minY) minY = tri.p[2].y;
		if (tri.p[2].z > maxZ) maxZ = tri.p[2].z;
		if (tri.p[2].z < minZ) minZ = tri.p[2].z;
	}

	// Center in x & y directions. Step back in the z direction.
	float startingX = 0.5f * (maxX + minX);
	float startingY = 0.5f * (maxY + minY);
	float zDepth = maxZ - minZ;
	float startingZ = minZ - zDepth;

	// Initialize the projection matrix
	projectionMatrix = tl::MakeProjectionMatrix(90.0f, 1.0f, 0.1f, 1000.0f);

	// Initialize the camera
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.position = { startingX, startingY, startingZ };
	camera.direction = { 0.0f, 0.0f, 1.0f };
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
		tl::DrawAlphabetCharacters(renderBuffer, "TOMS LANE", titleCharRect, 0x999999);

		if (input.buttons[KEY_S].isDown)
		{
			isStarted = true;
		}
		return;
	}

	float positionIncrement = 1.0f;
	if (!isTeapot) positionIncrement = 0.1f;
	float yawIncrement = 0.05f;
	float zOffset = 150.0f;
	if (!isTeapot) zOffset = 15.0f;

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

	tl::ClearScreen(renderBuffer, BACKGROUND_COLOR);

	theta += dt;
	// Initialize the rotation matrices
	tl::Matrix4x4<float> rotationMatrixX = tl::MakeXAxisRotationMatrix(theta);
	tl::Matrix4x4<float> rotationMatrixY = tl::MakeYAxisRotationMatrix(theta);
	tl::Matrix4x4<float> rotationMatrixZ = tl::MakeZAxisRotationMatrix(theta);

	// Initialize the translation matrix
	// Push back away from the camera which is implicitly located at z: 0. This ensures we're not trying to render trinagles behind the camera
	tl::Matrix4x4<float> translationMatrix = tl::MakeTranslationMatrix(0.0f, 0.0f, zOffset);

	// Combine all the rotation and translation matrices into a single world transfomration matrix
	tl::Matrix4x4<float> worldMatrix;
	// worldMatrix = MultiplyMatrixWithMatrix(rotationMatrixZ, rotationMatrixX);
	worldMatrix = tl::MakeIdentityMatrix<float>();
	worldMatrix = tl::MultiplyMatrixWithMatrix(worldMatrix, translationMatrix);

	tl::TransformAndRenderMesh(renderBuffer, mesh, camera, worldMatrix, projectionMatrix);
}

