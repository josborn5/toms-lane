#ifndef TOMS_LANE_DATA_STRUCTURES_H
#define TOMS_LANE_DATA_STRUCTURES_H

namespace tl
{
	struct MemorySpace
	{
		void* content;
		unsigned long sizeInBytes;
	};
	
	template<typename T>
	struct HeapArray
	{
		int length = 0;
		int capacity;
		T* content;
	};
}

#endif