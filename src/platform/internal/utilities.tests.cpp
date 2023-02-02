#include "./utilities.hpp"
#include <stdio.h>
#include <assert.h>

void RunUtilitiesTests()
{
	printf("\n===== Swap =====\n");
	int a = 3;
	int b = 2;
	
	tl::swap(a, b);

	assert(a == 2);
	assert(b == 3);
}