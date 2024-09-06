#include "./transform.hpp"

void RunTransformTests()
{
	Rect<float> largeRect;
	largeRect.position = { 0.0f, 0.0f };
	largeRect.halfSize = { 80.0f, 50.0f };

	Rect<float> smallRect;
	smallRect.position = { 0.0f, 0.0f };
	smallRect.halfSize = { 8.0f, 5.0f };

	Matrix2x3<float> transformMatrix;
}
