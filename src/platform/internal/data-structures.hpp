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
				if (_length < _capacity)
				{
					content[_length] = item;
					_length += 1;
					return 1;
				}
				return 0;
			}

			/*
			 * Sets capacity to current length
			 * and returns details of any
			  * remaining MemorySpace
			 */
			MemorySpace sizeToCurrentLength()
			{
				int remaining = _capacity - _length;
				_capacity = _length;
				size_t typeSizeInBytes = sizeof(T);
				size_t currentSizeInBytes = typeSizeInBytes * _length;
				MemorySpace remainingSpaceDetails;
				remainingSpaceDetails.content = (T *)(content + currentSizeInBytes);
				remainingSpaceDetails.sizeInBytes = (unsigned long)remaining * (unsigned long)typeSizeInBytes;
				return remainingSpaceDetails;
			}
	};

	template<typename T>
	struct HeapQueue
	{
		private:
			int _capacity = 0;
			int _length = 0;
			T* _head;
			T* _tail;
			T* _ogHead;

		public:
			const int& length = _length;
			const int& capacity = _capacity;

			HeapQueue(const MemorySpace& memory)
			{
				_head = (T *)memory.content;
				_ogHead = _head;
				size_t typeSizeInBytes = sizeof(T);
				_capacity = (int)(memory.sizeInBytes / typeSizeInBytes);
			}

			int enqueue(const T& item)
			{
				if (_length < _capacity) // TODO: Copy down if _ogHead > _head
				{
					_head[_length] = item;
					_length += 1;
					return 0;
				}
				return 1;
			}

			T& dequeue()
			{
				if (_length > 0)
				{
					T* headItem = _head;
					_head++;
					_length -= 1;

					return *headItem;
				}
				
				throw;
			}
	};
}

#endif