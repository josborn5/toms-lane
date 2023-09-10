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
				HeapArray<QuadTreeNode<T>>* space
			)
			{
				_footprint = footprint;
				_space = space;
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
				if (nwChild == nullptr)
				{
					split();
				}

				int returnValue = 0;
				returnValue = nwChild->insert(value, position);
				returnValue = neChild->insert(value, position);
				// returnValue = swChild->insert(value, position);
				// returnValue = seChild->insert(value, position);

				return returnValue;
			}

			int query(const Rect<float>& footprint, HeapArray<T>& foundValues)
			{
				for (int i = 0; i < _valueCount; i += 1)
				{
					foundValues.append(_values[i]);
				}

				if (nwChild != nullptr)
				{
					nwChild->query(footprint, foundValues);
					neChild->query(footprint, foundValues);
				}

				return 0;
			}

		private:
			int _capacity = 4;
			Rect<float> _footprint;
			HeapArray<QuadTreeNode<T>>* _space = nullptr;
			T _values[4] = { 0 };
			int _valueCount = 0;
			QuadTreeNode<T>* nwChild = nullptr;
			QuadTreeNode<T>* neChild = nullptr;
			QuadTreeNode<T>* seChild = nullptr;
			QuadTreeNode<T>* swChild = nullptr;

			void split()
			{
				Vec2<float> childHalfSize = {
					0.5f * _footprint.halfSize.x,
					0.5f * _footprint.halfSize.y
				};
				Vec2<float> childPos = {
					_footprint.position.x - childHalfSize.x,
					_footprint.position.y + childHalfSize.y
				};
				Rect<float> footprint;
				footprint.halfSize = childHalfSize;
				footprint.position = childPos;
				QuadTreeNode<T> nw = QuadTreeNode(footprint, _space);
				_space->append(nw);
				nwChild = _space->getTailPointer();

				// north east
				footprint.position.x += _footprint.halfSize.x;
				QuadTreeNode<T> ne = QuadTreeNode(footprint, _space);
				_space->append(ne);
				neChild = _space->getTailPointer();
			}
	};
}
#endif

