#include <iostream>
#include "../geometry.hpp"

void RunGeometryTests()
{
	std::cout << "Running geometry tests." << std::endl;

	// MakeIdentityMatrix
	tl::Matrix4x4<float> iMatrix = tl::MakeIdentityMatrix<float>();
	assert(iMatrix.m[0][0] == 1);

	// LookAt / PointAt test
	tl::Vec4<float> objectPosition = { 0.0f, 0.0f, 1.0f, 1.0f };
	tl::Vec4<float> cameraPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
	tl::Vec4<float> cameraDirection = { 1.0f, 0.0f, 0.0f, 1.0f };
	tl::Vec4<float> up = { 0.0f, 1.0f, 0.0f, 1.0f };
	
	// Camera matrix
	tl::Vec4<float> target = tl::AddVectors(cameraPosition, cameraDirection);
	tl::Matrix4x4<float> cameraMatrix = tl::PointAt(cameraPosition, target, up);

	// View matrix
	tl::Matrix4x4<float> viewMatrix = tl::LookAt(cameraMatrix);

	tl::Vec4<float> objectPositionInViewSpace;
	tl::MultiplyVectorWithMatrix(objectPosition, objectPositionInViewSpace, viewMatrix);

	assert(objectPositionInViewSpace.z == 0.0f);
	assert(objectPositionInViewSpace.y == 0.0f);
	assert(objectPositionInViewSpace.x == -1.0f);
}
