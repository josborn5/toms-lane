#include "./internal/math.tests.cpp"
#include "./internal/geometry.tests.cpp"
#include "./internal/software-rendering.tests.cpp"
#include "./internal/collision.tests.cpp"
#include "./internal/data-structures.tests.cpp"
#include "./internal/utilities.tests.cpp"
#include "./internal/quad-tree.tests.cpp"

int main()
{
	RunMathTests();

	RunGeometryTests();

	RunSoftwareRenderingTests();

	RunCollisionTests();

	RunDataStructureTests();

	RunUtilitiesTests();

	RunQuadTreeTests();

	printf("!!! ALL TESTS COMPLETE !!!");
}

