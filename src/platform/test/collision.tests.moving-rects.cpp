#include "../collision.hpp"

using namespace tl;

void CheckCollisionBetweenMovingRects(Vec2<float> aVelocity, Vec2<float> bPosition0, Vec2<float> bVelocity, float expectedCollisionTime, CollisionSide expectedCollisionResult, Vec2<float> expectedCollisionPosition)
{
	float collisionTime = originalCollisionTime;

	Rect<float> aRect;
	aRect.halfSize = oneByOneHalfSize;
	aRect.position = originPosition;
	aRect.velocity = aVelocity;

	Rect<float> bRect;
	bRect.halfSize = oneByOneHalfSize;
	bRect.position = bPosition0;
	bRect.velocity = bVelocity;

	CollisionResult result = CheckCollisionBetweenRects(aRect, bRect, collisionTime);

	printf("\n===== Moving & Moving =====\n");
	printf("actual collisionResult is %d\n", result.collisions[1].side);
	printf("expected collisionResult is %d\n", expectedCollisionResult);
	printf("actual collisionTime is %f\n", result.time);
	printf("expected collisionTime is %f\n", expectedCollisionTime);
	assert(result.collisions[1].side == expectedCollisionResult);
	assert(result.time == expectedCollisionTime);

	if (expectedCollisionResult != None)
	{
		printf("actual collisionPosition.x is %f\n", result.collisions[1].position.x);
		printf("expected collisionPosition.x is %f\n", expectedCollisionPosition.x);
		printf("actual collisionPosition.y is %f\n\n", result.collisions[1].position.y);
		printf("expected collisionPosition.y is %f\n", expectedCollisionPosition.y);
		assert(result.collisions[1].position.x == expectedCollisionPosition.x);
		assert(result.collisions[1].position.y == expectedCollisionPosition.y);
	}
}

void RunCheckCollisionBetweenMovingRectTests()
{
	std::cout << "Running CheckCollisionBetweenRects tests" << std::endl;

	// collision on x-axis: A--> <--B Right Hand Side collisions
	CheckCollisionBetweenMovingRects(movingRight, blockRightOfOrigin, movingLeft, 0.5f, Right, Vec2<float> { 3.0f, 0.0f });

	// collision on x-axis: <-A <--B Right Hand Side collisions
	CheckCollisionBetweenMovingRects(Vec2<float> { -1.0f, 0.0f }, blockRightOfOrigin, movingLeft, 2.0f, Right, Vec2<float> { 0.0f, 0.0f });

	// collision on x-axis: B--> <--A Left Hand Side collisions
	CheckCollisionBetweenMovingRects(movingLeft, blockLeftOfOrigin, movingRight, 0.5f, Left, Vec2<float> { -3.0f, 0.0f });

	// collision on x-axis: B--> A-> Left Hand Side collisions
	CheckCollisionBetweenMovingRects(Vec2<float> { 1.0f, 0.0f }, blockLeftOfOrigin, movingRight, 2.0f, Left, Vec2<float> { 0.0f, 0.0f });

	/* A	Bottom Side collisions
	 * |
	 * V
	 *
	 * Λ
	 * |
	 * B
	 */
	// collision on y-axis
	CheckCollisionBetweenMovingRects(movingDown, blockBelowOrigin, movingUp, 0.5f, Bottom, Vec2<float> { 0.0f, -3.0f });

	/* Λ	Bottom Side collisions
	 * |
	 * A
	 *
	 * Λ
	 * |
	 * |
	 * B
	 */
	// collision on y-axis
	CheckCollisionBetweenMovingRects(Vec2<float> { 0.0f, 1.0f }, blockBelowOrigin, movingUp, 2.0f, Bottom, Vec2<float> { 0.0f, 0.0f });

	/* B	Top Side collisions
	 * |
	 * V
	 *
	 * Λ
	 * |
	 * A
	 */
	// collision on y-axis
	CheckCollisionBetweenMovingRects(movingUp, blockAboveOrigin, movingDown, 0.5f, Top, Vec2<float> { 0.0f, 3.0f });

	/* B	Top Side collisions
	 * |
	 * |
	 * V
	 *
	 * A
	 * |
	 * V
	 */
	// collision on y-axis
	CheckCollisionBetweenMovingRects(Vec2<float> { 0.0f, -1.0f }, blockAboveOrigin, movingDown, 2.0f, Top, Vec2<float> { 0.0f, 0.0f });
}