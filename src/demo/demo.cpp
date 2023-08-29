#include <math.h>

#include "../platform/toms-lane-platform.hpp"
#include "./file.cpp"
#include "./demo-win32.cpp"

tl::Camera<float> camera;
tl::MeshArray<float> meshArray = tl::MeshArray<float>();
tl::Matrix4x4<float> projectionMatrix;

float theta = 0.0f;
float cameraYaw = 0.0f;
float positionIncrement = 1.0f;

tl::Vec3<float> max = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> min = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> startPosition = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> startDirection = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };
tl::Vec3<float> meshCenter = tl::Vec3<float> { 0.0f, 0.0f, 0.0f };

tl::Rect<float> map;
tl::Matrix2x3<float> mapProjectionMatrix;

bool isStarted = false;

tl::Matrix2x3<float> GenerateProjectionMatrix(const tl::Rect<float> &from, const tl::Rect<float> &to)
{
	float toMaxY = to.position.y + to.halfSize.y;
	float toMinY = to.position.y - to.halfSize.y;

	float toMaxX = to.position.x + to.halfSize.x;
	float toMinX = to.position.x - to.halfSize.x;

	float fromMaxY = from.position.y + from.halfSize.y;
	float fromMinY = from.position.y - from.halfSize.y;

	float fromMaxX = from.position.x + from.halfSize.x;
	float fromMinX = from.position.x - from.halfSize.x;

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

int tl::Initialize(const GameMemory& gameMemory, const RenderBuffer& renderBuffer)
{
	tl::MemorySpace transientMemory = gameMemory.transient;
	tl::MemorySpace permanentMemory = gameMemory.permanent;
	meshArray.triangles.initialize(permanentMemory);
	
	// EXE must be run as admin in order to have read permission for the file. need to figure out how to fix this.
	if (!tl::ReadObjFileToArray4("./teapot.obj", meshArray.triangles, transientMemory))
	{
		// Fall back to cube if file cannot be read
		// Using a clockwise winding convention
		/* meshArray.triangles.content = {
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
		}; */
		positionIncrement = 0.1f;
	}

	for (int i = 0; i < meshArray.triangles.length(); i += 1)
	{
		Triangle4d<float> tri = meshArray.triangles.content[i];
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
	topDownWorld.position = Vec2<float> { meshCenter.z, meshCenter.y };
	topDownWorld.halfSize = Vec2<float> { (0.5f * (max.z - min.z)), (0.5f * (max.y - min.y)) };
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

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	const uint32_t BACKGROUND_COLOR = 0x000000;
	tl::ClearScreen(renderBuffer, BACKGROUND_COLOR);
	if (!isStarted)
	{
		tl::Rect<float> titleCharRect = tl::Rect<float>();
		titleCharRect.position = { 100.0f, 350.0f };
		titleCharRect.halfSize = { 20.0f, 30.0f };
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"TL DEMO",
			titleCharRect,
			0x999999
		);

		tl::Rect<float> subtitleCharRect;
		subtitleCharRect.position = { 100.0f, 250.0f };
		subtitleCharRect.halfSize = MultiplyVectorByScalar(titleCharRect.halfSize, 0.5f);
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"RUN AS ADMIN FOR TEAPOT",
			subtitleCharRect,
			0x999999
		);

		subtitleCharRect.position.y -= 100.0f;
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"PRESS S TO START",
			subtitleCharRect,
			0x999999
		);

		if (input.buttons[KEY_S].isDown)
		{
			isStarted = true;
			ResetCamera();
		}
		return 0;
	}

	if (input.buttons[KEY_R].isDown)
	{
		isStarted = false;
	}

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
	tl::Vec4<float> rawCameraPositionStrafe = CrossProduct(camera.up, camera.direction);
	tl::Vec4<float> cameraPositionStrafe = MultiplyVectorByScalar(rawCameraPositionStrafe, positionIncrement);
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

	tl::MemorySpace transientMemory = gameMemory.transient;

	tl::TransformAndRenderMesh(renderBuffer, meshArray, camera, worldMatrix, projectionMatrix, transientMemory);


	// Show info about z-position
	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 100.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };

	tl::DrawAlphabetCharacters(renderBuffer, "MAX", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)max.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)max.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)max.z, charFoot, 0xAAAAAA);

	charFoot.position = { 200.0f, infoHeight };
	tl::DrawAlphabetCharacters(renderBuffer, "MIN", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)min.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)min.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)min.z, charFoot, 0xAAAAAA);

	charFoot.position = { 300.0f, infoHeight };
	tl::DrawAlphabetCharacters(renderBuffer, "POS", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)camera.position.x, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)camera.position.y, charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)camera.position.z, charFoot, 0xAAAAAA);

	charFoot.position = { 400.0f, infoHeight };
	tl::DrawAlphabetCharacters(renderBuffer, "MESH", charFoot, 0xAAAAAA);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, meshArray.triangles.length(), charFoot, 0xAAAAAA);

	// Draw the map
	tl::DrawRect(renderBuffer, 0x333399, map);


	tl::Rect<float> mapCamera;
	mapCamera.halfSize = tl::Vec2<float>{ 4.0f, 4.0f };
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
	tl::DrawNumber(renderBuffer, (int)pointPosition.x, charFoot, 0xAAAAAA);

	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, (int)mapCameraPointPosition.y, charFoot, 0xAAAAAA);

	mapCamera.position = Transform2DVector(topDownCameraPosition, mapProjectionMatrix);
	tl::DrawRect(renderBuffer, 0x993333, mapCamera);
	tl::DrawLineInPixels(
		renderBuffer,
		0x993333,
		mapCamera.position,
		mapCameraPointPosition
	);

	return 0;
}

