#ifndef TOMS_LANE_DATA_STRUCTURES_H
#define TOMS_LANE_DATA_STRUCTURES_H

#include "./platform.hpp"

namespace tl
{
	template<typename T>
	struct HeapArray
	{
		private:
			int _length = 0;
			int _capacity = 0;
			size_t _itemSizeInBytes = sizeof(T);

		public :
			const int& length = _length;
			const int& capacity = _capacity;

			T* content;

			HeapArray(const MemorySpace& memory)
			{
				initialize(memory);
			}
			HeapArray(T* pointer, int capacity)
			{
				content = pointer;
				_capacity = capacity;
			}

			void initialize(const MemorySpace& memory)
			{
				content = (T*)memory.content;
				_capacity = (int)(memory.sizeInBytes / _itemSizeInBytes);
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
				T* addressOfNextItem = &content[_length];
				int remaining = _capacity - _length;
				_capacity = _length;
				MemorySpace remainingSpaceDetails;
				remainingSpaceDetails.content = addressOfNextItem;
				remainingSpaceDetails.sizeInBytes = (unsigned long)(remaining * _itemSizeInBytes);
				return remainingSpaceDetails;
			}

			void clear()
			{
				_length = 0;
			}
	};

	template<typename T>
	struct HeapQueue
	{
		private:
			int _capacity = 0;
			int _length = 0;
			T* _ogHead;

		public:
			const int& length = _length;
			const int& capacity = _capacity;
			T* content;

			HeapQueue(const MemorySpace& memory)
			{
				content = (T *)memory.content;
				_ogHead = content;
				size_t typeSizeInBytes = sizeof(T);
				_capacity = (int)(memory.sizeInBytes / typeSizeInBytes);
			}

			int enqueue(const T& item)
			{
				if (_length < _capacity) // TODO: Copy down if _ogHead > _head
				{
					content[_length] = item;
					_length += 1;
					return 0;
				}
				return 1;
			}

			T dequeue()
			{
				if (_length > 0)
				{
					T headItem = *content;
					content++;
					_length -= 1;
					_capacity -= 1; // TODO: Convert this to a tail reference and connect to the enqueue check

					return headItem;
				}
				
				throw;
			}
	};
}

#endif