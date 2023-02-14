#include <windows.h>

namespace tl
{
	enum FileAccess
	{
		Read,
		Write
	};

	HANDLE Win32GetFileHandle(char* fileName, FileAccess access)
	{
		if (access == Write)
		{
			return CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		}
		return CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	}

	int WriteFile(char* fileName, MemorySpace writeBuffer)
	{
		HANDLE fileHandle = Win32GetFileHandle(fileName, Write);

		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			return 456; // TODO: define enum for error codes
		}

		if(!writeBuffer.content)
		{
			CloseHandle(fileHandle);
			return 459; // TODO: define enum for error codes
		}

		LPDWORD bytesWritten = 0;
		if(!::WriteFile(fileHandle, writeBuffer.content, writeBuffer.sizeInBytes, bytesWritten, NULL))
		{
			CloseHandle(fileHandle);
			return 460; // TODO: define enum for error codes
		}

		CloseHandle(fileHandle);
		return 0;
	}
	
	int ReadFile(char* fileName, MemorySpace readBuffer)
	{
		HANDLE fileHandle = Win32GetFileHandle(fileName, Read);

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

		if (fileSize64 > readBuffer.sizeInBytes)
		{
			CloseHandle(fileHandle);
			return 458; // TODO: define enum for error codes
		}

		if(!readBuffer.content)
		{
			CloseHandle(fileHandle);
			return 459; // TODO: define enum for error codes
		}
		DWORD bytesToRead;
		if(!::ReadFile(fileHandle, readBuffer.content, fileSize32, &bytesToRead, 0))
		{
			CloseHandle(fileHandle);
			return 460; // TODO: define enum for error codes
		}

		CloseHandle(fileHandle);
		return 0;
	}
}
