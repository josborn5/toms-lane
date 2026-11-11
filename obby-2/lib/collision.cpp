namespace col {

enum CollisionSide
{
	None,
	PositiveFace,
	NegativeFace,
	Overlap
};

struct Collision
{
	CollisionSide side = None; // relative to static
	tl::Vec2<float> position;
};

struct CollisionResult
{
	float time = 0.0f;
	Collision x_collision;
	Collision y_collision;
};

typedef float (position_func)(float t, const void* context);
typedef float (time_func)(float position, const void* context);

CollisionResult CheckStaticAndMovingRectCollision(
	const tl::Rect<float>& staticRect,
	const tl::Rect<float>& movingRect,
	const position_func* position_function,
	const time_func* time_function,
	const void* vertical_context,
	const void* horizontal_context,
	tl::Vec2<float>& movingRectVelocityB,
	float maxCollisionTime
)
{
	CollisionResult result;
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
		Collision movingRect;
		movingRect.side = Overlap;
		movingRect.position = movingRectPositionB;
		result.x_collision.position = movingRectPositionB;
		result.x_collision.side = Overlap;
		result.y_collision.position = movingRectPositionB;
		result.y_collision.side = Overlap;
		return result;
	}

	// Check for collision between block side and ball path
	// 1. Top/bottom side
	CollisionSide verCollisionResult = None;
	float yCollisionCheckPos = 0.0f;
	if (movingRectVelocityB.y > 0.0f)
	{
		yCollisionCheckPos = blockBottomSide;
		verCollisionResult = NegativeFace;
	}
	if (movingRectVelocityB.y < 0.0f)
	{
		yCollisionCheckPos = blockTopSide;
		verCollisionResult = PositiveFace;
	}

	if (movingRectVelocityB.y != 0.0f)
	{
		float tYCollision = time_function(yCollisionCheckPos, vertical_context);
		if (tYCollision >= 0.0f)
		{
			float ballXPosAtCollision = position_function(tYCollision, horizontal_context);
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
				result.y_collision = movingRect;
			}
		}
	}

	// 2. Left/right side, 
	CollisionSide horCollisionResult = None;
	float xCollisionCheckPos = 0.0f;
	if (movingRectVelocityB.x > 0.0f)
	{
		xCollisionCheckPos = blockLeftSide;
		horCollisionResult = NegativeFace;
	}
	if (movingRectVelocityB.x < 0.0f)
	{
		xCollisionCheckPos = blockRightSide;
		horCollisionResult = PositiveFace;
	}

	if (movingRectVelocityB.x != 0.0f)
	{
		float tXCollision = time_function(xCollisionCheckPos, horizontal_context);
		if (tXCollision >= 0.0f)
		{
			float ballYPosAtCollision = position_function(tXCollision, vertical_context);
			// TODO: if (ballYPosAtCollision == blockBottomSide || ballYPosAtCollision == blockTopSide) AND (movingRectVelocityB.y == 0.0f) then the moving rect is 'brushing' against the static rect. Need to convey that in the return value somehow.
			if (ballYPosAtCollision > blockBottomSide && ballYPosAtCollision < blockTopSide && tXCollision <= maxCollisionTime)
			{
				result.time = tXCollision;
				Collision movingRect;
				movingRect.side = horCollisionResult;
				movingRect.position = tl::Vec2<float> { xCollisionCheckPos, ballYPosAtCollision };
				result.x_collision = movingRect;
			}
		}
	}
	return result;
}

}

