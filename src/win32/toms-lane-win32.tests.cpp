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

	int testBuffer2[5] = { 1, 2, 3, 4, 5 };
	MemorySpace testSpace2;
	testSpace2.sizeInBytes = 5 * sizeof(int);
	testSpace2.content = &testBuffer2[0];

	// Read file that does not exist returns non-zero result
	int readFileResult = tl::ReadFile("test-file.dat", testSpace);
	assert(readFileResult == tl::FileDoesNotExist);

	// Create new file
	int writeFileResult = tl::WriteFile("test-file.dat", testSpace2);
	assert(writeFileResult == 0);

	// Read the written file
	readFileResult = tl::ReadFile("test-file.dat", testSpace);
	assert(readFileResult == 0);
	assert(testBuffer[0] == 1);
	assert(testBuffer[1] == 2);
	assert(testBuffer[2] == 3);
	assert(testBuffer[3] == 4);
	assert(testBuffer[4] == 5);

	return 0;
}