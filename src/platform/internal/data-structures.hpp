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
		private:
			int _length = 0;
		public :
			const int& length = _length;
			int capacity; // TODO: make this a private prop like length. Define a constructor that takes in a MemorySpace type to set the capacity.
			T* content;
			int append(const T& item)
			{
				if (_length < capacity)
				{
					content[_length] = item;
					_length += 1;
					return 1;
				}
				return 0;
			}
	};
}

#endif