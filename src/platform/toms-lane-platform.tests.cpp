#include "./math.tests.cpp"
#include "./geometry.tests.cpp"
#include "./software-rendering.tests.cpp"
#include "./collision.tests.cpp"
#include "./data-structures.tests.cpp"
#include "./utilities.tests.cpp"
#include "./quad-tree.tests.cpp"

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

