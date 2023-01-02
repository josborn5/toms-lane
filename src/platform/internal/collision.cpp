#include "./math.hpp"
#include "./geometry.hpp"
#include "./collision.hpp"

namespace tl
{
	CollisionResult CheckRectAndXLineCollision(
		float wallYPos,
		float wallFaceDir, // +ve value means the wall faces upwards (in +ve y direction). -ve value means wall faces downwards in the -ve y direction.
		const Rect<float> &rect,
		float maxCollisionTime
	)
	{
		CollisionResult result = CollisionResult();
		float yCollisionCheckPos = (wallFaceDir < 0.0f) ? wallYPos - rect.halfSize.y : wallYPos + rect.halfSize.y;

		// Check for overlap
		if (((wallFaceDir >= 0.0f) && (rect.position.y < yCollisionCheckPos))
			|| ((wallFaceDir < 0.0f) && (rect.position.y > yCollisionCheckPos))) {
			result.time = 0.0f;
			Collision rectCollision = Collision();
			rectCollision.side = Overlap;
			rectCollision.position = rect.position;
			result.collisions[0] = rectCollision;
			return result;
		}

		// Check if rect is moving toward the wall face
		if (rect.velocity.y >= 0 && wallFaceDir >= 0.0f) return result;
		if (rect.velocity.y <= 0 && wallFaceDir < 0.0f) return result;

		// If the rect is moving toward the wall face, check for collision between ball and topside of wall
		float tCollision = (yCollisionCheckPos - rect.position.y) / rect.velocity.y;
		if (tCollision >= 0 && tCollision < maxCollisionTime)
		{
			result.time = tCollision;
			Collision rectCollision = Collision();
			rectCollision.side = (wallFaceDir < 0.0f) ? Top : Bottom;
			float xCollisionPos = rect.position.x + (tCollision * rect.velocity.x);
			rectCollision.position = Vec2<float> { xCollisionPos, yCollisionCheckPos };
			result.collisions[0] = rectCollision;
		}
		return result;
	}

	CollisionResult CheckRectAndYLineCollision(
		float wallXPos,
		float wallFaceDir, // +ve value means the wall faces right (in +ve x direction). -ve value means wall faces left in the -ve x direction.
		const Rect<float> &rect,
		float maxCollisionTime
	)
	{
		CollisionResult result = CollisionResult();
		float xCollisionCheckPos = (wallFaceDir < 0.0f) ? wallXPos - rect.halfSize.x : wallXPos + rect.halfSize.x;

		// Check for overlap
		if (((wallFaceDir >= 0.0f) && (rect.position.x < xCollisionCheckPos))
			|| ((wallFaceDir < 0.0f) && (rect.position.x > xCollisionCheckPos))) {
			result.time = 0.0f;
			Collision rectCollision = Collision();
			rectCollision.side = Overlap;
			rectCollision.position = rect.position;
			result.collisions[0] = rectCollision;
			return result;
		}

		// Check if rect is moving toward the wall face
		if (rect.velocity.x >= 0 && wallFaceDir >= 0.0f) return result;
		if (rect.velocity.x <= 0 && wallFaceDir < 0.0f) return result;

		// If the rect is moving toward the wall face, check for collision between ball and topside of wall
		float tCollision = (xCollisionCheckPos - rect.position.x) / rect.velocity.x;
		if (tCollision >= 0 && tCollision < maxCollisionTime)
		{
			result.time = tCollision;
			Collision rectCollision = Collision();
			rectCollision.side = (wallFaceDir < 0.0f) ? Top : Bottom;
			float yCollisionPos = rect.position.y + (tCollision * rect.velocity.y);
			rectCollision.position = Vec2<float> { xCollisionCheckPos, yCollisionPos };
			result.collisions[0] = rectCollision;
		}
		return result;
	}

	CollisionResult CheckStaticAndMovingRectCollision(
		const Vec2<float> &staticRectHalfSizeA,
		const Vec2<float> &staticRectPositionA,
		const Vec2<float> &movingRectHalfSizeB,
		const Vec2<float> &movingRectPositionB,
		const Vec2<float> &movingRectVelocityB,
		float maxCollisionTime
	)
	{
		CollisionResult result = CollisionResult();
		float blockTopSide = staticRectPositionA.y + staticRectHalfSizeA.y + movingRectHalfSizeB.y;
		float blockBottomSide = staticRectPositionA.y - staticRectHalfSizeA.y - movingRectHalfSizeB.y;
		float blockLeftSide = staticRectPositionA.x - staticRectHalfSizeA.x - movingRectHalfSizeB.x;
		float blockRightSide = staticRectPositionA.x + staticRectHalfSizeA.x + movingRectHalfSizeB.x;

		// Check for overlap
		if (movingRectPositionB.y < blockTopSide && movingRectPositionB.y > blockBottomSide
			&& movingRectPositionB.x < blockRightSide && movingRectPositionB.x > blockRightSide)
		{
			result.time = 0.0f;
			Collision movingRect = Collision();
			movingRect.side = Overlap;
			movingRect.position = movingRectPositionB;
			result.collisions[0] = movingRect;
			result.a = movingRect;
		}

		// Check for collision between block side and ball path
		// 1. Top/bottom side
		CollisionSide verCollisionResult = None;
		float yCollisionCheckPos = 0.0f;
		if (movingRectVelocityB.y > 0.0f)
		{
			yCollisionCheckPos = blockBottomSide;
			verCollisionResult = Bottom;
		}
		if (movingRectVelocityB.y < 0.0f)
		{
			yCollisionCheckPos = blockTopSide;
			verCollisionResult = Top;
		}

		if (movingRectVelocityB.y != 0.0f)
		{
			float tYCollision = (yCollisionCheckPos - movingRectPositionB.y) / movingRectVelocityB.y;
			if (tYCollision >= 0.0f)
			{
				float ballXPosAtCollision = movingRectPositionB.x + (tYCollision * movingRectVelocityB.x);
				if (ballXPosAtCollision >= blockLeftSide && ballXPosAtCollision <= blockRightSide && tYCollision <= maxCollisionTime)
				{
					result.time = tYCollision;
					Collision movingRect = Collision();
					movingRect.side = verCollisionResult;
					movingRect.position = Vec2<float> { ballXPosAtCollision, yCollisionCheckPos };
					result.collisions[0] = movingRect;
					result.a = movingRect;
				}
			}
		}

		// 2. Left/right side, 
		CollisionSide horCollisionResult = None;
		float xCollisionCheckPos = 0.0f;
		if (movingRectVelocityB.x > 0.0f)
		{
			xCollisionCheckPos = blockLeftSide;
			horCollisionResult = Left;
		}
		if (movingRectVelocityB.x < 0.0f)
		{
			xCollisionCheckPos = blockRightSide;
			horCollisionResult = Right;
		}

		if (movingRectVelocityB.x != 0.0f)
		{
			float tXCollision = (xCollisionCheckPos - movingRectPositionB.x) / movingRectVelocityB.x;
			if (tXCollision >= 0.0f)
			{
				float ballYPosAtCollision = movingRectPositionB.y + (tXCollision * movingRectVelocityB.y);
				if (ballYPosAtCollision >= blockBottomSide && ballYPosAtCollision <= blockTopSide && tXCollision <= maxCollisionTime)
				{
					result.time = tXCollision;
					Collision movingRect = Collision();
					movingRect.side = horCollisionResult;
					movingRect.position = Vec2<float> { xCollisionCheckPos, ballYPosAtCollision };
					result.collisions[0] = movingRect;
				}
			}
		}
		return result;
	}

	CollisionResult CheckCollisionBetweenMovingRects(
		const Vec2<float> &aHalfSize,
		const Vec2<float> &aPosition0,
		const Vec2<float> &aVelocity,
		const Vec2<float> &bHalfSize,
		const Vec2<float> &bPosition0,
		const Vec2<float> &bVelocity,
		float maxCollisionTime
	)
	{
		// Calculate relative velocity as between a & b, as if a is static. i.e. the origin of our co-ordinate system is fixed to whereever object 'a' is
		Vec2<float> aRelBVelocity = SubtractVectors(bVelocity, aVelocity);

		CollisionResult result = CheckStaticAndMovingRectCollision(aHalfSize, aPosition0, bHalfSize, bPosition0, aRelBVelocity, maxCollisionTime);

		// 'a' is the static rect and 'b' is the moving rect in the relative coordinate system, so shift the result so that
		// collisions[0] refers to the 'a' rect
		// collisions[1] refers to the 'b' rect
		result.collisions[1] = result.collisions[0];
		// Translate bPosition from the co-ordinate system whose origin is on 'a' back to the static co-ordinate system
		if (result.collisions[1].side != None)
		{
			Vec2<float> deltaAPosition = MultiplyVectorByScalar(aVelocity, result.time);
			result.collisions[1].position = AddVectors(deltaAPosition, result.collisions[1].position);
			// TODO: set the collision result for aRect in result.collisions[0]!
		}
		return result;
	}

	CollisionResult CheckCollisionBetweenRects(
		const Rect<float> &aRect,
		const Rect<float> &bRect,
		float maxCollisionTime
	)
	{
		return CheckCollisionBetweenMovingRects(
			aRect.halfSize,
			aRect.position,
			aRect.velocity,
			bRect.halfSize,
			bRect.position,
			bRect.velocity,
			maxCollisionTime
		);
	}
}

