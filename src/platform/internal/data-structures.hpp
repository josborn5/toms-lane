#ifndef TOMS_LANE_DATA_STRUCTURES_H
#define TOMS_LANE_DATA_STRUCTURES_H

#include "../../tl-application.hpp"

namespace tl
{
	template<typename T>
	struct HeapArray
	{
		private:
			int _length = 0;
			int _capacity = 0;
			size_t _itemSizeInBytes = sizeof(T);
			T* _content;

		public :
			HeapArray(){}
			HeapArray(const MemorySpace& memory)
			{
				initialize(memory);
			}
			HeapArray(T* pointer, int capacity)
			{
				_content = pointer;
				_capacity = capacity;
			}

			void initialize(const MemorySpace& memory)
			{
				_content = (T*)memory.content;
				_capacity = (int)(memory.sizeInBytes / _itemSizeInBytes);
			}

			T& access(int index)
			{
				return _content[index];
			}

			T& get(int index) const
			{
				return _content[index];
			}

			int capacity() const
			{
				return _capacity;
			}

			int length() const
			{
				return _length;
			}

			int append(const T& item)
			{
				if (_length < _capacity)
				{
					_content[_length] = item;
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
				T* addressOfNextItem = &_content[_length];
				int remaining = _capacity - _length;
				_capacity = _length;
				MemorySpace remainingSpaceDetails;
				remainingSpaceDetails.content = addressOfNextItem;
				remainingSpaceDetails.sizeInBytes = (uint64_t)(remaining * _itemSizeInBytes);
				return remainingSpaceDetails;
			}

			void clear()
			{
				_length = 0;
			}

			T* getTailPointer()
			{
				return &_content[_length - 1];
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
			int length() const
			{
				return _length;
			}
			int capacity() const
			{
				return _capacity;
			}
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
