#include <windows.h>

namespace tl
{
	int Win32ReadFile(FileReadRequest requestInfo)
	{
		HANDLE fileHandle = CreateFileA(requestInfo.fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			return 456; // TODO: define enum for error codes
		}

		LARGE_INTEGER fileSize;
		if(!GetFileSizeEx(fileHandle, &fileSize))
		{
			CloseHandle(fileHandle);
			return 457; // TODO: define enum for error codes
		}

		if (fileSize.QuadPart >= 0xFFFFFFFF)
		{
			return 461; // TODO: define enum for error codes
		}

		uint32_t fileSize32 = (uint32_t)(fileSize.QuadPart); // assert we're not trying to open a file larger than 4GB, since ReadFile will break for 4GB+ file
		unsigned long fileSize64 = (unsigned long)fileSize.QuadPart;

		if (fileSize64 > requestInfo.readBuffer.sizeInBytes)
		{
			CloseHandle(fileHandle);
			return 458; // TODO: define enum for error codes
		}
		// ReadFileResult.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		if(!requestInfo.readBuffer.content)
		{
			CloseHandle(fileHandle);
			return 459; // TODO: define enum for error codes
		}
		DWORD bytesToRead;
		if(!ReadFile(fileHandle, requestInfo.readBuffer.content, fileSize32, &bytesToRead, 0) && fileSize32 == bytesToRead)
		{
			CloseHandle(fileHandle);
			return 460; // TODO: define enum for error codes
		}

		CloseHandle(fileHandle);
		return 0;
	}
}
