#include "./internal/math.tests.cpp"
#include "./internal/geometry.tests.cpp"
#include "./internal/software-rendering.tests.cpp"

int main()
{
	RunMathTests();

	RunGeometryTests();

	RunSoftwareRenderingTests();
}
