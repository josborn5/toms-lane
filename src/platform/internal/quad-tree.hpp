#ifndef TOMS_LANE_QUAD_TREE_H
#define TOMS_LANE_QUAD_TREE_H

#include "./platform.hpp"

namespace tl
{
	template<typename T>
	struct QuadTreeNode
	{
		public:
			QuadTreeNode() {}

			QuadTreeNode(const Rect<float>& footprint)
			{
				_footprint = footprint;
			}

			QuadTreeNode(
				const Rect<float>& footprint,
				HeapArray<QuadTreeNode<T>>& space
			)
			{
				_footprint = footprint;
				_space = &space;
			}

			int insert(const T& value, const Vec2<float>& position)
			{
				float minFootprintX = _footprint.position.x - _footprint.halfSize.x;
				float maxFootprintX = _footprint.position.x + _footprint.halfSize.x;
				float minFootprintY = _footprint.position.y - _footprint.halfSize.y;
				float maxFootprintY = _footprint.position.y + _footprint.halfSize.y;

				if (position.x < minFootprintX ||
					position.x > maxFootprintX ||
					position.y < minFootprintY ||
					position.y > maxFootprintY)
				{
					return 0;
				}

				if (_valueCount < _capacity)
				{
					_values[_valueCount] = value;
					_valueCount += 1;

					return 0;
				}
				if (!_hasChildren)
				{
					return 1;
				}

				int returnValue = 0;
				returnValue = nwChild->insert(value, position);
				returnValue = neChild->insert(value, position);
				returnValue = swChild->insert(value, position);
				returnValue = seChild->insert(value, position);

				return returnValue;
			}

			int query(const Rect<float> footprint, HeapArray<T>& foundValues)
			{
				for (int i = 0; i < _valueCount; i += 1)
				{
					foundValues.append(_values[i]);
				}

				return 0;
			}

		private:
			const int _capacity = 4;
			bool _hasChildren = false;
			Rect<float> _footprint;
			HeapArray<QuadTreeNode<T>>* _space = nullptr;
			T _values[4] = { 0 };
			int _valueCount = 0;
			QuadTreeNode<T>* nwChild;
			QuadTreeNode<T>* neChild;
			QuadTreeNode<T>* seChild;
			QuadTreeNode<T>* swChild;
	};
}
#endif
