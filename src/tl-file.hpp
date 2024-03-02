#ifndef TOMS_LANE_FILE
#define TOMS_LANE_FILE

#include "./tl-memory.hpp"

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

int file_interface_write(
	char* fileName,
	const MemorySpace& writeBuffer
);

int file_interface_read(
	char* fileName,
	const MemorySpace& readBuffer
);

int file_interface_size_get(
	char* fileName,
	uint64_t& size
);

int file_interface_location_get(
	MemorySpace& space
);

}

#endif
