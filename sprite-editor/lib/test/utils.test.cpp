#include "../utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void RunUtilitiesTests()
{
	printf("\n===== CharStringToInt =====\n");
	assert(CharStringToInt("1") == 1);
	assert(CharStringToInt("0") == 0);
	assert(CharStringToInt("23") == 23);
	assert(CharStringToInt("654") == 654);
	assert(CharStringToInt("6543210") == 6543210);
	assert(CharStringToInt("01") == 1);

	assert(CharStringToInt("-1") == -1);
	assert(CharStringToInt("-0") == 0);
	assert(CharStringToInt("-23") == -23);
	assert(CharStringToInt("-654") == -654);
	assert(CharStringToInt("-6543210") == -6543210);

	printf("\n===== CopyToEndOfNumberChar =====\n");
	char* copyNumberString = "123 ";
	const int to_length = 8;
	char* to = (char*)malloc(to_length);

	CopyToEndOfNumberChar(copyNumberString, to);
	assert(to[0] == '1');

	free(to);

	printf("\n===== GetNextNumberChar =====\n");
	char* numberString = "ab 123";
	char* result = GetNextNumberChar(numberString);
	assert(*result == '1');
	assert(result == &numberString[3]);
}
