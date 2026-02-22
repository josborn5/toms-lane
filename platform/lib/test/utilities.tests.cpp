#include "../utilities.hpp"
#include <stdio.h>
#include <assert.h>

void RunUtilitiesTests()
{
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
}
