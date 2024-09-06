#include "./transform.hpp"
#include <assert.h>

void RunTransformTests()
{
	Rect<float> largeRect;
	largeRect.position = { 0.0f, 0.0f };
	largeRect.halfSize = { 80.0f, 50.0f };

	Rect<float> smallRect;
	smallRect.position = { 0.0f, 0.0f };
	smallRect.halfSize = { 8.0f, 5.0f };

	Matrix2x3<float> transformMatrix;

	tl::transform_interface_create_2d_projection_matrix(
		smallRect,
		largeRect,
		transformMatrix
	);

	assert(transformMatrix.m[0][0] == 10.0f);
	assert(transformMatrix.m[0][1] == 0.0f);
	assert(transformMatrix.m[0][2] == 800.0f);

	assert(transformMatrix.m[1][0] == 0.0f);
	assert(transformMatrix.m[1][1] == 10.0f);
	assert(transformMatrix.m[1][2] == 500.0f);
}
