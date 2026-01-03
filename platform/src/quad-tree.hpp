#ifndef TOMS_LANE_QUAD_TREE_H
#define TOMS_LANE_QUAD_TREE_H

#include "./data-structures.hpp"

namespace tl
{
	struct rect_node_value
	{
		Rect<float> footprint;
		void* value;
	};

	struct rect_node
	{
		static const int capacity = 4;
		rect_node_value values[capacity];
		Rect<float> footprint;
		int count = 0;
		rect_node* nw = nullptr;
		rect_node* ne = nullptr;
		rect_node* se = nullptr;
		rect_node* sw = nullptr;
	};

	struct rect_tree
	{
		rect_node root;
		array<rect_node> descendents;
	};

	int rect_tree_insert(rect_tree& tree, const rect_node_value& value);
	int rect_tree_query(rect_tree& tree, const Rect<float>& footprint, array<rect_node_value>& foundValues);
	void rect_tree_clear(rect_tree& tree);
}

#endif

