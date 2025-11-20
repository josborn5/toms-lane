#include <assert.h>
#include "./math.hpp"

void run_math_tests() {
	float actual_distance = ShortestDistanceFromPointToPlane(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f}
	);
	assert(actual_distance == 2.0f);

	actual_distance = ShortestDistanceFromPointToPlane(
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f}
	);
	assert(actual_distance == 0.0f);
}

