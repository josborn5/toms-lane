#include "./ring-buffer.hpp"
#include <stdio.h>
#include <assert.h>

void run_ring_buffer_tests()
{
	printf("\nRunning ring buffer tests\n");

	stack_ring_buffer<int, 2> ring_buffer;

	ring_buffer.push(1);
	ring_buffer.push(2);

	operation<int> pop = ring_buffer.pop();
	assert(pop.result == operation_success);
	assert(pop.value == 2);

	pop = ring_buffer.pop();
	assert(pop.result == operation_success);
	assert(pop.value == 1);

	pop = ring_buffer.pop();
	assert(pop.result == operation_fail);

	printf("\nRing buffer tests complete.\n");
}
