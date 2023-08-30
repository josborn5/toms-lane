#include "./quad-tree.hpp"
#include <stdio.h>
#include <assert.h>

Rect<float> Get1x1Footprint()
{
	Rect<float> footprint;
	footprint.position = { 1.0f, 1.0f };
	footprint.halfSize = { 1.0f, 1.0f };
	return footprint;
}

void InsertSingleIntValue()
{
	int insertValue = 1234;
	Rect<float> rootFootprint = Get1x1Footprint();

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
	Rect<float> rootFootprint = Get1x1Footprint();

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

void RejectsSingleValueOutsideOfFootprint()
{
	int insertValue = 1234;
	Rect<float> rootFootprint = Get1x1Footprint();

	QuadTreeNode<int> rootNode = QuadTreeNode<int>(rootFootprint);

	rootNode.insert(insertValue, { 4.0f, 4.0f });

	int notFound = -1;
	int queryResultStore[5] = { notFound };
	HeapArray<int> queryResults = HeapArray<int>(queryResultStore, 5);

	rootNode.query(rootFootprint, queryResults);

	int found = queryResults.get(0);
	assert(found == notFound);
}

void InsertFourValues()
{
	Rect<float> rootFootprint = Get1x1Footprint();

	QuadTreeNode<int> rootNode = QuadTreeNode<int>(rootFootprint);

	rootNode.insert(1, { 0.5f, 0.5f });
	rootNode.insert(2, { 0.5f, 1.5f });
	rootNode.insert(3, { 1.5f, 0.5f });
	rootNode.insert(4, { 1.5f, 1.5f });

	int queryResultStore[5] = { 0, 0, 0, 0, 0};
	HeapArray<int> queryResults = HeapArray<int>(queryResultStore, 5);

	rootNode.query(rootFootprint, queryResults);

	assert(queryResults.get(0) == 1);
	assert(queryResults.get(1) == 2);
	assert(queryResults.get(2) == 3);
	assert(queryResults.get(3) == 4);
	assert(queryResults.get(4) == 0);
}

void InsertFiveValues()
{
	Rect<float> rootFootprint = Get1x1Footprint();

	QuadTreeNode<int> nodeArray[5];
	HeapArray<QuadTreeNode<int>> nodes = HeapArray<QuadTreeNode<int>>(nodeArray, 5);

	QuadTreeNode<int> rootNode = QuadTreeNode<int>(rootFootprint, &nodes);

	rootNode.insert(1, { 0.5f, 0.5f });
	rootNode.insert(2, { 0.5f, 1.5f });
	rootNode.insert(3, { 1.5f, 0.5f });
	rootNode.insert(4, { 1.5f, 1.5f });
	rootNode.insert(5, { 0.5f, 0.5f });

	int queryResultStore[5] = { 0, 0, 0, 0, 0};
	HeapArray<int> queryResults = HeapArray<int>(queryResultStore, 5);

	rootNode.query(rootFootprint, queryResults);

	assert(queryResults.get(0) == 1);
	assert(queryResults.get(1) == 2);
	assert(queryResults.get(2) == 3);
	assert(queryResults.get(3) == 4);
	assert(queryResults.get(4) == 0);
}

void RunQuadTreeTests()
{
	printf("========= Quad Tree Tests ========\n\n");

	InsertSingleIntValue();

	InsertSingleReferenceValue();

	RejectsSingleValueOutsideOfFootprint();

	InsertFourValues();

	InsertFiveValues();
}
