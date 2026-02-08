#include "./math.tests.cpp"
#include "./software-rendering.tests.cpp"
#include "./collision.tests.cpp"
#include "./data-structures.tests.cpp"
#include "./utilities.tests.cpp"
#include "./quad-tree.tests.cpp"
#include "./bitmap.tests.cpp"
#include "./transform.tests.cpp"

int main()
{
	RunMathTests();

	RunSoftwareRenderingTests();

	RunCollisionTests();

	RunDataStructureTests();

	RunUtilitiesTests();

	RunQuadTreeTests();

	RunBitmapTests();

	RunTransformTests();

	printf("!!! ALL TESTS COMPLETE !!!");
}

