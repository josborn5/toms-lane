using namespace tl;

CollisionResult CheckStaticAndMovingRectCollision(
	const Rect<float>& staticRect,
	const Rect<float>& movingRect,
	const Vec2<float>& movingRectVelocityB,
	float maxCollisionTime
)
{
	tl::CollisionResult result = CollisionResult();
	float blockTopSide = staticRect.y_max() + movingRect.halfSize.y;
	float blockBottomSide = staticRect.y_min() - movingRect.halfSize.y;
	float blockLeftSide = staticRect.x_min() - movingRect.halfSize.x;
	float blockRightSide = staticRect.x_max() + movingRect.halfSize.x;

	const Vec2<float>& movingRectPositionB = movingRect.position;

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
		verCollisionResult = tl::Bottom;
	}
	if (movingRectVelocityB.y < 0.0f)
	{
		yCollisionCheckPos = blockTopSide;
		verCollisionResult = tl::Top;
	}

	if (movingRectVelocityB.y != 0.0f)
	{
		float tYCollision = (yCollisionCheckPos - movingRectPositionB.y) / movingRectVelocityB.y;
		if (tYCollision >= 0.0f)
		{
			float ballXPosAtCollision = movingRectPositionB.x + (tYCollision * movingRectVelocityB.x);
			// TODO: if (ballXPosAtCollision == blockLeftSide || ballXPosAtCollision || blockRightSide) AND (movingRectVelocityB.x == 0.0f) then the moving rect is 'brushing' against the static rect. Need to convey that in the return value somehow.
			if (ballXPosAtCollision > blockLeftSide && ballXPosAtCollision < blockRightSide && tYCollision <= maxCollisionTime)
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
		horCollisionResult = tl::Left;
	}
	if (movingRectVelocityB.x < 0.0f)
	{
		xCollisionCheckPos = blockRightSide;
		horCollisionResult = tl::Right;
	}

	if (movingRectVelocityB.x != 0.0f)
	{
		float tXCollision = (xCollisionCheckPos - movingRectPositionB.x) / movingRectVelocityB.x;
		if (tXCollision >= 0.0f)
		{
			float ballYPosAtCollision = movingRectPositionB.y + (tXCollision * movingRectVelocityB.y);
			// TODO: if (ballYPosAtCollision == blockBottomSide || ballYPosAtCollision == blockTopSide) AND (movingRectVelocityB.y == 0.0f) then the moving rect is 'brushing' against the static rect. Need to convey that in the return value somehow.
			if (ballYPosAtCollision > blockBottomSide && ballYPosAtCollision < blockTopSide && tXCollision <= maxCollisionTime)
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
	const Rect<float>& aRect,
	const Vec2<float>& aVelocity,
	const Rect<float>& bRect,
	const Vec2<float>& bVelocity,
	float maxCollisionTime
)
{
	// Calculate relative velocity as between a & b, as if a is static. i.e. the origin of our co-ordinate system is fixed to whereever object 'a' is
	Vec2<float> aRelBVelocity = SubtractVectors(bVelocity, aVelocity);

	CollisionResult result = CheckStaticAndMovingRectCollision(
		aRect,
		bRect,
		aRelBVelocity,
		maxCollisionTime
	);

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
		aRect,
		aRect.velocity,
		bRect,
		bRect.velocity,
		maxCollisionTime
	);
}

