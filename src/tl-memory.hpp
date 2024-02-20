#ifndef TOMS_LANE_MEMORY_H
#define TOMS_LANE_MEMORY_H

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

	int InitializeMemory(
		unsigned long permanentSpaceInMegabytes,
		unsigned long transientSpaceInMegabytes,
		GameMemory& memory
	);
}

#endif
