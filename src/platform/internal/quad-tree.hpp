#ifndef TOMS_LANE_QUAD_TREE_H
#define TOMS_LANE_QUAD_TREE_H
namespace tl
{
	template<typename T>
	struct QuadTreeNode
	{
		const int capacity = 4;
		int valueCount = 0;
		T* values[4];
		Rect<float> place;
		bool hasChildren;
		QuadTreeNode* nwChild;
		QuadTreeNode* neChild;
		QuadTreeNode* seChild;
		QuadTreeNode* swChild;
	}

	template<typename T>
	int InsertValueInQuadTree(QuadTreeNode& root, T* value, const Vec2<float>& position);

	template<typename T>
	int QueryValuesInRect(const QuadTreeNode& root, const Vec2<float>& position, HeapArray<T*>& foundValues);
}
#endif
