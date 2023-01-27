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
			int _capacity = 0;

		public :
			const int& length = _length;
			const int& capacity = _capacity;

			T* content;

			HeapArray() {}
			HeapArray(const MemorySpace& memory)
			{
				initialize(memory);
			}

			void initialize(const MemorySpace& memory)
			{
				content = (T *)memory.content;
				size_t typeSizeInBytes = sizeof(T);
				_capacity = (int)(memory.sizeInBytes / typeSizeInBytes);
			}

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