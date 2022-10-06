#include "./math.hpp"
#include <iostream>
#include <assert.h>

void RunMathTests()
{

	std::cout << "Running math tests." << std::endl;

	// AddVectors
	tl::Vec2<int> addVectorsResult = tl::AddVectors(tl::Vec2<int>{ 1, 2 }, tl::Vec2<int>{ 3, 4 });
	assert(addVectorsResult.x == 4);
	assert(addVectorsResult.y == 6);

	// SubtractVectos
	tl::Vec2<int> subtractVectorsResult = tl::SubtractVectors(tl::Vec2<int>{ 4, 6 }, tl::Vec2<int>{ 3, 4 });
	assert(subtractVectorsResult.x == 1);
	assert(subtractVectorsResult.y == 2);

	// dot_product
	float dot = tl::DotProduct(tl::Vec4<float>{ 1.0f, 2.0f, 3.0f }, tl::Vec4<float>{ 4.0f, 5.0f, 6.0f });
	assert(dot == (float)32);
}
