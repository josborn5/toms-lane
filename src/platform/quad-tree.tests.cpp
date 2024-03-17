#include "./quad-tree.hpp"
#include <stdio.h>
#include <assert.h>

int fourValues[4] = { 1, 2, 3, 4 };
int eightValues[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

Rect<float> Get1x1Footprint()
{
	Rect<float> footprint;
	footprint.position = { 1.0f, 1.0f };
	footprint.halfSize = { 1.0f, 1.0f };
	return footprint;
}

static rect_node_value GetValue(void* value, Vec2<float> position)
{
	rect_node_value nodeValue;
	nodeValue.value = value;
	nodeValue.footprint.position = position;
	return nodeValue;
}

void InsertSingleIntValue()
{
	int insertValue = 1234;
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node rootNode;
	rootNode.footprint = rootFootprint;

	rect_node_value nodeValue = GetValue(&insertValue, { 1.0f, 1.0f });
	rect_node_insert(rootNode, nodeValue);

	rect_node_value queryResultStore[5] = { 0 };
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 5);

	rect_node_query(rootNode, rootFootprint, queryResults);

	rect_node_value found = queryResults.get(0);
	assert(*(int*)found.value == insertValue);
}

void InsertSingleReferenceValue()
{
	Rect<float> insertValue;
	insertValue.position = { 0.0f, 2.0f };
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node rootNode;
	rootNode.footprint = rootFootprint;

	rect_node_value nodeValue = GetValue(&insertValue, { 1.0f, 1.0f });
	rect_node_insert(rootNode, nodeValue);

	rect_node_value queryResultStore[5] = { 0 };
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 5);

	rect_node_query(rootNode, rootFootprint, queryResults);

	rect_node_value foundNode = queryResults.get(0);
	Rect<float>* found = (Rect<float>*)foundNode.value;
	assert(found->position.x == insertValue.position.x);
	assert(found->position.y == insertValue.position.y);
	assert(found == &insertValue);

	rect_node_value nullNode = queryResults.get(1);
	Rect<float>* shouldBeNull = (Rect<float>*)nullNode.value;
	assert(shouldBeNull == nullptr);
}

void RejectsSingleValueOutsideOfFootprint()
{
	int insertValue = 1234;
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node rootNode;
	rootNode.footprint = rootFootprint;

	rect_node_value nodeValue = GetValue(&insertValue, { 4.0f, 4.0f });
	rect_node_insert(rootNode, nodeValue);

	rect_node_value notFound = rect_node_value();
	rect_node_value queryResultStore[5] = { notFound };
	
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 5);

	rect_node_query(rootNode, rootFootprint, queryResults);

	rect_node_value found = queryResults.get(0);
	assert(found.value == notFound.value);
}

void AssertIntValue(rect_node_value& actualValue, int expected)
{
	assert(actualValue.value != nullptr);
	int* actualIntValue = (int*)actualValue.value;
	assert(*actualIntValue == expected);
}

void InsertFourValues()
{
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node rootNode;
	rootNode.footprint = rootFootprint;

	rect_node_insert(rootNode, GetValue(&fourValues[0], { 0.5f, 0.5f, }));
	rect_node_insert(rootNode, GetValue(&fourValues[1], { 0.5f, 1.5f, }));
	rect_node_insert(rootNode, GetValue(&fourValues[2], { 1.5f, 0.5f, }));
	rect_node_insert(rootNode, GetValue(&fourValues[3], { 1.5f, 1.5f, }));

	rect_node_value queryResultStore[5] = { 0 };
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 5);

	rect_node_query(rootNode, rootFootprint, queryResults);

	assert(queryResults.length() == 4);
	AssertIntValue(queryResults.get(0), 1);
	AssertIntValue(queryResults.get(1), 2);
	AssertIntValue(queryResults.get(2), 3);
	AssertIntValue(queryResults.get(3), 4);
}

void InsertEightValues()
{
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node nodeArray[7];
	array<rect_node> nodes = array<rect_node>(nodeArray, 7);

	rect_node rootNode;
	rootNode.footprint = rootFootprint;
	rootNode.space = &nodes;

	Vec2<float> nwPos = { 0.5f, 1.5f };
	Vec2<float> nePos = { 1.5f, 1.5f };
	Vec2<float> swPos = { 0.5f, 0.5f };
	Vec2<float> sePos = { 1.5f, 0.5f };

	rect_node_insert(rootNode, GetValue(&eightValues[0], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[1], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[2], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[3], swPos));
	rect_node_insert(rootNode, GetValue(&eightValues[4], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[5], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[6], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[7], swPos));

	rect_node_value queryResultStore[10] = { 0 };
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 10);

	rect_node_query(rootNode, rootFootprint, queryResults);

	AssertIntValue(queryResults.get(0), 1);
	AssertIntValue(queryResults.get(1), 2);
	AssertIntValue(queryResults.get(2), 3);
	AssertIntValue(queryResults.get(3), 4);
	AssertIntValue(queryResults.get(4), 5);
	AssertIntValue(queryResults.get(5), 6);
	AssertIntValue(queryResults.get(6), 7);
	AssertIntValue(queryResults.get(7), 8);
}

void InsertEightValuesAndClear()
{
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node nodeArray[7];
	array<rect_node> nodes = array<rect_node>(nodeArray, 7);

	rect_node rootNode;
	rootNode.footprint = rootFootprint;
	rootNode.space = &nodes;

	Vec2<float> nwPos = { 0.5f, 1.5f };
	Vec2<float> nePos = { 1.5f, 1.5f };
	Vec2<float> swPos = { 0.5f, 0.5f };
	Vec2<float> sePos = { 1.5f, 0.5f };

	rect_node_insert(rootNode, GetValue(&eightValues[0], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[1], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[2], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[3], swPos));
	rect_node_insert(rootNode, GetValue(&eightValues[4], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[5], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[6], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[7], swPos));

	rect_node_value queryResultStore[10] = { 0 };
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 10);

	rect_node_query(rootNode, rootFootprint, queryResults);

	assert(queryResults.length() == 8);

	// clear everything
	rect_node_clear(rootNode);
	queryResults.clear();
	assert(queryResults.length() == 0);

	// query should return nothing after clearing the tree
	rect_node_query(rootNode, rootFootprint, queryResults);

	assert(queryResults.length() == 0);

	// inserting should be possible after clearing
	rect_node_insert(rootNode, GetValue(&eightValues[0], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[1], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[2], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[3], swPos));
	rect_node_insert(rootNode, GetValue(&eightValues[4], nwPos));

	rect_node_query(rootNode, rootFootprint, queryResults);
	assert(queryResults.length() == 5);
}

void QueryForSubSectionOfFootprint()
{
	Rect<float> rootFootprint = Get1x1Footprint();

	rect_node nodeArray[7];
	array<rect_node> nodes = array<rect_node>(nodeArray, 7);

	rect_node rootNode;
	rootNode.footprint = rootFootprint;
	rootNode.space = &nodes;

	Vec2<float> nwPos = { 0.5f, 1.5f };
	Vec2<float> nePos = { 1.5f, 1.5f };
	Vec2<float> swPos = { 0.5f, 0.5f };
	Vec2<float> sePos = { 1.5f, 0.5f };

	rect_node_insert(rootNode, GetValue(&eightValues[0], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[1], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[2], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[3], swPos));
	rect_node_insert(rootNode, GetValue(&eightValues[4], nwPos));
	rect_node_insert(rootNode, GetValue(&eightValues[5], nePos));
	rect_node_insert(rootNode, GetValue(&eightValues[6], sePos));
	rect_node_insert(rootNode, GetValue(&eightValues[7], swPos));

	rect_node_value queryResultStore[10] = { 0 };
	array<rect_node_value> queryResults = array<rect_node_value>(queryResultStore, 10);

	Rect<float> queryFootprint;
	queryFootprint.position = sePos;
	queryFootprint.halfSize = { 0.1f, 0.1f };

	rect_node_query(rootNode, queryFootprint, queryResults);

	assert(queryResults.length() == 2);
}

void RunQuadTreeTests()
{
	printf("========= Quad Tree Tests ========\n\n");

	InsertSingleIntValue();

	InsertSingleReferenceValue();

	RejectsSingleValueOutsideOfFootprint();

	printf("Insert 4 values\n");
	InsertFourValues();

	printf("Insert 8 values\n");
	InsertEightValues();

	printf("Insert 8 values and clear\n");
	InsertEightValuesAndClear();

	printf("Subsection query\n");
	QueryForSubSectionOfFootprint();
}
