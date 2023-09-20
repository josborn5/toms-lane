#ifndef TOMS_LANE_WIN32_H
#define TOMS_LANE_WIN32_H

#include <windows.h>
#include "../platform/toms-lane-platform.hpp"

namespace tl
{
	int Win32Main(HINSTANCE instance);

	int Win32Main(HINSTANCE instance, const WindowSettings &settings);

	int GetFileSize(char* fileName, uint64_t& size);
	int ReadFile(char* fileName, const MemorySpace& readBuffer);
	int WriteFile(char* fileName, const MemorySpace& writeBuffer);

	enum FileStatusCode
	{
		Success = 0,
		InvalidFilePath = 456,
		FileDoesNotExist = 457,
		FileSizeError = 458,
		InvalidBuffer = 459,
		BufferTooSmallForFile = 460,
		FileReadError = 461,
		FileTooLargeToRead = 462,
		FileWriteError = 463
	};
}

#endif
