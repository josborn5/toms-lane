#include <math.h>
#include "math.cpp"
#include "levels.cpp"
#include "sound.cpp"


static const Boundary topBoundary = { Top, 720, -1.0f };
static const Boundary bottomBoundary = { Bottom, 0, 1.0f };
static const Boundary leftBoundary = { Left, 0, 1.0f };
static const Boundary rightBoundary = { Right, 1280, -1.0f };

static const int blockCapacity = 64;
static tl::rect_node blockTreeStorage[blockCapacity];
static Block blocks[blockCapacity];

static const int ballCapacity = 3;
static Ball balls[ballCapacity];

static GameState gamestate = {0};

struct WallCollision
{
	Boundary wall;
	tl::CollisionResult result;
};

static void ResetBalls()
{
	const float minimumBallSpeed = 400.0f;

	gamestate.balls.clear();
	for (int i = 0; i < ballCapacity; i += 1)
	{
		Ball newBall;
		newBall.exists = (i == 0);
		newBall.exists = (i == 0);
		newBall.velocity.y = minimumBallSpeed;
		newBall.velocity.x = minimumBallSpeed;
		newBall.halfSize = { 10.0f, 10.0f };
		newBall.position.y = 100 + balls[i].halfSize.y;
		newBall.position.x = 100 + balls[i].halfSize.x;
		gamestate.balls.append(newBall);
	}
}

static void StartNextLevel()
{
	gamestate.level += 1;
	gamestate.mode = ReadyToStart;

	gamestate.isCometActive = false;

	ResetBalls();

	tl::Rect<float> totalBlockAreaFootprint;
	totalBlockAreaFootprint.position = { 300.0f, 600.0f };
	totalBlockAreaFootprint.halfSize = { 400.0f, 100.0f };
	PopulateBlocksForLevel(
		gamestate.level,
		gamestate.blocks,
		totalBlockAreaFootprint,
		gamestate.blockTree
	);
}

void InitializeGameState()
{
	gamestate.mode = ReadyToStart;
	float worldHalfX = 0.5f * (float)rightBoundary.position;
	float worldHalfY = 0.5f * (float)topBoundary.position;
	gamestate.world.halfSize.x = worldHalfX;
	gamestate.world.halfSize.y = worldHalfY;
	gamestate.world.position.x = worldHalfX;
	gamestate.world.position.y = worldHalfY;

	gamestate.blockTree.descendents = tl::array<tl::rect_node>(&blockTreeStorage[0], blockCapacity);
	gamestate.blockTree.root.footprint = gamestate.world;

	gamestate.player.halfSize.x = 100.0f;
	gamestate.player.halfSize.y = 10.0f;

	gamestate.player.position.x = (float)leftBoundary.position;
	gamestate.player.position.y = 200;
	gamestate.player.velocity = tl::Vec2<float> { 0.0f, 0.0f };

	gamestate.balls.initialize(&balls[0], ballCapacity);
	gamestate.blocks.initialize(&blocks[0], blockCapacity);

	gamestate.score = 0;
	gamestate.lives = 3;
	gamestate.level = 0;
}

static float ClampFloat(float min, float val, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static WallCollision CheckWallCollision(const Ball &ball, float minimumTime)
{
	float finalCollisionTime = minimumTime;
	WallCollision wallCollision;

	Boundary horizontalWalls[2] = { topBoundary, bottomBoundary };
	for (int i = 0; i < 2; i += 1)
	{
		Boundary wall = horizontalWalls[i];
		tl::CollisionResult wallCollisionResult = tl::CheckRectAndXLineCollision(
			wall.position,
			wall.direction,
			ball,
			finalCollisionTime
		);
		if (wallCollisionResult.collisions[0].side != tl::None)
		{
			wallCollision.result = wallCollisionResult;
			wallCollision.wall = wall;
			finalCollisionTime = wallCollisionResult.time;
		}
	}

	Boundary verticalWalls[2] = { leftBoundary, rightBoundary };
	for (int i = 0; i < 2; i += 1)
	{
		Boundary wall = verticalWalls[i];
		tl::CollisionResult wallCollisionResult = tl::CheckRectAndYLineCollision(
			wall.position,
			wall.direction,
			ball,
			finalCollisionTime
		);
		if (wallCollisionResult.collisions[0].side != tl::None)
		{
			wallCollision.result = wallCollisionResult;
			wallCollision.wall = wall;
			finalCollisionTime = wallCollisionResult.time;
		}
	}

	return wallCollision;
}

static bool isPaused = false;

static void UpdatePlayerStateFromInput(const tl::Input& input, float dt)
{
	tl::Rect<float> newPlayerState;
	newPlayerState.halfSize = gamestate.player.halfSize;
	newPlayerState.position.x = ClampFloat((float)leftBoundary.position, (float)input.mouse.x, (float)rightBoundary.position);
	newPlayerState.position.y = gamestate.player.position.y;
	newPlayerState.velocity.x = (newPlayerState.position.x - gamestate.player.position.x) / dt;
	newPlayerState.velocity.y = 0.0f;
	gamestate.player = newPlayerState;
}

static void UpdateBallAndBlockState(float dt)
{
	for (int i = 0; i < ballCapacity; i += 1)
	{
		if (!balls[i].exists) continue;

		float minCollisionTime = dt;
		float t1 = dt;
		float t0 = 0.0f;
		int collisionCheckCount = 0;
		int maxCollisionCheckCount = 4;
		bool checkCollision = true;

		while(checkCollision && collisionCheckCount < maxCollisionCheckCount)
		{
			// Check for collision between any boundary of the world
			WallCollision ballWallCollision = CheckWallCollision(balls[i], minCollisionTime);
			if (ballWallCollision.result.collisions[0].side != tl::None)
			{
				minCollisionTime = ballWallCollision.result.time;
				checkCollision = true;
			}

			Block *block = nullptr;

			tl::CollisionResult blockBallCollisionResult;
			tl::CollisionResult hitBlockResult;
			hitBlockResult.collisions[1].side = tl::None;

			// query the block tree for collision candidates
			tl::Rect<float> ballFootprint;
			float ballDistanceCoveredX = balls[i].velocity.x * dt * 10;
			float ballDistanceCoveredY = balls[i].velocity.y * dt * 10;

			if (ballDistanceCoveredX < 0) ballDistanceCoveredX *= -1;
			if (ballDistanceCoveredY < 0) ballDistanceCoveredY *= -1;
			ballFootprint.position = balls[i].position;

			// Blocks are stored in the quad tree by their center position.
			// So the footprint to check needs to be at least the halfSize
			// of the block.
			Block firstBlock = gamestate.blocks.get(0);
			ballFootprint.halfSize = { 
				balls[i].halfSize.x + ballDistanceCoveredX + firstBlock.halfSize.x,
				balls[i].halfSize.y + ballDistanceCoveredY + firstBlock.halfSize.y
			 };

			tl::rect_node_value candidateStorage[blockCapacity];
			tl::array<tl::rect_node_value> candidates = tl::array<tl::rect_node_value>(
				&candidateStorage[0],
				blockCapacity
			);

			for (int x = 0; x < gamestate.blocks.length(); x += 1)
			{
				gamestate.blocks.access(x).color = gamestate.blocks.get(x).ogColor;
			}

			tl::rect_tree_query(gamestate.blockTree, ballFootprint, candidates);

			// check for collision between ball and blocks
			for (int j = 0; j < candidates.length(); j += 1)
			{
				Block* checkBlock = (Block*)(candidates.get(j).value);
				if (!checkBlock->exists) continue;
				checkBlock->color = 0xFF0000;
				blockBallCollisionResult = tl::CheckCollisionBetweenRects(*checkBlock, balls[i], minCollisionTime);
				if (blockBallCollisionResult.collisions[1].side != tl::None)
				{
					block = checkBlock;
					minCollisionTime = blockBallCollisionResult.time;
					hitBlockResult = blockBallCollisionResult;
					checkCollision = true;
				}
			}

			// Check for collision between ball and bat
			tl::Rect<float> player = gamestate.player;

			tl::CollisionResult ballBatCollisionResult = tl::CheckCollisionBetweenRects(player, balls[i], minCollisionTime);

			// Check collision results in the reverse order in which they are calculated
			// (player, block, wall)
			Ball newBallState;
			newBallState.velocity = balls[i].velocity;
			newBallState.halfSize = balls[i].halfSize;
			newBallState.exists = balls[i].exists;
			if (ballBatCollisionResult.collisions[1].side != tl::None)
			{
				playTone(262, 100);
				newBallState.position = ballBatCollisionResult.collisions[1].position;
				checkCollision = true;
				if (ballBatCollisionResult.collisions[1].side == tl::Top)
				{
					// Add a horizontal velocity to allow player to change ball direction
					float ballAngleFromNormal = GetThetaForBallPlayerCollision(player.position.x, balls[i].position.x, player.halfSize.x);
					float ballSpeed = tl::Length(balls[i].velocity);
					newBallState.velocity.x = (float)sinf(ballAngleFromNormal) * ballSpeed;
					newBallState.velocity.y = (float)cosf(ballAngleFromNormal) * ballSpeed;
				}
				else if (ballBatCollisionResult.collisions[1].side == tl::Left)
				{
					float ballVelocityX = (balls[i].velocity.x > 0) ? -balls[i].velocity.x : balls[i].velocity.x;
					newBallState.velocity.x = MinFloat(ballVelocityX, player.velocity.x);
				}
				else
				{
					float ballVelocityX = (balls[i].velocity.x < 0) ? -balls[i].velocity.x : balls[i].velocity.x;
					newBallState.velocity.x = MaxFloat(ballVelocityX, player.velocity.x);
				}

			}
			else if (hitBlockResult.collisions[1].side != tl::None)
			{
				playTone(440, 100);
				newBallState.position = hitBlockResult.collisions[1].position;
				checkCollision = true;

				if (!gamestate.isCometActive && (hitBlockResult.collisions[1].side == tl::Top || hitBlockResult.collisions[1].side == tl::Bottom))
				{
					newBallState.velocity.y = -newBallState.velocity.y;
				}
				else if (!gamestate.isCometActive && (hitBlockResult.collisions[1].side == tl::Left || hitBlockResult.collisions[1].side == tl::Right))
				{
					newBallState.velocity.x = -newBallState.velocity.x;
				}

				block->exists = false;
				gamestate.score += 10;

				// Check for powerup
				if (block->powerUp.type != Nothing)
				{
					block->powerUp.exists = true;
				}
			}
			else if (ballWallCollision.result.collisions[0].side != tl::None)
			{
				newBallState.position = ballWallCollision.result.collisions[0].position;
				checkCollision = true;

				if (ballWallCollision.wall.side == Left || ballWallCollision.wall.side == Right)
				{
					playTone(330, 200);
					newBallState.velocity.x = -newBallState.velocity.x;
				}
				else if (ballWallCollision.wall.side == Top)
				{
					playTone(330, 200);
					newBallState.velocity.y = -newBallState.velocity.y;
				}
				else if (ballWallCollision.wall.side == Bottom)
				{
					newBallState.exists = false;

					bool anyBallsLeft = false;
					for (int k = 0; k < ballCapacity && !anyBallsLeft; k += 1)
					{
						if (balls[k].exists)
						{
							anyBallsLeft = true;
						}
					}

					if (!anyBallsLeft)
					{
						gamestate.lives -= 1;

						if (gamestate.lives <= 0)
						{
							gamestate.mode = GameOver;
						}
						else
						{
							ResetBalls();
						}
						return;
					}
				}
			}
			else
			{
				newBallState.position = tl::AddVectors(balls[i].position, tl::MultiplyVectorByScalar(balls[i].velocity, t1 - t0));
				checkCollision = false;
			}

			balls[i] = newBallState;

			t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
			collisionCheckCount += 1;
		}

		// final bounds check to make sure ball doesn't leave the world
		balls[i].position.x = ClampFloat(
			leftBoundary.position + balls[i].halfSize.x,
			balls[i].position.x,
			rightBoundary.position - balls[i].halfSize.x
		);
		balls[i].position.y = ClampFloat(
			bottomBoundary.position + balls[i].halfSize.y,
			balls[i].position.y,
			topBoundary.position - balls[i].halfSize.y
		);
	}
}

GameState& UpdateGameState(const tl::Input& input, float dt)
{
	if (tl::IsReleased(input, tl::KEY_SPACE))
	{
		isPaused = !isPaused;
	}

	if (isPaused)
	{
		return gamestate;
	}

	if (tl::IsReleased(input, tl::KEY_R) || gamestate.mode == GameOver)
	{
		InitializeGameState();
		return gamestate;
	}

	if (gamestate.mode != Started)
	{
		if (tl::IsReleased(input, tl::KEY_S))
		{
			StartNextLevel();
			gamestate.mode = Started;
		}
		return gamestate;
	}

	UpdatePlayerStateFromInput(input, dt);

	UpdateBallAndBlockState(dt);
	if (gamestate.mode == GameOver)
	{
		return gamestate;
	}

	// Update power up gamestate
	bool allBlocksGoneResult = true;
	for (int i = 0; i < gamestate.blocks.length(); i += 1)
	{
		Block& block = gamestate.blocks.access(i);
		if (allBlocksGoneResult && block.exists)
		{
			allBlocksGoneResult = false;
		}
		if (!block.powerUp.exists) continue;

		block.powerUp.position = tl::AddVectors(block.powerUp.position, tl::MultiplyVectorByScalar(block.powerUp.velocity, dt));

		// Can get away with a super simple position check for the power up falling off screen here
		if (block.powerUp.position.y < bottomBoundary.position)
		{
			block.powerUp.exists = false;
		}
		else
		{
			tl::CollisionResult powerUpCollision = tl::CheckCollisionBetweenRects(gamestate.player, block.powerUp, dt);

			if (powerUpCollision.collisions[1].side != tl::None)
			{
				block.powerUp.exists = false;

				switch (block.powerUp.type)
				{
					case Comet:
						gamestate.isCometActive = true;
						break;
					case Multiball:
						// get the first ball that exists
						Ball* existingBall = balls;
						while (!existingBall->exists)
						{
							existingBall++;
						}

						for (int j = 0; j < ballCapacity; j += 1)
						{
							Ball* ball = &balls[j];
							if (ball->exists) continue;

							ball->exists = true;
							ball->position = existingBall->position;
							ball->velocity.y = existingBall->velocity.y;
							ball->velocity.x = existingBall->velocity.x + (j * 10.0f);
						}
						break;
				}
			}
		}
	}

	if (allBlocksGoneResult)
	{
		gamestate.mode = ReadyToStart;
	}

	return gamestate;
}
