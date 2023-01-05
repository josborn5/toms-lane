#include <stdint.h>

static float MaxFloat(float val1, float val2)
{
	if (val1 > val2)
	{
		return val1;
	}
	else
	{
		return val2;
	}
}

static float MinFloat(float val1, float val2)
{
	if (val1 < val2)
	{
		return val1;
	}
	else
	{
		return val2;
	}
}

static uint32_t MakeColorFromGrey(uint8_t grey)
{
	return (grey << 0) | (grey << 8) | (grey << 16);
}

static float GetThetaForBallPlayerCollision(float playerPositionX, float ballPositionX, float playerHalfSizeX)
{
	// work out where on the player the ball hit to determine the angle the ball moves after bouncing
	float thetaInRadians = 1.0f;
	float m = thetaInRadians / playerHalfSizeX;
	float b = (thetaInRadians * playerPositionX) / playerHalfSizeX;
	float horFactor = (m * ballPositionX) - b;
	return horFactor;
}
