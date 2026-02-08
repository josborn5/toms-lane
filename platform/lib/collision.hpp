#ifndef TOMS_LANE_COLLISION_H
#define TOMS_LANE_COLLISION_H

#include "./math.hpp"
#include "./geometry.hpp"

namespace tl
{
	enum CollisionSide
	{
		None,
		Top,
		Right,
		Bottom,
		Left,
		Overlap
	};

	struct Collision
	{
		CollisionSide side = None;
		Vec2<float> position;
	};

	struct CollisionResult
	{
		float time = 0.0f;
		Collision collisions[2];
		Collision a;
		Collision b;
	};

	CollisionResult CheckRectAndXLineCollision(
		float wallYPos,
		float wallFaceDir, // +ve value means the wall faces upwards (in +ve y direction). -ve value means wall faces downwards in the -ve y direction.
		const Rect<float> &rect,
		float maxCollisionTime
	);

	CollisionResult CheckRectAndYLineCollision(
		float wallXPos,
		float wallFaceDir, // +ve value means the wall faces right (in +ve x direction). -ve value means wall faces left in the -ve x direction.
		const Rect<float> &rect,
		float maxCollisionTime
	);

	CollisionResult CheckCollisionBetweenRects(
		const Rect<float> &aRect,
		const Rect<float> &bRect,
		float maxCollisionTime
	);
}

#endif
