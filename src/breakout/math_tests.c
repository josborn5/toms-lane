#include <assert.h>
#include "math.c"

static void RunGetThetaForBallPlayerCollisionTest(float playerPosX, float ballPosX, float playerHalfSizeX, float expectedResult)
{
	float result = GetThetaForBallPlayerCollision(playerPosX, ballPosX, playerHalfSizeX);
	assert(result == expectedResult);
}

static void RunGameTests()
{
	RunGetThetaForBallPlayerCollisionTest(20.0f, 15.0f, 5.0f, -1.0f);

	RunGetThetaForBallPlayerCollisionTest(20.0f, 20.0f, 5.0f, 0.0f);

	RunGetThetaForBallPlayerCollisionTest(20.0f, 25.0f, 5.0f, 1.0f);
}
