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

	printf("\n===== CharToInt =====\n");
	assert(tl::CharStringToInt("1") == 1);
	assert(tl::CharStringToInt("0") == 0);
	assert(tl::CharStringToInt("23") == 23);
	assert(tl::CharStringToInt("654") == 654);
	assert(tl::CharStringToInt("6543210") == 6543210);

	assert(tl::CharStringToInt("-1") == -1);
	assert(tl::CharStringToInt("-0") == 0);
	assert(tl::CharStringToInt("-23") == -23);
	assert(tl::CharStringToInt("-654") == -654);
	assert(tl::CharStringToInt("-6543210") == -6543210);
}