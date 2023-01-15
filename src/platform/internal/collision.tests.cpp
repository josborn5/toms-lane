#define originalCollisionTime 10.0f
#define originPosition Vec2<float> { 0.0f, 0.0f }
#define oneByOneHalfSize Vec2<float> { 1.0f, 1.0f }

#define movingLeft Vec2<float> { -2.0f, 0.0f }
#define movingRight Vec2<float> { 2.0f, 0.0f }
#define movingUp Vec2<float> { 0.0f, 2.0f }
#define movingDown Vec2<float> { 0.0f, -2.0f }

#define blockRightOfOrigin Vec2<float> { 4.0f, 0.0f }
#define blockLeftOfOrigin Vec2<float> { -4.0f, 0.0f }
#define blockAboveOrigin Vec2<float> { 0.0f, 4.0f }
#define blockBelowOrigin Vec2<float> { 0.0f, -4.0f }

#include <stdio.h>
#include <assert.h>

#include "./collision.hpp"
#include "./collision.tests.moving-rects.cpp"

using namespace tl;

const float bottomWall = 1.0f;
const float topWall = -1.0f;

void RunCheckStaticAndMovingRectCollisionTestsWithTime(
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

void RunCheckStaticAndMovingRectCollisionTests(
	Vec2<float> bPosition0,
	Vec2<float> bVelocity,
	float expectedCollisionTime,
	CollisionSide expectedCollisionResult,
	Vec2<float> expectedCollisionPosition)
{
	RunCheckStaticAndMovingRectCollisionTestsWithTime(
		bPosition0,
		bVelocity,
		expectedCollisionTime,
		expectedCollisionResult,
		expectedCollisionPosition,
		originalCollisionTime
	);
}

void RunCheckRectAndXLineCollisionTest(
	const char *name,
	float wallFaceDir,
	Vec2<float> blockPosition0,
	Vec2<float> blockVelocity,
	float expectedCollisionTime,
	int expectedCollisionResult,
	Vec2<float> expectedCollisionPosition)
{
	float wallYPos = 0.0f;
	float collisionTime = originalCollisionTime;
	Rect<float> rect;
	rect.velocity = blockVelocity;
	rect.halfSize = oneByOneHalfSize;
	rect.position = blockPosition0;

	CollisionResult result = CheckRectAndXLineCollision(
		wallYPos,
		wallFaceDir,
		rect,
		collisionTime
	);

	printf("\n===== Wall & Moving =====\n");
	printf(name);
	printf("\n");
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
	printf("\n");
}

void RunCollisionTests()
{
	/*
	* CheckBlockAndBallCollision
	*
	*/

	/* A <--B--> Right Hand Side collisions */
	printf("\n// collision on x-axis\n");
	RunCheckStaticAndMovingRectCollisionTests(blockRightOfOrigin, movingLeft, 1, Right, Vec2<float> { 2.0f, 0.0f });

	printf("\n// No collision on x-axis when moving too slow to collide before collision time boundary\n");
	RunCheckStaticAndMovingRectCollisionTests(blockRightOfOrigin, Vec2<float> { -0.19999998f, 0.0f }, 0.0f, None, Vec2<float> { 4.0f, 0.0f });

	printf("\n// Collision on x-axis when moving slow to collide on collision time\n");
	RunCheckStaticAndMovingRectCollisionTests(blockRightOfOrigin, Vec2<float> { -0.20000002f, 0.0f }, 9.99999905f, Right, Vec2<float> { 2.0f, 0.0f });

	printf("\n// No collision on x axis when moving away from each other\n");
	RunCheckStaticAndMovingRectCollisionTests(blockRightOfOrigin, movingRight, 0.0f, None, Vec2<float> { 4.0f, 0.0f });

	printf("\n// Collision on x axis when moving toward each other off center at boundary\n");
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 4.0f, 2.0f }, movingLeft, 1, Right, Vec2<float> { 2.0f, 2.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 4.0f, 2.000001f }, movingLeft, 0.0f, None, Vec2<float> { 4.0f, 2.000001f });

	// Collision when already touching and moving toward each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 2.0f, 0.0f }, movingLeft, 0.0f, Right, Vec2<float> { 2.0f, 0.0f });

	// No collision when already touching and moving away from each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 2.0f, 0.0f }, movingRight, 0.0f, None, Vec2<float> { 2.0f, 0.0f });


	/* <--B--> A Left Hand Side collisions */
	// collision on x-axis
	RunCheckStaticAndMovingRectCollisionTests(blockLeftOfOrigin, movingRight, 1, Left, Vec2<float> { -2.0f, 0.0f });

	printf("\n// No collision on x-axis when moving too slow to collide before collision time boundary\n");
	RunCheckStaticAndMovingRectCollisionTests(blockLeftOfOrigin, Vec2<float> { 0.19999999f, 0.0f }, 0.0f, None, Vec2<float> { -4.0f, 0.0f });

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckStaticAndMovingRectCollisionTests(blockLeftOfOrigin, Vec2<float> { 0.20000002f, 0.0f }, 9.99999905f, Left, Vec2<float> { -2.0f, 0.0f });

	// No collision on x axis when moving away from each other
	RunCheckStaticAndMovingRectCollisionTests(blockLeftOfOrigin, movingLeft, 0.0f, None, Vec2<float> { -4.0f, 0.0f });

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(blockLeftOfOrigin, movingRight, 1, Left, Vec2<float> { -2.0f, 0.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { -4.0f, 2.000001f }, movingRight, 0.0f, None, Vec2<float> { -4.0f, 2.000001f });

	// Collision when already touching and moving toward each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { -2.0f, 0.0f }, movingRight, 0.0f, Left, Vec2<float> { -2.0f, 0.0f });

	// No collision when already touching and moving away from each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { -2.0f, 0.0f }, movingLeft, 0.0f, None, Vec2<float> { -2.0f, 0.0f });

	/* A	Bottom Side collisions
	*
	* Λ
	* |
	* B
	* |
	* V
	*/
	// collision on y-axis
	RunCheckStaticAndMovingRectCollisionTests(blockBelowOrigin, movingUp, 1, Bottom, Vec2<float> { 0.0f, -2.0f });

	printf("\n// No collision on y-axis when moving too slow to collide before collision time boundary\n");
	RunCheckStaticAndMovingRectCollisionTests(blockBelowOrigin, Vec2<float> { 0.0f, 0.19999999f }, 0.0f, None, Vec2<float> { 0.0f, -4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckStaticAndMovingRectCollisionTests(blockBelowOrigin, Vec2<float> { 0.0f, 0.20000002f }, 9.99999905f, Bottom, Vec2<float> { 0.0f, -2.0f });

	// No collision on y axis when moving away from each other
	RunCheckStaticAndMovingRectCollisionTests(blockBelowOrigin, movingDown, 0.0f, None, Vec2<float> { 0.0f, -4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(blockBelowOrigin, movingUp, 1, Bottom, Vec2<float> { 0.0f, -2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 2.000001f, -4.0f }, movingUp, 0.0f, None, Vec2<float> { 2.000001f, -4.0f });

	// Collision when already touching and moving toward each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 0.0f, -2.0f }, movingUp, 0.0f, Bottom, Vec2<float> { 0.0f, -2.0f });

	// No collision when already touching and moving away from each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 0.0f, -2.0f }, movingDown, 0.0f, None, Vec2<float> { 0.0f, -2.0f });

	/* Λ	Top Side collisions
	 * |
	 * B
	 * |
	 * V
	 *
	 * A
	 */
	// collision on y-axis
	RunCheckStaticAndMovingRectCollisionTests(blockAboveOrigin, movingDown, 1, Top, Vec2<float> { 0.0f, 2.0f });

	printf("\n// No collision on y-axis when moving too slow to collide before collision time boundary\n");
	RunCheckStaticAndMovingRectCollisionTests(blockAboveOrigin, Vec2<float> { 0.0f, -0.19999999f }, 0.0f, None, Vec2<float> { 0.0f, 4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckStaticAndMovingRectCollisionTests(blockAboveOrigin, Vec2<float> { 0.0f, -0.20000002f }, 9.99999905f, Top, Vec2<float> { 0.0f, 2.0f });

	// No collision on y axis when moving away from each other
	RunCheckStaticAndMovingRectCollisionTests(blockAboveOrigin, movingUp, 0.0f, None, Vec2<float> { 0.0f, 4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 2.0f, 4.0f }, movingDown, 1, Top, Vec2<float> { 2.0f, 2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 2.000001f, 4.0f }, movingDown, 0.0f, None, Vec2<float> { 2.000001f, 4.0f });

	// Collision when already touching and moving toward each other
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 0.0f, 2.0f }, movingDown, 0.0f, Top, Vec2<float> { 0.0f, 2.0f });

	printf("\n// Collision when already touching and moving toward each other and zero min collision time\n");
	RunCheckStaticAndMovingRectCollisionTestsWithTime(Vec2<float> { 0.0f, 2.0f }, movingDown, 0.0f, Top, Vec2<float> { 0.0f, 2.0f }, 0.0f);

	printf("\n// No collision when already touching and moving away from each other\n");
	RunCheckStaticAndMovingRectCollisionTests(Vec2<float> { 0.0f, 2.0f }, movingUp, 0.0f, None, Vec2<float> { 0.0f, 2.0f });

	RunCheckCollisionBetweenMovingRectTests();

	/*
	* CheckBlockAndTopsideOfWallCollision
	*
	*/

	/* B
	 * |
	 * V
	 *---
	 */
	RunCheckRectAndXLineCollisionTest("1", bottomWall, Vec2<float> { 10.0f, 4.0f }, movingDown, 1.5f, Bottom, Vec2<float> { 10.0f, 1.0f });

	/* B
	 *---
	 * |
	 * V
	 */
	RunCheckRectAndXLineCollisionTest("2", bottomWall, Vec2<float> { -10.0f, 1.0f }, movingDown, 0.0f, Bottom, Vec2<float> { -10.0f, 1.0f });

	/*---
	 * B
	 * |
	 * V
	 */
	RunCheckRectAndXLineCollisionTest("3", bottomWall, Vec2<float> { 10.0f, -1.0f }, movingDown, 0.0f, Overlap, Vec2<float> { 10.0f, -1.0f });

	/* ^
	 * |
	 * B
	 *---
	 */
	RunCheckRectAndXLineCollisionTest("4", bottomWall, Vec2<float> { 0.0f, 1.0f }, movingUp, 0.0f, None, Vec2<float> { 0.0f, 1.0f });

	/* B-->
	 *------
	 */
	RunCheckRectAndXLineCollisionTest("5", bottomWall, Vec2<float> { 0.0f, 1.0f }, movingRight, 0.0f, None, Vec2<float> { 0.0f, 1.0f });

	/* <--B
	 *------
	 */
	RunCheckRectAndXLineCollisionTest("6", bottomWall, Vec2<float> { 0.0f, 1.0f }, movingLeft, 0.0f, None, Vec2<float> { 0.0f, 1.0f });


	/*
	* RunCheckRectAndXLineCollisionFromNegativeYTest
	*
	*/

	/*---
	 * Λ
	 * |
	 * B
	 */
	RunCheckRectAndXLineCollisionTest("7", topWall, Vec2<float> { 10.0f, -4.0f }, movingUp, 1.5f, Top, Vec2<float> { 10.0f, -1.0f });

	/* Λ
	 * |
	 *---
	 * B
	 */
	RunCheckRectAndXLineCollisionTest("8", topWall, Vec2<float> { -10.0f, -1.0f }, movingUp, 0.0f, Top, Vec2<float> { -10.0f, -1.0f });

	/* Λ
	 * |
	 * B
	 *---
	 */
	// RunCheckRectAndXLineCollisionTest("9", topWall, Vec2<float> { 10.0f, 1.0f }, movingUp, 0.0f, Overlap, Vec2<float> { -10.0f, 1.0f });

	/*---
	 * B
	 * |
	 * V
	 */
	RunCheckRectAndXLineCollisionTest("10", topWall, Vec2<float> { 0.0f, -1.0f }, movingDown, 0.0f, None, Vec2<float> { 0.0f, -1.0f });

	/*------
	 *B-->
	 */
	RunCheckRectAndXLineCollisionTest("11", topWall, Vec2<float> { 0.0f, -1.0f }, movingRight, 0.0f, None, Vec2<float> { 0.0f, -1.0f });

	/*------
	 * <--B
	 */
	RunCheckRectAndXLineCollisionTest("12", topWall, Vec2<float> { 0.0f, -1.0f }, movingLeft, 0.0f, None, Vec2<float> { 0.0f, -1.0f });
}
