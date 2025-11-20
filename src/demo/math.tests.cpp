#include <assert.h>
#include "./math.hpp"

void run_math_tests() {
	float actual_distance = 0.0f;

	// x-axis normal to plane
	actual_distance = ShortestDistanceFromPointToPlane(
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

	actual_distance = ShortestDistanceFromPointToPlane(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f }
	);
	assert(actual_distance == -2.0f);

	// y-axis normal to plane
	actual_distance = ShortestDistanceFromPointToPlane(
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	);
	assert(actual_distance == 2.0f);

	actual_distance = ShortestDistanceFromPointToPlane(
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f }
	);
	assert(actual_distance == -2.0f);

	// z-axis normal to plane
	actual_distance = ShortestDistanceFromPointToPlane(
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 1.0f }
	);
	assert(actual_distance == 2.0f);

	actual_distance = ShortestDistanceFromPointToPlane(
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f }
	);
	assert(actual_distance == -2.0f);

	// angled

	actual_distance = ShortestDistanceFromPointToPlane(
		{ 1.0f, 1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		tl::Vec3<float>{ -1.0f, -1.0f, -1.0f }
	);
	assert(actual_distance < 0.0f);
}

