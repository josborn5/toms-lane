#include "./stack.hpp"

template<typename T, unsigned int CAPACITY>
struct stack_ring_buffer
{
	public:
		stack_ring_buffer()
		{
		}

		void push(T item)
		{
			_content[_next_tail_index] = item;
			if ((_next_tail_index + 1) > max_index())
			{
				_next_tail_index = 0;
			}
			else
			{
				_next_tail_index += 1;
			}

			if (_length < CAPACITY)
			{
				_length += 1;
			}
		}

		operation<T> pop()
		{
			operation<T> result;
			if (_length == 0)
			{
				result.result = operation_fail;
				return result;
			}

			result.result = operation_success;
			result.value = _content[tail_index()];

			_length -= 1;
			_next_tail_index = tail_index();

			return result;
		}

	private:
		unsigned int _length = 0;
		unsigned int _next_tail_index = 0;
		T _content[CAPACITY];

		int tail_index()
		{
			return (_next_tail_index == 0) ? max_index() : _next_tail_index - 1;
		}

		unsigned int max_index()
		{
			return CAPACITY - 1;
		}
};
