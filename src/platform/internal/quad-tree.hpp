#ifndef TOMS_LANE_QUAD_TREE_H
#define TOMS_LANE_QUAD_TREE_H

#include "./platform.hpp"

namespace tl
{
	template<typename T>
	struct QuadTreeNode
	{
		public:
			QuadTreeNode(const Rect<float> footprint)
			{
				_footprint = footprint;
			}

			int insert(const T& value, const Vec2<float> position)
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
					return 1;
				}

				if (_valueCount < _capacity)
				{
					_values[_valueCount] = value;
					_valueCount += 1;

					return 0;
				}

				return 1;
			}

			int query(const Rect<float> footprint, HeapArray<T>& foundValues)
			{
				if (_valueCount > 0) {
					foundValues.append(_values[0]);
				}

				return 0;
			}

		private:
			const int _capacity = 4;
			bool _hasChildren = false;
			Rect<float> _footprint;
			T _values[4] = { 0 };
			int _valueCount = 0;
			QuadTreeNode<T>* nwChild;
			QuadTreeNode<T>* neChild;
			QuadTreeNode<T>* seChild;
			QuadTreeNode<T>* swChild;
	};
}
#endif
