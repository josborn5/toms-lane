#ifndef TOMS_LANE_GEOMETRY_H
#define TOMS_LANE_GEOMETRY_H

#include "./math.hpp"

namespace tl
{
	template<typename T>
	struct Rect
	{
		Vec2<T> position = {0};
		Vec2<T> halfSize = {0};
		Vec2<T> velocity = {0};

		T x_min() const
		{
			return position.x - halfSize.x;
		}

		T x_max() const
		{
			return position.x + halfSize.x;
		}

		T y_min() const
		{
			return position.y - halfSize.y;
		}

		T y_max() const
		{
			return position.y + halfSize.y;
		}
	};
}

#endif
