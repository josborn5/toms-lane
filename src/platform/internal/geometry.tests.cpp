#include <iostream>
#include "geometry.hpp"

void RunGeometryTests()
{
	std::cout << "Running geometry tests." << std::endl;

	// LookAt / PointAt test
	tl::Vec4<float> objectPosition = { 0.0f, 0.0f, 1.0f, 1.0f };
	tl::Vec4<float> cameraPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
	tl::Vec4<float> cameraDirection = { 1.0f, 0.0f, 0.0f, 1.0f };
	tl::Vec4<float> up = { 0.0f, 1.0f, 0.0f, 1.0f };
	
	// Camera matrix
	tl::Vec4<float> target = tl::AddVectors(cameraPosition, cameraDirection);
	tl::Matrix4x4<float> cameraMatrix = tl::PointAt<float>(cameraPosition, target, up);

	// View matrix
	tl::Matrix4x4<float> viewMatrix = tl::LookAt<float>(cameraMatrix);

	tl::Vec4<float> objectPositionInViewSpace;
	tl::MultiplyVectorWithMatrix(objectPosition, objectPositionInViewSpace, viewMatrix);

	assert(objectPositionInViewSpace.z == 0.0f);
	assert(objectPositionInViewSpace.y == 0.0f);
	assert(objectPositionInViewSpace.x == -1.0f);


	// IntersectPlane test
	tl::Plane<float> plane;
	plane.position = { 0.0f, 1.0f, 0.0f };
	plane.normal = { 0.0f, -1.0f, 0.0f };
	tl::Vec4<float> lineStart = { 0.0f, 0.0f, 0.0f, 1.0f };
	tl::Vec4<float> lineEnd = { 0.0f, 2.0f, 0.0f, 1.0f };

	tl::Vec4<float> result = tl::IntersectPlane(plane, lineStart, lineEnd);

	assert(result.x == 0.0f);
	assert(result.y == 1.0f);
	assert(result.z == 0.0f);
}
