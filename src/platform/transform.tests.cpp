#include "./transform.hpp"
#include <stdio.h>
#include <assert.h>

static void assertEqual(float actual, float expected)
{
	printf("actual: %2.2f, expected: %2.2f\n", actual, expected);
	assert(actual == expected);
}

void RunTransformTests()
{
	Rect<float> cameraRender;
	cameraRender.position = { 0.0f, 0.0f };
	cameraRender.halfSize = { 80.0f, 50.0f };

	Rect<float> cameraWorld;
	cameraWorld.position = { 0.0f, 0.0f };
	cameraWorld.halfSize = { 8.0f, 5.0f };

	Matrix2x3<float> worldToRenderProjection;

	tl::transform_interface_create_2d_projection_matrix(
		cameraWorld,
		cameraRender,
		worldToRenderProjection
	);

	assertEqual(worldToRenderProjection.m[0][0], 10.0f);
	assertEqual(worldToRenderProjection.m[0][1], 0.0f);
	assertEqual(worldToRenderProjection.m[0][2], 0.0f);

	assertEqual(worldToRenderProjection.m[1][0], 0.0f);
	assertEqual(worldToRenderProjection.m[1][1], 10.0f);
	assertEqual(worldToRenderProjection.m[1][2], 0.0f);

	Rect<float> bottomLeftWorld;
	bottomLeftWorld.halfSize = { 3.0f, 2.0f };
	bottomLeftWorld.position.x = cameraWorld.position.x - cameraWorld.halfSize.x + bottomLeftWorld.halfSize.x;
	bottomLeftWorld.position.y = cameraWorld.position.y - cameraWorld.halfSize.y + bottomLeftWorld.halfSize.y;

	Rect<float> bottomLeftRender;
	tl::transform_interface_project_rect(
		worldToRenderProjection,
		bottomLeftWorld,
		bottomLeftRender
	);

	assertEqual(bottomLeftRender.position.x, -50.0f);
	assertEqual(bottomLeftRender.position.y, -30.0f);

	assertEqual(bottomLeftRender.halfSize.x, 30.0f);
	assertEqual(bottomLeftRender.halfSize.y, 20.0f);
}
