#ifndef TOMS_LANE_APPLICATION_H
#define TOMS_LANE_APPLICATION_H

#include <stdint.h>

namespace tl
{
	struct MemorySpace
	{
		void* content;
		uint64_t sizeInBytes;
	};

	struct GameMemory
	{
		MemorySpace permanent;
		MemorySpace transient;
	};
}

#endif
