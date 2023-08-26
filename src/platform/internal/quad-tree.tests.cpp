#include "./quad-tree.hpp"
#include <stdio.h>
#include <assert.h>

void InsertSingleValue()
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

void RunQuadTreeTests()
{
	printf("========= Quad Tree Tests ========\n\n");

	InsertSingleValue();
}

