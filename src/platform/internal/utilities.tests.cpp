#include "./utilities.hpp"
#include <stdio.h>
#include <assert.h>

void RunUtilitiesTests()
{
	printf("\n===== Swap =====\n");
	int a = 3;
	int b = 2;
	int space;

	tl::MemorySpace testSpace;
	testSpace.content = &space;
	testSpace.sizeInBytes = sizeof(int);
	
	assert(tl::swap(a, b, testSpace) == 0);

	assert(a == 2);
	assert(b == 3);

	testSpace.sizeInBytes = sizeof(char);

	assert(tl::swap(a, b, testSpace) == 1);

	assert(a == 2);
	assert(b == 3);
}