enum operation_result
{
	operation_success = 0,
	operation_fail = 1
};

template<typename T>
struct operation
{
	T value;
	operation_result result;
};

template<typename T, unsigned int N>
struct stack_stack
{
	private:
		unsigned int _length = 0;
		unsigned int _capacity = N;
		size_t _item_size_in_bytes = sizeof(T);
		T _content[N];

	public:
		stack_stack()
		{
		}

		operation_result push(const T item)
		{
			if (_length >= _capacity)
			{
				return operation_fail;
			}

			_content[_length] = item;
			_length += 1;
			return operation_success;
		}

		void clear()
		{
			_length = 0;
		}

		operation<T> pop()
		{
			operation<T> return_value = {0};

			if (_length < 1)
			{
				return_value.result = operation_fail;
				return return_value;
			}

			return_value.value = _content[_length - 1];
			return_value.result = opeartion_success;
			return return_value;
		}
};
