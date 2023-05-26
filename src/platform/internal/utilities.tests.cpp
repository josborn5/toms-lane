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

	printf("\n===== CharStringToInt =====\n");
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

	printf("\n===== IntToCharString =====\n");
	char testBuffer[10];
	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a';

	tl::MemorySpace charStringBuffer;
	charStringBuffer.content = testBuffer;
	charStringBuffer.sizeInBytes = 10 * sizeof(char);
	tl::IntToCharString(0, charStringBuffer);
	printf(testBuffer);
	printf("\n");
	assert(testBuffer[0] == '0');
	assert(testBuffer[1] == '\0');

	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a';
	tl::IntToCharString(9, charStringBuffer);

	printf(testBuffer);
	printf("\n");
	assert(testBuffer[0] == '9');
	assert(testBuffer[1] == '\0');


	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a';
	tl::IntToCharString(9876, charStringBuffer);

	printf(testBuffer);
	printf("\n");
	assert(testBuffer[0] == '9');
	assert(testBuffer[1] == '8');
	assert(testBuffer[2] == '7');
	assert(testBuffer[3] == '6');
	assert(testBuffer[4] == '\0');

	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a';
	tl::IntToCharString(-9876, charStringBuffer);

	printf(testBuffer);
	printf("\n");
	assert(testBuffer[0] == '-');
	assert(testBuffer[1] == '9');
	assert(testBuffer[2] == '8');
	assert(testBuffer[3] == '7');
	assert(testBuffer[4] == '6');
	assert(testBuffer[5] == '\0');

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

	printf("\n===== CopyToEndOfNumberChar =====\n");
	char* copyNumberString = "123 ";
	tl::CopyToEndOfNumberChar(copyNumberString, to);
	assert(to[0] == '1');

	free(to);

	printf("\n===== GetNextNumberChar =====\n");
	char* numberString = "ab 123";
	char* result = tl::GetNextNumberChar(numberString);
	assert(*result == '1');
	assert(result == &numberString[3]);

	printf("\n===== CarveMemorySpace =====\n");
	char* spaceTestContent = "0123456789";

	tl::MemorySpace toCarve;
	toCarve.content = spaceTestContent;
	toCarve.sizeInBytes = 10 * sizeof(char);

	uint64_t sliceSize = 2 * (uint64_t)sizeof(char); // Carve of the first 2 chars
	tl::MemorySpace sliced = CarveMemorySpace(sliceSize, toCarve);

	char* remainingString = (char*)toCarve.content;
	char* slicedChars = (char*)sliced.content;
	printf(slicedChars);
	printf("\n");
	printf(remainingString);
	printf("\n");
	assert(((char*)toCarve.content)[0] == '2');
	assert(((char*)sliced.content)[0] == '0');

	printf("\nDONE!\n");

	printf("\n===== DeleteFromArray =====\n");
	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a' + (char)i;
	assert(testBuffer[0] == 'a');
	assert(testBuffer[1] == 'b');
	assert(testBuffer[2] == 'c');
	assert(testBuffer[3] == 'd');
	assert(testBuffer[4] == 'e');

	DeleteFromArray(testBuffer, 2, 3, 5);

	assert(testBuffer[0] == 'a');
	assert(testBuffer[1] == 'b');
	assert(testBuffer[2] == 'e');
	assert(testBuffer[3] == 'd');
	assert(testBuffer[4] == 'e');

	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a' + (char)i;
	DeleteFromArray(testBuffer, 0, 3, 10);

	assert(testBuffer[0] == 'e');
	assert(testBuffer[1] == 'f');
	assert(testBuffer[2] == 'g');
	assert(testBuffer[3] == 'h');
	assert(testBuffer[4] == 'i');

	for (int i = 0; i < 10; i += 1) testBuffer[i] = 'a' + (char)i;
	DeleteFromArray(testBuffer, 2, 2, 10);

	assert(testBuffer[0] == 'a');
	assert(testBuffer[1] == 'b');
	assert(testBuffer[2] == 'd');
	assert(testBuffer[3] == 'e');
	assert(testBuffer[4] == 'f');
}
