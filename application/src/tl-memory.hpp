#ifndef TOMS_LANE_MEMORY_H
#define TOMS_LANE_MEMORY_H

#include "../../src/platform/memory.hpp"

namespace tl
{
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
