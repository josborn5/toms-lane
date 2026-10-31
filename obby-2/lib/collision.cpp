namespace col {

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
	tl::Vec2<float> position;
};

struct CollisionResult
{
	float time = 0.0f;
	Collision collisions[2];
};

CollisionResult CheckStaticAndMovingRectCollision(
	const tl::Rect<float>& staticRect,
	const tl::Rect<float>& movingRect,
	const tl::Vec2<float>& movingRectVelocityB,
	float maxCollisionTime
)
{
	CollisionResult result = CollisionResult();
	float blockTopSide = staticRect.y_max() + movingRect.halfSize.y;
	float blockBottomSide = staticRect.y_min() - movingRect.halfSize.y;
	float blockLeftSide = staticRect.x_min() - movingRect.halfSize.x;
	float blockRightSide = staticRect.x_max() + movingRect.halfSize.x;

	const tl::Vec2<float>& movingRectPositionB = movingRect.position;

	// Check for overlap
	if (movingRectPositionB.y < blockTopSide && movingRectPositionB.y > blockBottomSide
		&& movingRectPositionB.x < blockRightSide && movingRectPositionB.x > blockRightSide)
	{
		result.time = 0.0f;
		Collision movingRect = Collision();
		movingRect.side = Overlap;
		movingRect.position = movingRectPositionB;
		result.collisions[0] = movingRect;
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
			// TODO: if (
			// 			ballXPosAtCollision == blockLeftSide
			// 			|| ballXPosAtCollision || blockRightSide)
			// 				AND (movingRectVelocityB.x == 0.0f)
			// then the moving rect is 'brushing' against the static rect.
			// Need to convey that in the return value somehow.
			if (ballXPosAtCollision > blockLeftSide && ballXPosAtCollision < blockRightSide && tYCollision <= maxCollisionTime)
			{
				result.time = tYCollision;
				Collision movingRect;
				movingRect.side = verCollisionResult;
				movingRect.position = tl::Vec2<float> { ballXPosAtCollision, yCollisionCheckPos };
				result.collisions[0] = movingRect;
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
			// TODO: if (ballYPosAtCollision == blockBottomSide || ballYPosAtCollision == blockTopSide) AND (movingRectVelocityB.y == 0.0f) then the moving rect is 'brushing' against the static rect. Need to convey that in the return value somehow.
			if (ballYPosAtCollision > blockBottomSide && ballYPosAtCollision < blockTopSide && tXCollision <= maxCollisionTime)
			{
				result.time = tXCollision;
				Collision movingRect = Collision();
				movingRect.side = horCollisionResult;
				movingRect.position = tl::Vec2<float> { xCollisionCheckPos, ballYPosAtCollision };
				result.collisions[0] = movingRect;
			}
		}
	}
	return result;
}

}

