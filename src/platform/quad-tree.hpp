#ifndef TOMS_LANE_QUAD_TREE_H
#define TOMS_LANE_QUAD_TREE_H

#include "./platform.hpp"

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
		array<rect_node>* space = nullptr;
		int count = 0;
		rect_node* nw = nullptr;
		rect_node* ne = nullptr;
		rect_node* se = nullptr;
		rect_node* sw = nullptr;
	};

	int rect_node_insert(rect_node& node, const rect_node_value& value);
	int rect_node_query(rect_node& node, const Rect<float>& footprint, array<rect_node_value>& foundValues);
	void rect_node_clear(rect_node& node);
}

#endif

