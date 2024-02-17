#ifndef TOMS_LANE_QUAD_TREE_H
#define TOMS_LANE_QUAD_TREE_H

#include "./platform.hpp"

namespace tl
{
	static void get_rect_min_max(
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
				if (!footprintContainsPosition(_footprint, position))
				{
					return 0;
				}

				if (_valueCount < _capacity)
				{
					_values[_valueCount] = value;
					_positions[_valueCount] = { position.x, position.y };
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
				returnValue = seChild->insert(value, position);
				returnValue = swChild->insert(value, position);
				return returnValue;
			}

			int query(const Rect<float>& footprint, HeapArray<T>& foundValues)
			{
				float minFootprintX, maxFootprintX, minFootprintY, maxFootprintY;
				get_rect_min_max(footprint, minFootprintX, maxFootprintX, minFootprintY, maxFootprintY);

				float _minFootprintX, _maxFootprintX, _minFootprintY, _maxFootprintY;
				get_rect_min_max(_footprint, _minFootprintX, _maxFootprintX, _minFootprintY, _maxFootprintY);

				if ((minFootprintX > _maxFootprintX) ||
					(maxFootprintX < _minFootprintX) ||
					(minFootprintY > _maxFootprintY) ||
					(maxFootprintY < _minFootprintY)
				)
				{
					return 0;
				}

				for (int i = 0; i < _valueCount; i += 1)
				{
					T checkValue = _values[i];
					if (footprintContainsPosition(footprint, _positions[i]))
					{
						foundValues.append(checkValue);
					}
				}

				if (nwChild != nullptr)
				{
					nwChild->query(footprint, foundValues);
					neChild->query(footprint, foundValues);
					seChild->query(footprint, foundValues);
					swChild->query(footprint, foundValues);
				}

				return 0;
			}

			void clear()
			{
				_valueCount = 0;
				if (nwChild != nullptr)
				{
					nwChild->clear();
					neChild->clear();
					seChild->clear();
					swChild->clear();
				}
			}

		private:
			int _capacity = 4;
			Rect<float> _footprint;
			HeapArray<QuadTreeNode<T>>* _space = nullptr;
			T _values[4] = { 0 };
			Vec2<float> _positions[4] = { 0 };
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

				// north west
				Vec2<float> childPos = {
					_footprint.position.x - childHalfSize.x,
					_footprint.position.y + childHalfSize.y
				};
				Rect<float> footprint;
				footprint.halfSize = childHalfSize;
				footprint.position = childPos;
				appendChild(footprint, &nwChild);

				// north east
				footprint.position.x += _footprint.halfSize.x;
				appendChild(footprint, &neChild);

				// south east
				footprint.position.y -= _footprint.halfSize.y;
				appendChild(footprint, &seChild);

				// south west
				footprint.position.x -= _footprint.halfSize.x;
				appendChild(footprint, &swChild);
			}

			void appendChild(
				const Rect<float>& footprint,
				QuadTreeNode<T>** target
			) {
				QuadTreeNode<T> newChild = QuadTreeNode(footprint, _space);
				_space->append(newChild);
				*target = _space->getTailPointer();
			}

			bool footprintContainsPosition
			(
				const Rect<float>& footprint,
				const Vec2<float>& position
			)
			{
				float minFootprintX, maxFootprintX, minFootprintY, maxFootprintY;
				get_rect_min_max(footprint, minFootprintX, maxFootprintX, minFootprintY, maxFootprintY);

				return position.x > minFootprintX &&
						position.x <= maxFootprintX &&
						position.y > minFootprintY &&
						position.y <= maxFootprintY;
			}
	};
}

#endif

