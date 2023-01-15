void CheckStaticAndMovingRectCollisionWithTime(
	Vec2<float> bPosition0,
	Vec2<float> bVelocity,
	float expectedCollisionTime,
	CollisionSide expectedCollisionResult,
	Vec2<float> expectedCollisionPosition,
	float collisionTime)
{
	Rect<float> aRect;
	aRect.halfSize = oneByOneHalfSize;
	aRect.position = originPosition;
	aRect.velocity = Vec2<float> { 0.0f, 0.0f };

	Rect<float> bRect;
	bRect.halfSize = oneByOneHalfSize;
	bRect.position = bPosition0;
	bRect.velocity = bVelocity;

	CollisionResult result = CheckCollisionBetweenRects(aRect, bRect, collisionTime);

	printf("\n===== Static & Moving =====\n");
	printf("actual collisionResult is %d\n", result.collisions[0].side);
	printf("expected collisionResult is %d\n", expectedCollisionResult);
	printf("actual collisionTime is %f\n", result.time);
	printf("expected collisionTime is %f\n", expectedCollisionTime);
	assert(result.collisions[0].side == expectedCollisionResult);
	assert(result.time == expectedCollisionTime);

	if (expectedCollisionResult != None)
	{
		printf("actual collisionPosition.x is %f\n", result.collisions[0].position.x);
		printf("expected collisionPosition.x is %f\n", expectedCollisionPosition.x);
		printf("actual collisionPosition.y is %f\n", result.collisions[0].position.y);
		printf("expected collisionPosition.y is %f\n", expectedCollisionPosition.y);
		assert(result.collisions[0].position.x == expectedCollisionPosition.x);
		assert(result.collisions[0].position.y == expectedCollisionPosition.y);
	}
}

void CheckStaticAndMovingRectCollision(
	Vec2<float> bPosition0,
	Vec2<float> bVelocity,
	float expectedCollisionTime,
	CollisionSide expectedCollisionResult,
	Vec2<float> expectedCollisionPosition)
{
	CheckStaticAndMovingRectCollisionWithTime(
		bPosition0,
		bVelocity,
		expectedCollisionTime,
		expectedCollisionResult,
		expectedCollisionPosition,
		originalCollisionTime
	);
}

void RunStaticAndMovingRectCollisionTests()
{
		/* A <--B--> Right Hand Side collisions */
	printf("\n// collision on x-axis\n");
	CheckStaticAndMovingRectCollision(blockRightOfOrigin, movingLeft, 1, Right, Vec2<float> { 2.0f, 0.0f });

	printf("\n// No collision on x-axis when moving too slow to collide before collision time boundary\n");
	CheckStaticAndMovingRectCollision(blockRightOfOrigin, Vec2<float> { -0.19999998f, 0.0f }, 0.0f, None, Vec2<float> { 4.0f, 0.0f });

	printf("\n// Collision on x-axis when moving slow to collide on collision time\n");
	CheckStaticAndMovingRectCollision(blockRightOfOrigin, Vec2<float> { -0.20000002f, 0.0f }, 9.99999905f, Right, Vec2<float> { 2.0f, 0.0f });

	printf("\n// No collision on x axis when moving away from each other\n");
	CheckStaticAndMovingRectCollision(blockRightOfOrigin, movingRight, 0.0f, None, Vec2<float> { 4.0f, 0.0f });

	printf("\n// Collision on x axis when moving toward each other off center at boundary\n");
	CheckStaticAndMovingRectCollision(Vec2<float> { 4.0f, 2.0f }, movingLeft, 1, Right, Vec2<float> { 2.0f, 2.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(Vec2<float> { 4.0f, 2.000001f }, movingLeft, 0.0f, None, Vec2<float> { 4.0f, 2.000001f });

	// Collision when already touching and moving toward each other
	CheckStaticAndMovingRectCollision(Vec2<float> { 2.0f, 0.0f }, movingLeft, 0.0f, Right, Vec2<float> { 2.0f, 0.0f });

	// No collision when already touching and moving away from each other
	CheckStaticAndMovingRectCollision(Vec2<float> { 2.0f, 0.0f }, movingRight, 0.0f, None, Vec2<float> { 2.0f, 0.0f });


	/* <--B--> A Left Hand Side collisions */
	// collision on x-axis
	CheckStaticAndMovingRectCollision(blockLeftOfOrigin, movingRight, 1, Left, Vec2<float> { -2.0f, 0.0f });

	printf("\n// No collision on x-axis when moving too slow to collide before collision time boundary\n");
	CheckStaticAndMovingRectCollision(blockLeftOfOrigin, Vec2<float> { 0.19999999f, 0.0f }, 0.0f, None, Vec2<float> { -4.0f, 0.0f });

	// Collision on x-axis when moving slow to collide on collision time
	CheckStaticAndMovingRectCollision(blockLeftOfOrigin, Vec2<float> { 0.20000002f, 0.0f }, 9.99999905f, Left, Vec2<float> { -2.0f, 0.0f });

	// No collision on x axis when moving away from each other
	CheckStaticAndMovingRectCollision(blockLeftOfOrigin, movingLeft, 0.0f, None, Vec2<float> { -4.0f, 0.0f });

	// Collision on x axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(blockLeftOfOrigin, movingRight, 1, Left, Vec2<float> { -2.0f, 0.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(Vec2<float> { -4.0f, 2.000001f }, movingRight, 0.0f, None, Vec2<float> { -4.0f, 2.000001f });

	// Collision when already touching and moving toward each other
	CheckStaticAndMovingRectCollision(Vec2<float> { -2.0f, 0.0f }, movingRight, 0.0f, Left, Vec2<float> { -2.0f, 0.0f });

	// No collision when already touching and moving away from each other
	CheckStaticAndMovingRectCollision(Vec2<float> { -2.0f, 0.0f }, movingLeft, 0.0f, None, Vec2<float> { -2.0f, 0.0f });

	/* A	Bottom Side collisions
	*
	* Λ
	* |
	* B
	* |
	* V
	*/
	// collision on y-axis
	CheckStaticAndMovingRectCollision(blockBelowOrigin, movingUp, 1, Bottom, Vec2<float> { 0.0f, -2.0f });

	printf("\n// No collision on y-axis when moving too slow to collide before collision time boundary\n");
	CheckStaticAndMovingRectCollision(blockBelowOrigin, Vec2<float> { 0.0f, 0.19999999f }, 0.0f, None, Vec2<float> { 0.0f, -4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	CheckStaticAndMovingRectCollision(blockBelowOrigin, Vec2<float> { 0.0f, 0.20000002f }, 9.99999905f, Bottom, Vec2<float> { 0.0f, -2.0f });

	// No collision on y axis when moving away from each other
	CheckStaticAndMovingRectCollision(blockBelowOrigin, movingDown, 0.0f, None, Vec2<float> { 0.0f, -4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(blockBelowOrigin, movingUp, 1, Bottom, Vec2<float> { 0.0f, -2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(Vec2<float> { 2.000001f, -4.0f }, movingUp, 0.0f, None, Vec2<float> { 2.000001f, -4.0f });

	// Collision when already touching and moving toward each other
	CheckStaticAndMovingRectCollision(Vec2<float> { 0.0f, -2.0f }, movingUp, 0.0f, Bottom, Vec2<float> { 0.0f, -2.0f });

	// No collision when already touching and moving away from each other
	CheckStaticAndMovingRectCollision(Vec2<float> { 0.0f, -2.0f }, movingDown, 0.0f, None, Vec2<float> { 0.0f, -2.0f });

	/* Λ	Top Side collisions
	 * |
	 * B
	 * |
	 * V
	 *
	 * A
	 */
	// collision on y-axis
	CheckStaticAndMovingRectCollision(blockAboveOrigin, movingDown, 1, Top, Vec2<float> { 0.0f, 2.0f });

	printf("\n// No collision on y-axis when moving too slow to collide before collision time boundary\n");
	CheckStaticAndMovingRectCollision(blockAboveOrigin, Vec2<float> { 0.0f, -0.19999999f }, 0.0f, None, Vec2<float> { 0.0f, 4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	CheckStaticAndMovingRectCollision(blockAboveOrigin, Vec2<float> { 0.0f, -0.20000002f }, 9.99999905f, Top, Vec2<float> { 0.0f, 2.0f });

	// No collision on y axis when moving away from each other
	CheckStaticAndMovingRectCollision(blockAboveOrigin, movingUp, 0.0f, None, Vec2<float> { 0.0f, 4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(Vec2<float> { 2.0f, 4.0f }, movingDown, 1, Top, Vec2<float> { 2.0f, 2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	CheckStaticAndMovingRectCollision(Vec2<float> { 2.000001f, 4.0f }, movingDown, 0.0f, None, Vec2<float> { 2.000001f, 4.0f });

	// Collision when already touching and moving toward each other
	CheckStaticAndMovingRectCollision(Vec2<float> { 0.0f, 2.0f }, movingDown, 0.0f, Top, Vec2<float> { 0.0f, 2.0f });

	printf("\n// Collision when already touching and moving toward each other and zero min collision time\n");
	CheckStaticAndMovingRectCollisionWithTime(Vec2<float> { 0.0f, 2.0f }, movingDown, 0.0f, Top, Vec2<float> { 0.0f, 2.0f }, 0.0f);

	printf("\n// No collision when already touching and moving away from each other\n");
	CheckStaticAndMovingRectCollision(Vec2<float> { 0.0f, 2.0f }, movingUp, 0.0f, None, Vec2<float> { 0.0f, 2.0f });

}