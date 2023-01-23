#ifndef TOMS_LANE_DATA_STRUCTURES_H
#define TOMS_LANE_DATA_STRUCTURES_H

namespace tl
{
	template<typename T>
	struct HeapArray
	{
		int length = 0;
		int capacity;
		T* content;
	};
}

#endif