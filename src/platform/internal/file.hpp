#ifndef TOMS_LANE_FILE_HPP
#define TOMS_LANE_FILE_HPP

#include "./data-structures.hpp"

namespace tl
{ 
	struct FileReadRequest
	{
		int start = 0;
		char* fileName;
		MemorySpace readBuffer;
	};
}

#endif