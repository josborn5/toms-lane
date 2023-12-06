#ifndef TOMS_LANE_WIN32_FILE
#define TOMS_LANE_WIN32_FILE

#include "../platform/toms-lane-application.hpp"

namespace tl
{

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

int win32_file_interface_write(
	char* fileName,
	const MemorySpace& writeBuffer
);

int win32_file_interface_read(
	char* fileName,
	const MemorySpace& readBuffer
);

int win32_file_interface_size_get(
	char* fileName,
	uint64_t& size
);

}

#endif