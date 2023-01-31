#include "./data-structures.hpp"
#include <assert.h>

using namespace tl;

MemorySpace AllocateMemorySpace(int capacity)
{
	int sizeInBytes = capacity * sizeof(int);
	void* memory = malloc(sizeInBytes);
	MemorySpace space;
	space.content = memory;
	space.sizeInBytes = sizeInBytes;
	return space;
}

void ReturnMemorySpace(MemorySpace space)
{
	free(space.content);
}

void RunHeapArrayTests()
{
	MemorySpace fourInts = AllocateMemorySpace(4);

	HeapArray<int> array = HeapArray<int>(fourInts);
	assert(array.length == 0);
	assert(array.capacity == 4);

	array.append(5);
	assert(array.content[0] == 5);
	assert(array.length == 1);
	assert(array.capacity == 4);

	array.append(4);
	assert(array.content[0] == 5);
	assert(array.content[1] == 4);
	assert(array.length == 2);
	assert(array.capacity == 4);

	MemorySpace remaining = array.sizeToCurrentLength();
	assert(array.content[0] == 5);
	assert(array.content[1] == 4);
	assert(array.length == 2);
	assert(array.capacity == 2);

	size_t sizeOfTwoInts = sizeof(int) * 2;
	assert(remaining.content == ((int *)fourInts.content + sizeOfTwoInts));
	assert(remaining.sizeInBytes == sizeOfTwoInts);

	// Write to first item of remaining and assert array content remains unchanged
	int* firstItemInRemainingSpace = (int *)remaining.content;
	*firstItemInRemainingSpace = 3;
	assert(array.content[0] == 5);
	assert(array.content[1] == 4);

	ReturnMemorySpace(fourInts);
}

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
	RunHeapArrayTests();

	RunHeapQueueTests();
}
