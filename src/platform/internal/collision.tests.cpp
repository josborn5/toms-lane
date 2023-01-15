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
#include "./collision.tests.static-and-moving-rects.cpp"

using namespace tl;

const float bottomWall = 1.0f;
const float topWall = -1.0f;



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
	RunStaticAndMovingRectCollisionTests();

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
