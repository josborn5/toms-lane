#ifndef TL_MEMORY_H
#define TL_MEMORY_H

#include "../platform/toms-lane-application.hpp"

namespace tl
{
	int InitializeMemory(
		unsigned long permanentSpaceInMegabytes,
		unsigned long transientSpaceInMegabytes,
		GameMemory& memory
	);
}

#endif
