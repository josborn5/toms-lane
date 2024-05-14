#include "./data-structures.hpp"
#include <stdio.h>
#include <assert.h>

using namespace tl;

void RunArrayTailTest()
{
	int fourIntsStack[4] = { 0 };
	MemorySpace fourInts;
	fourInts.content = &fourIntsStack[0];
	fourInts.sizeInBytes = 4 * sizeof(int);

	array<int> heapArray = array<int>(fourInts);

	heapArray.append(1);

	int* tail = heapArray.getTailPointer();

	assert(tail == &fourIntsStack[0]);

	heapArray.append(2);
	tail = heapArray.getTailPointer();

	assert(tail == &fourIntsStack[1]);
}

void RunArrayTests()
{
	RunArrayTailTest();

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

	array<int> array2 = array<int>(&sixInts[1], 4);
	array<int> array1 = array<int>(fourInts);
	assert(array1.length() == 0);
	assert(array1.capacity() == 4);
	assert(array2.length() == 0);
	assert(array2.capacity() == 4);

	operation<int> failedRemoval = array1.pop();
	assert(failedRemoval.result == 1);

	array1.append(5);
	assert(array1.get(0) == 5);
	assert(array1.length() == 1);
	assert(array1.capacity() == 4);
	assert(array2.get(0) == 5); // screwed up assertion and test, but it found a defect
	assert(sixInts[0] == originalMemValue);
	assert(sixInts[5] == originalMemValue);

	array1.append(4);
	assert(array1.get(0) == 5);
	assert(array1.get(1) == 4);
	assert(array1.length() == 2);
	assert(array1.capacity() == 4);
	assert(sixInts[0] == originalMemValue);
	assert(sixInts[5] == originalMemValue);

	operation<int> removedItem = array1.pop();
	assert(removedItem.value == 4);
	assert(removedItem.result == 0);
	assert(array1.length() == 1);
	assert(array1.capacity() == 4);
	array1.append(4);

	MemorySpace remaining = array1.sizeToCurrentLength();
	assert(array1.get(0) == 5);
	assert(array1.get(1) == 4);
	assert(array1.length() == 2);
	assert(array1.capacity() == 2);
	assert(sixInts[0] == originalMemValue);
	assert(sixInts[5] == originalMemValue);

	assert(remaining.content == &sixInts[3]);
	assert(remaining.sizeInBytes == 2 * sizeof(int));

	// Write to first item of remaining and assert array content remains unchanged
	int* firstItemInRemainingSpace = (int *)remaining.content;
	*firstItemInRemainingSpace = 3;
	assert(array1.get(0) == 5);
	assert(array1.get(1) == 4);

	assert(array1.length() == 2);
	array1.clear();
	assert(array1.length() == 0);
}

void RunQueueTests()
{
	int sizeInBytes = 4 * sizeof(int);
	void* memory = malloc(sizeInBytes);
	MemorySpace fourInts;
	fourInts.content = memory;
	fourInts.sizeInBytes = sizeInBytes;

	queue<int> queue1 = queue<int>(fourInts);

	assert(queue1.length() == 0);
	assert(queue1.capacity() == 4);

	assert(queue1.enqueue(1) == 0);
	assert(queue1.length() == 1);
	assert(queue1.capacity() == 4);

	assert(queue1.enqueue(2) == 0);
	assert(queue1.length() == 2);
	assert(queue1.capacity() == 4);

	assert(queue1.enqueue(3) == 0);
	assert(queue1.length() == 3);
	assert(queue1.capacity() == 4);

	assert(queue1.enqueue(4) == 0);
	assert(queue1.length() == 4);
	assert(queue1.capacity() == 4);

	assert(queue1.enqueue(5) == 1);
	assert(queue1.length() == 4);
	assert(queue1.capacity() == 4);

	int first = queue1.dequeue();
	assert(first == 1);
	assert(queue1.length() == 3);
	assert(queue1.capacity() == 3);

	int second = queue1.dequeue();
	assert(second == 2);
	assert(queue1.length() == 2);
	assert(queue1.capacity() == 2);

	int third = queue1.dequeue();
	assert(third == 3);
	assert(queue1.length() == 1);
	assert(queue1.capacity() == 1);

	int fourth = queue1.dequeue();
	assert(fourth == 4);
	assert(queue1.length() == 0);
	assert(queue1.capacity() == 0);

	assert(queue1.enqueue(1) == 1);
	assert(queue1.length() == 0);
	assert(queue1.capacity() == 0);

	assert(queue1.enqueue(2) == 1);
	assert(queue1.length() == 0);
	assert(queue1.capacity() == 0);
}

void RunDataStructureTests()
{
	printf("\n===== Array =====\n");
	RunArrayTests();

	printf("\n===== Queue =====\n");
	RunQueueTests();
}
