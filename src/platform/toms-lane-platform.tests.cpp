#include "./internal/math.tests.cpp"
#include "./internal/geometry.tests.cpp"
#include "./internal/software-rendering.tests.cpp"
#include "./internal/collision.tests.cpp"
#include "./internal/data-structures.tests.cpp"
#include "./internal/utilities.tests.cpp"

int main()
{
	RunMathTests();

	RunGeometryTests();

	RunSoftwareRenderingTests();

	RunCollisionTests();

	RunDataStructureTests();

	RunUtilitiesTests();

	printf("!!! ALL TESTS COMPLETE !!!");
}

