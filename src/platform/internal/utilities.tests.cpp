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
	assert(tl::CharStringToInt("01") == 1);

	assert(tl::CharStringToInt("-1") == -1);
	assert(tl::CharStringToInt("-0") == 0);
	assert(tl::CharStringToInt("-23") == -23);
	assert(tl::CharStringToInt("-654") == -654);
	assert(tl::CharStringToInt("-6543210") == -6543210);

	printf("\n===== CopyToChar =====\n");
	char* from = "aaa-aaa";
	const int length = 8;
	char* to = (char *)malloc(length);
	for (int i = 0; i < length; i += 1)
	{
		to[i] = 'b';
	}

	char* pointer = tl::CopyToChar(from, to, '-');

	assert(*pointer == '-');
	assert(pointer == &from[3]);

	assert(to[0] == 'a');
	assert(to[1] == 'a');
	assert(to[2] == 'a');
	assert(to[3] == '\0');
	assert(to[4] == 'b');
	assert(to[5] == 'b');
	assert(to[6] == 'b');

	assert(from[0] == 'a');
	assert(from[1] == 'a');
	assert(from[2] == 'a');
	assert(from[3] == '-');
	assert(from[4] == 'a');
	assert(from[5] == 'a');
	assert(from[6] == 'a');
	assert(from[7] == '\0');

	from = "aaaaa";
	for (int i = 0; i < length; i += 1)
	{
		to[i] = 'b';
	}

	pointer = tl::CopyToChar(from, to, '-');

	assert(*pointer == '\0');
	assert(pointer == &from[5]);

	assert(to[0] == 'a');
	assert(to[1] == 'a');
	assert(to[2] == 'a');
	assert(to[3] == 'a');
	assert(to[4] == 'a');
	assert(to[5] == '\0');
	assert(to[6] == 'b');

	assert(from[0] == 'a');
	assert(from[1] == 'a');
	assert(from[2] == 'a');
	assert(from[3] == 'a');
	assert(from[4] == 'a');
	assert(from[5] == '\0');

	free(to);

	printf("\n===== GetNextNumberChar =====\n");
	char* numberString = "ab 123";
	char* result = tl::GetNextNumberChar(numberString);
	assert(*result == '1');
	assert(result == &numberString[3]);

	printf("\nDONE!\n");
}
