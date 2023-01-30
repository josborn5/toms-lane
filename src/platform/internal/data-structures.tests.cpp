#include "./data-structures.hpp"
#include <assert.h>

using namespace tl;

void RunHeapQueueTests()
{
	int sizeInBytes = 4 * sizeof(int);
	void* memory = malloc(sizeInBytes);
	MemorySpace fourInts;
	fourInts.content = memory;
	fourInts.sizeInBytes = sizeInBytes;

	HeapQueue<int> queue = HeapQueue<int>(fourInts);

	assert(queue.length == 0);
	assert(queue.capacity == 4);

	assert(queue.enqueue(1) == 0);
	assert(queue.length == 1);
	assert(queue.capacity == 4);

	assert(queue.enqueue(2) == 0);
	assert(queue.length == 2);
	assert(queue.capacity == 4);

	assert(queue.enqueue(3) == 0);
	assert(queue.length == 3);
	assert(queue.capacity == 4);

	assert(queue.enqueue(4) == 0);
	assert(queue.length == 4);
	assert(queue.capacity == 4);

	assert(queue.enqueue(5) == 1);
	assert(queue.length == 4);
	assert(queue.capacity == 4);

	int first = queue.dequeue();
	assert(first == 1);
	assert(queue.length == 3);
	assert(queue.capacity == 4);

	int second = queue.dequeue();
	assert(second == 2);
	assert(queue.length == 2);
	assert(queue.capacity == 4);

	assert(queue.length == 2);
	assert(queue.capacity == 4);

	int third = queue.dequeue();
	assert(third == 3);
	assert(queue.length == 1);
	assert(queue.capacity == 4);

	int fourth = queue.dequeue();
	assert(fourth == 4);
	assert(queue.length == 0);
	assert(queue.capacity == 4);
}

void RunDataStructureTests()
{
	RunHeapQueueTests();
}
