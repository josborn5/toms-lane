#ifndef TOMS_LANE_UTILITIES_H
#define TOMS_LANE_UTILITIES_H

#include "./memory.hpp"

namespace tl
{
	MemorySpace CarveMemorySpace(uint64_t carveSizeInBytes, MemorySpace& toCarve);
}

#endif
