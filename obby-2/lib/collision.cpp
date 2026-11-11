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

typedef float (*position_func)(float t, void* context);
typedef float (*time_func)(float position, void* context);

struct linear_movement_context {
	float start_position = 0.0f;
	float velocity = 0.0f;
};

float linear_position_func(float time, void* context) {
	linear_movement_context* typed_context = static_cast<linear_movement_context*>(context);
	float position = typed_context->start_position + (time * typed_context->velocity);
	return position;
}

float linear_time_func(float position, void* context) {
	linear_movement_context* typed_context = static_cast<linear_movement_context*>(context);
	float time = (position - typed_context->start_position) / typed_context->velocity;
	return time;
}

CollisionResult CheckStaticAndMovingRectCollision(
	const tl::Rect<float>& staticRect,
	const tl::Rect<float>& movingRect,
	const tl::Vec2<float>& movingRectVelocityB,
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

	linear_movement_context vertical_context = {
		movingRectPositionB.y,
		movingRectVelocityB.y
	};

	linear_movement_context horizontal_context = {
		movingRectPositionB.x,
		movingRectVelocityB.x
	};

	if (movingRectVelocityB.y != 0.0f)
	{
		float tYCollision = linear_time_func(yCollisionCheckPos, &vertical_context);
		if (tYCollision >= 0.0f)
		{
			float ballXPosAtCollision = linear_position_func(tYCollision, &horizontal_context);
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
		float tXCollision = linear_time_func(xCollisionCheckPos, &horizontal_context);
		if (tXCollision >= 0.0f)
		{
			float ballYPosAtCollision = linear_position_func(tXCollision, &vertical_context);
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

