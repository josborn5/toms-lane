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
			return CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		}
		return CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	}

	int WriteFile(char* fileName, const MemorySpace& writeBuffer)
	{
		HANDLE fileHandle = Win32GetFileHandle(fileName, Write);

		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			return InvalidFilePath;
		}

		if(!writeBuffer.content)
		{
			CloseHandle(fileHandle);
			return InvalidBuffer;
		}

		LPDWORD bytesWritten = 0;
		if(!::WriteFile(fileHandle, writeBuffer.content, writeBuffer.sizeInBytes, bytesWritten, NULL))
		{
			CloseHandle(fileHandle);
			return FileWriteError;
		}

		CloseHandle(fileHandle);
		return Success;
	}
	
	int ReadFile(char* fileName, const MemorySpace& readBuffer)
	{
		HANDLE fileHandle = Win32GetFileHandle(fileName, Read);

		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			return FileDoesNotExist;
		}

		LARGE_INTEGER fileSize;
		if(!GetFileSizeEx(fileHandle, &fileSize))
		{
			CloseHandle(fileHandle);
			return FileSizeError;
		}

		if (fileSize.QuadPart >= 0xFFFFFFFF)
		{
			return FileTooLargeToRead;
		}

		uint32_t fileSize32 = (uint32_t)(fileSize.QuadPart); // assert we're not trying to open a file larger than 4GB, since ReadFile will break for 4GB+ file
		unsigned long fileSize64 = (unsigned long)fileSize.QuadPart;

		if (fileSize64 > readBuffer.sizeInBytes)
		{
			CloseHandle(fileHandle);
			return BufferTooSmallForFile;
		}

		if(!readBuffer.content)
		{
			CloseHandle(fileHandle);
			return InvalidBuffer;
		}
		DWORD bytesToRead;
		if(!::ReadFile(fileHandle, readBuffer.content, fileSize32, &bytesToRead, 0))
		{
			CloseHandle(fileHandle);
			return FileReadError;
		}

		CloseHandle(fileHandle);
		return Success;
	}
}
