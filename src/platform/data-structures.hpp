#ifndef TOMS_LANE_DATA_STRUCTURES_H
#define TOMS_LANE_DATA_STRUCTURES_H

#include "../tl-application.hpp"

namespace tl
{
	template<typename T>
	struct operation
	{
		T value;
		int result;
	};

	template<typename T>
	struct array
	{
		private:
			unsigned int _length = 0;
			unsigned int _capacity = 0;
			size_t _itemSizeInBytes = sizeof(T);
			T* _content;

		public :
			array(){}
			array(const MemorySpace& memory)
			{
				initialize(memory);
			}
			array(T* pointer, int capacity)
			{
				initialize(pointer, capacity);
			}

			void initialize(const MemorySpace& memory)
			{
				_content = (T*)memory.content;
				_capacity = (int)(memory.sizeInBytes / _itemSizeInBytes);
			}

			void initialize(const MemorySpace& memory, unsigned int length)
			{
				initialize(memory);
				_length = length;
			}

			void initialize(T* pointer, int capacity)
			{
				_content = pointer;
				_capacity = capacity;
			}

			T& access(int index)
			{
				return _content[index];
			}

			T& get(int index) const
			{
				return _content[index]; // TODO: bounds check and return monad value
			}

			int capacity() const
			{
				return _capacity;
			}

			int length() const
			{
				return _length;
			}

			int append(const T item)
			{
				if (_length < _capacity)
				{
					_content[_length] = item;
					_length += 1;
					return 1;
				}
				return 0;
			}

			operation<T> pop()
			{
				operation<T> operationValue;
				if (_length == 0)
				{
					operationValue.result = 1;
					return operationValue;
				}
				operationValue.value = _content[_length - 1];
				operationValue.result = 0;
				_length -= 1;
				return operationValue;
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

			T* head_pointer() {
				return _content;
			}

			int delete_from(unsigned int inclusive_start_index, unsigned int inclusive_end_index)
			{
				if (inclusive_start_index >= _length ||
					inclusive_end_index >= _length ||
					inclusive_start_index > inclusive_end_index)
				{
					return -1;
				}

				unsigned int start_copy_index = inclusive_end_index + 1;
				unsigned int offset = start_copy_index - inclusive_start_index;
				for (unsigned int source_index = start_copy_index; source_index < _length; source_index += 1)
				{
					unsigned int target_index = source_index - offset;
					T copy_value = _content[source_index];
					_content[target_index] = copy_value;
				}

				unsigned int subtraction_count = inclusive_end_index - inclusive_start_index + 1;

				_length -= subtraction_count;

				return 0;
			}
	};

	template<typename T, unsigned int N>
	struct stack_array
	{
		private:
			unsigned int _length = 0;
			unsigned int _capacity = N;
			size_t _itemSizeInBytes = sizeof(T);
			T _content[N];

		public :
			stack_array()
			{
			}

			operation<T*> get_pointer(unsigned int index)
			{
				operation<T*> return_value = {0};
				if (index >= _length)
				{
					return_value.result = 1;
				}
				else
				{
					return_value.result = 0;
					return_value.value = &_content[index];
				}
				return return_value;
			}

			operation<T> get_copy(unsigned int index) const
			{
				operation<T> return_value = {0};
				if (index >= _length)
				{
					return_value.result = 1;
				}
				else
				{
					return_value.result = 0;
					return_value.value = _content[index];
				}
				return return_value;
			}

			unsigned int length() const
			{
				return _length;
			}

			int append(const T item)
			{
				if (_length < _capacity)
				{
					_content[_length] = item;
					_length += 1;
					return 1;
				}
				return 0;
			}

			void clear()
			{
				_length = 0;
			}

			unsigned int capacity()
			{
				return _capacity;
			}
	};

	template<typename T>
	struct queue
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

			queue(const MemorySpace& memory)
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

			operation<T> dequeue()
			{
				operation<T> dequeue_op;
				dequeue_op.result = -1;
				if (_length > 0)
				{
					T head_item = *content;
					content++;
					_length -= 1;
					_capacity -= 1; // TODO: Convert this to a tail reference and connect to the enqueue check

					dequeue_op.value = head_item;
					dequeue_op.result = 0;
				}
				return dequeue_op;
			}
	};
}

#endif
