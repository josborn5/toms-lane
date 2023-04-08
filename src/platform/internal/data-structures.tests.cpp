#include "./data-structures.hpp"
#include <stdio.h>
#include <assert.h>

using namespace tl;

void RunHeapArrayTests()
{
	const int originalMemValue = 8;
	int sixInts[6] = {
		originalMemValue,
		originalMemValue,
		originalMemValue,
		originalMemValue,
		originalMemValue,
		originalMemValue
	};
	// MemorySpace fourInts = AllocateMemorySpace(4);
	MemorySpace fourInts;
	fourInts.content = &sixInts[1];
	fourInts.sizeInBytes = 4 * sizeof(int);

	HeapArray<int> array = HeapArray<int>(fourInts);
	assert(array.length == 0);
	assert(array.capacity == 4);

	array.append(5);
	assert(array.content[0] == 5);
	assert(array.length == 1);
	assert(array.capacity == 4);
	assert(sixInts[0] == originalMemValue);
	assert(sixInts[5] == originalMemValue);

	array.append(4);
	assert(array.content[0] == 5);
	assert(array.content[1] == 4);
	assert(array.length == 2);
	assert(array.capacity == 4);
	assert(sixInts[0] == originalMemValue);
	assert(sixInts[5] == originalMemValue);

	MemorySpace remaining = array.sizeToCurrentLength();
	assert(array.content[0] == 5);
	assert(array.content[1] == 4);
	assert(array.length == 2);
	assert(array.capacity == 2);
	assert(sixInts[0] == originalMemValue);
	assert(sixInts[5] == originalMemValue);

	assert(remaining.content == &sixInts[3]);
	assert(remaining.sizeInBytes == 2 * sizeof(int));

	// Write to first item of remaining and assert array content remains unchanged
	int* firstItemInRemainingSpace = (int *)remaining.content;
	*firstItemInRemainingSpace = 3;
	assert(array.content[0] == 5);
	assert(array.content[1] == 4);

	assert(array.length == 2);
	array.clear();
	assert(array.length == 0);
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
	assert(queue.capacity == 3);

	int second = queue.dequeue();
	assert(second == 2);
	assert(queue.length == 2);
	assert(queue.capacity == 2);

	int third = queue.dequeue();
	assert(third == 3);
	assert(queue.length == 1);
	assert(queue.capacity == 1);

	int fourth = queue.dequeue();
	assert(fourth == 4);
	assert(queue.length == 0);
	assert(queue.capacity == 0);

	assert(queue.enqueue(1) == 1);
	assert(queue.length == 0);
	assert(queue.capacity == 0);

	assert(queue.enqueue(2) == 1);
	assert(queue.length == 0);
	assert(queue.capacity == 0);
}

void RunDataStructureTests()
{
	printf("\n===== Heap Array =====\n");
	RunHeapArrayTests();

	printf("\n===== Heap Queue =====\n");
	RunHeapQueueTests();
}
