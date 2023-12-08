#include <windows.h>
#include "../win32-file.hpp"

namespace tl
{
	enum FileAccess
	{
		Read,
		Write
	};

	static HANDLE Win32GetFileHandle(char* fileName, FileAccess access)
	{
		if (access == Write)
		{
			return CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		}
		return CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	}

	int win32_file_interface_write(char* fileName, const MemorySpace& writeBuffer)
	{
		if(!writeBuffer.content)
		{
			return InvalidBuffer;
		}

		HANDLE fileHandle = Win32GetFileHandle(fileName, Write);

		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			return InvalidFilePath;
		}

		LPDWORD bytesWritten = 0;
		bool writeSuccess = WriteFile(fileHandle, writeBuffer.content, (DWORD)writeBuffer.sizeInBytes, bytesWritten, NULL);
		CloseHandle(fileHandle);

		return writeSuccess ? Success : FileWriteError;
	}

	int win32_file_interface_size_get(char* fileName, uint64_t& size)
	{
		HANDLE fileHandle = Win32GetFileHandle(fileName, Read);

		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			return FileDoesNotExist;
		}

		LARGE_INTEGER fileSize;
		bool readSuccess = GetFileSizeEx(fileHandle, &fileSize);
		CloseHandle(fileHandle);
		if(!readSuccess)
		{
			return FileSizeError;
		}

		if (fileSize.QuadPart >= 0xFFFFFFFF)
		{
			return FileTooLargeToRead;
		}

		size = (uint64_t)fileSize.QuadPart;

		return Success;
	}
	
	int win32_file_interface_read(char* fileName, const MemorySpace& readBuffer)
	{
		if(!readBuffer.content)
		{
			return InvalidBuffer;
		}

		uint64_t fileSize;
		int fileReadResult = win32_file_interface_size_get(fileName, fileSize);
		if (fileReadResult != Success)
		{
			return fileReadResult;
		}

		uint32_t fileSize32 = (uint32_t)fileSize; // assert we're not trying to open a file larger than 4GB, since ReadFile will break for 4GB+ file
		unsigned long fileSize64 = (unsigned long)fileSize;

		if (fileSize64 > readBuffer.sizeInBytes)
		{
			return BufferTooSmallForFile;
		}

		HANDLE fileHandle = Win32GetFileHandle(fileName, Read);
		DWORD bytesToRead;
		bool readSuccess = ReadFile(fileHandle, readBuffer.content, fileSize32, &bytesToRead, 0);
		CloseHandle(fileHandle);

		return readSuccess ? Success : FileReadError;
	}

	int win32_file_interface_location_get(MemorySpace& space)
	{
		DWORD bufferSize = (DWORD)space.sizeInBytes;
		LPSTR fileName = (LPSTR)space.content;
		DWORD outputPathCount = GetModuleFileNameA(
			NULL,
			fileName,
			bufferSize
		);
		// output includes the name of the executable. strip that from the result
		char* endStringPointer = (char*)space.content;

		// locate the end of the string
		endStringPointer += outputPathCount;

		// wind back to the last '\' char in the string
		while (endStringPointer && *endStringPointer != '\\')
		{
			endStringPointer--;
		}

		// trim anything past the last '\' char
		if (*endStringPointer == '\\')
		{
			endStringPointer++;
			*endStringPointer = '\0';
		}

		return 0;
	}
}
