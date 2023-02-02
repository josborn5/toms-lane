#ifndef TOMS_LANE_UTILITIES_H
#define TOMS_LANE_UTILITIES_H

#include "./data-structures.hpp"

namespace tl
{
	template<typename T>
	int swap(T& a, T& b, MemorySpace& transient)
	{
		if (transient.sizeInBytes < sizeof(T)) return 1;

		*(T *)transient.content = a;
		a = b;
		b = *(T *)transient.content;

		return 0;
	}
}

#endif