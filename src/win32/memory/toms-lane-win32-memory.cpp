#include <windows.h>
#include "../../tl-memory.hpp"

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)

namespace tl
{

static void DisplayLastWin32Error()
{
	DWORD ErrorCode = GetLastError();
	char ErrorCodeBuffer[256];
	wsprintf(ErrorCodeBuffer, "VirtualAlloc error code: %d\n", ErrorCode);
}

int InitializeMemory(
	unsigned long permanentSpaceInMegabytes,
	unsigned long transientSpaceInMegabytes,
	GameMemory& memory)
{
	// Initialize general use memory
	memory.permanent.sizeInBytes = Megabytes(permanentSpaceInMegabytes);
	memory.transient.sizeInBytes = Megabytes(transientSpaceInMegabytes);

	uint64_t totalStorageSpace = memory.permanent.sizeInBytes + memory.transient.sizeInBytes;
	memory.permanent.content = VirtualAlloc(0, (size_t)totalStorageSpace, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if(memory.permanent.content == NULL)
	{
		DisplayLastWin32Error();
		return -1;
	}

	memory.transient.content = (uint8_t*)memory.permanent.content + memory.permanent.sizeInBytes;

	return 0;
}

}

