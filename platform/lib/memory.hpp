#ifndef TOMS_LANE_PLATFORM_MEMORY_H
#define TOMS_LANE_PLATFORM_MEMORY_H

#include <stdint.h>

namespace tl
{
	struct MemorySpace
	{
		void* content = nullptr;
		uint64_t sizeInBytes = 0;
	};
}

#endif
