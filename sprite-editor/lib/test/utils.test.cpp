#include "../utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void RunUtilitiesTests()
{
	printf("\n===== CopyToEndOfNumberChar =====\n");
	const char* copyNumberString = "123 ";
	const int to_length = 8;
	char* to = (char*)malloc(to_length);

	CopyToEndOfNumberChar(copyNumberString, to);
	assert(to[0] == '1');

	free(to);

	printf("\n===== GetNextNumberChar =====\n");
	const char* numberString = "ab 123";
	const char* result = GetNextNumberChar(numberString);
	assert(*result == '1');
	assert(result == &numberString[3]);
}
