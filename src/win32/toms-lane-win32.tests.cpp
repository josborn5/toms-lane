#include <assert.h>
#include <stdio.h>
#include "./toms-lane-win32.hpp"
#include "toms-lane-win32-file.cpp"

using namespace tl;

int main()
{
	printf("\n===== File Read & Write =====\n");

	int testBuffer[5] = { 0, 0, 0, 0, 0 };
	MemorySpace testSpace;
	testSpace.sizeInBytes = 5 * sizeof(int);
	testSpace.content = &testBuffer[0];

	// Read file that does not exist returns non-zero result
	assert(tl::ReadFile("test-file.dat", testSpace) != 0);

	// Create new file
	int writeFileResult = tl::WriteFile("test-file.dat", testSpace);
	printf("%d", writeFileResult);

	return 0;
}