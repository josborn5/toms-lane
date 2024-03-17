#include "./quad-tree.hpp"

namespace tl
{
	static void get_rect_min_max1(
		const Rect<float>& rect,
		float& minX,
		float& maxX,
		float& minY,
		float& maxY
	) {
		minX = rect.position.x - rect.halfSize.x;
		maxX = rect.position.x + rect.halfSize.x;
		minY = rect.position.y - rect.halfSize.y;
		maxY = rect.position.y + rect.halfSize.y;
	}

	static bool get_rects_overlap1(
		const Rect<float>& rectA,
		const Rect<float>& rectB
	) {
		float minRectAX, maxRectAX, minRectAY, maxRectAY;
		get_rect_min_max1(rectA, minRectAX, maxRectAX, minRectAY, maxRectAY);

		float minRectBX, maxRectBX, minRectBY, maxRectBY;
		get_rect_min_max1(rectB, minRectBX, maxRectBX, minRectBY, maxRectBY);

		return maxRectAX >= minRectBX &&
			minRectAX <= maxRectBX &&
			maxRectAY >= minRectBY &&
			minRectAY <= maxRectBY;
	}

	static void rect_node_child_append(
		rect_node& node,
		const Rect<float>& footprint,
		rect_node** target
	) {
		rect_node child;
		child.footprint = footprint;
		child.space = node.space;
		node.space->append(child);
		*target = node.space->getTailPointer();
	}


	static void rect_node_split(rect_node& node)
	{
		Vec2<float> childHalfSize = {
			0.5f * node.footprint.halfSize.x,
			0.5f * node.footprint.halfSize.y
		};

		// north west
		Vec2<float> childPos = {
			node.footprint.position.x - childHalfSize.x,
			node.footprint.position.y + childHalfSize.y
		};
		Rect<float> child_footprint;
		child_footprint.halfSize = childHalfSize;
		child_footprint.position = childPos;
		rect_node_child_append(node, child_footprint, &node.nw);

		// north east
		child_footprint.position.x += node.footprint.halfSize.x;
		rect_node_child_append(node, child_footprint, &node.ne);

		// south east
		child_footprint.position.y -= node.footprint.halfSize.y;
		rect_node_child_append(node, child_footprint, &node.se);

		// south west
		child_footprint.position.x -= node.footprint.halfSize.x;
		rect_node_child_append(node, child_footprint, &node.sw);
	}


	int rect_node_insert(rect_node& node, const rect_node_value& value)
	{
		if (!get_rects_overlap1(node.footprint, value.footprint))
		{
			return 0;
		}

		if (node.count < node.capacity)
		{
			node.values[node.count] = value;
			node.count += 1;

			return 0;
		}
		if (node.nw == nullptr)
		{
			rect_node_split(node);
		}

		int returnValue = 0;
		returnValue = rect_node_insert(*node.nw, value);
		returnValue = rect_node_insert(*node.ne, value);
		returnValue = rect_node_insert(*node.se, value);
		returnValue = rect_node_insert(*node.sw, value);
		return returnValue;
	}


	void rect_node_clear(rect_node& node)
	{
		node.count = 0;
		if (node.nw == nullptr)
		{
			return;
		}

		rect_node_clear(*node.nw);
		rect_node_clear(*node.ne);
		rect_node_clear(*node.se);
		rect_node_clear(*node.sw);
	}


	int rect_node_query(rect_node& node, const Rect<float>& footprint, array<rect_node_value>& foundValues)
	{
		if (!get_rects_overlap1(footprint, node.footprint))
		{
			return 0;
		}

		for (int i = 0; i < node.count; i += 1)
		{
			rect_node_value check_value = node.values[i];
			if (get_rects_overlap1(footprint, check_value.footprint))
			{
				foundValues.append(check_value);
			}
		}

		if (node.nw != nullptr)
		{
			rect_node_query(*node.nw, footprint, foundValues);
			rect_node_query(*node.ne, footprint, foundValues);
			rect_node_query(*node.se, footprint, foundValues);
			rect_node_query(*node.sw, footprint, foundValues);
		}

		return 0;
	}
}

