#include "./quad-tree.hpp"
#include <stdio.h>
#include <assert.h>

void InsertSingleIntValue()
{
	int insertValue = 1234;
	Rect<float> rootFootprint;
	rootFootprint.position = { 1.0f, 1.0f };
	rootFootprint.halfSize = { 1.0f, 1.0f };

	QuadTreeNode<int> rootNode = QuadTreeNode<int>(rootFootprint);

	rootNode.insert(insertValue, { 1.0f, 1.0f });

	int queryResultStore[5] = { 0, 0, 0, 0, 0};
	HeapArray<int> queryResults = HeapArray<int>(queryResultStore, 5);

	rootNode.query(rootFootprint, queryResults);

	int found = queryResults.get(0);
	assert(found == insertValue);
}

void InsertSingleReferenceValue()
{
	Rect<float> insertValue;
	insertValue.position = { 0.0f, 2.0f };
	Rect<float> rootFootprint;
	rootFootprint.position = { 1.0f, 1.0f };
	rootFootprint.halfSize = { 1.0f, 1.0f };

	QuadTreeNode<Rect<float>*> rootNode = QuadTreeNode<Rect<float>*>(rootFootprint);

	rootNode.insert(&insertValue, { 1.0f, 1.0f });

	Rect<float>* queryResultStore[5] = {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
	HeapArray<Rect<float>*> queryResults = HeapArray<Rect<float>*>(queryResultStore, 5);

	rootNode.query(rootFootprint, queryResults);

	Rect<float>* found = queryResults.get(0);
	assert(found->position.x == insertValue.position.x);
	assert(found->position.y == insertValue.position.y);
	assert(found == &insertValue);
	Rect<float>* shouldBeNull = queryResults.get(1);
	assert(shouldBeNull == nullptr);
}


void RunQuadTreeTests()
{
	printf("========= Quad Tree Tests ========\n\n");

	InsertSingleIntValue();

	InsertSingleReferenceValue();
}

