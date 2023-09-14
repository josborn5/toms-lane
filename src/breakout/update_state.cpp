#define BLOCK_AREA tl::Vec2<float> { 800.0f, 200.0f }
#define BLOCK_AREA_POS tl::Vec2<float> { 300.0f, 600.0f }
#define BALL_HALF_SIZE tl::Vec2<float> { 10.0f, 10.0f }

const float MIN_BALL_SPEED = 400.0f;
const int BLOCK_SCORE = 10;

const int X_DIM_ORIGIN = 0;
const int X_DIM_BASE = 1280;
const int Y_DIM_ORIGIN = 0;
const int Y_DIM_BASE = 720;
const int STARTING_LIVES = 3;

const Boundary topBoundary = { Top, Y_DIM_BASE, -1.0f };
const Boundary bottomBoundary = { Bottom, Y_DIM_ORIGIN, 1.0f };
const Boundary leftBoundary = { Left, X_DIM_ORIGIN, 1.0f };
const Boundary rightBoundary = { Right, X_DIM_BASE, -1.0f };

tl::Vec2<int> GAME_RECT = { X_DIM_BASE, Y_DIM_BASE };

const float BAT_WIDTH = 100.0f;
const float BAT_HEIGHT = 10.0f;

float minPlayerX;
float maxPlayerX;

struct WallCollision
{
	Boundary wall;
	tl::CollisionResult result;
};

static void ResetBalls(GameState* state)
{
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		state->balls[i].exists = (i == 0);
		state->balls[i].velocity.y = MIN_BALL_SPEED;
		state->balls[i].velocity.x = MIN_BALL_SPEED;

		state->balls[i].position.y = 100 + state->balls[i].halfSize.y;
		state->balls[i].position.x = 100 + state->balls[i].halfSize.x;
	}
}

static void StartLevel(int newLevel, GameState* state)
{
	state->level = newLevel;
	state->mode = ReadyToStart;

	state->isCometActive = false;

	ResetBalls(state);

	PopulateBlocksForLevel(
		newLevel,
		&state->blocks[0],
		BLOCK_ARRAY_SIZE,
		BLOCK_AREA,
		BLOCK_AREA_POS,
		&state->blockTree
	);
}

static void InitializeGameState(GameState *state, const tl::Input& input)
{
	state->mode = ReadyToStart;
	float worldHalfX = 0.5f * (float)X_DIM_BASE;
	float worldHalfY = 0.5f * (float)Y_DIM_BASE;
	state->world.halfSize.x = worldHalfX;
	state->world.halfSize.y = worldHalfY;
	state->world.position.x = worldHalfX;
	state->world.position.y = worldHalfY;

	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		state->balls[i].halfSize = BALL_HALF_SIZE;
	}

	state->blockTree.descendents = tl::HeapArray<tl::QuadTreeNode<Block*>>(&state->blockTree.storage[0], BLOCK_ARRAY_SIZE);
	state->blockTree.root = tl::QuadTreeNode<Block*>(state->world, &state->blockTree.descendents);

	state->player.halfSize.x = BAT_WIDTH;
	state->player.halfSize.y = BAT_HEIGHT;

	minPlayerX = 0.0f;
	maxPlayerX = (float)X_DIM_BASE;

	state->player.position.x = (float)input.mouse.x;
	state->player.position.x = ClampFloat(minPlayerX, state->player.position.x, maxPlayerX);
	state->player.position.y = 200;
	state->player.velocity = tl::Vec2<float> { 0.0f, 0.0f };

	state->score = 0;
	state->lives = STARTING_LIVES;
	StartLevel(1, state);
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

static void UpdateGameState(GameState *state, const tl::Input& input, float dt)
{
	if (state->mode == ReadyToStart)
	{
		if (tl::IsReleased(input, tl::KEY_S))
		{
			state->mode = Started;
		}
		return;
	}

	// Update player state
	tl::Rect<float> newPlayerState;
	newPlayerState.halfSize = state->player.halfSize;
	newPlayerState.position.x = ClampFloat(minPlayerX, (float)input.mouse.x, maxPlayerX);
	newPlayerState.position.y = state->player.position.y;
	newPlayerState.velocity.x = (newPlayerState.position.x - state->player.position.x) / dt;
	newPlayerState.velocity.y = 0.0f;
	state->player = newPlayerState;

	// Update ball & block state
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		if (!state->balls[i].exists) continue;

		float minCollisionTime = dt;
		float t1 = dt;
		float t0 = 0.0f;
		int collisionCheckCount = 0;
		int maxCollisionCheckCount = 4;
		bool checkCollision = true;

		while(checkCollision && collisionCheckCount < maxCollisionCheckCount)
		{
			// Check for collision between any boundary of the world
			WallCollision ballWallCollision = CheckWallCollision(state->balls[i], minCollisionTime);
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
			float ballDistanceCoveredX = state->balls[i].velocity.x * dt;
			float ballDistanceCoveredY = state->balls[i].velocity.y * dt;
			ballFootprint.position = state->balls[i].position;
			ballFootprint.halfSize = { 
				ballDistanceCoveredX,
				ballDistanceCoveredY
			 };

			Block* candidateStorage[BLOCK_ARRAY_SIZE];
			tl::HeapArray<Block*> candidates = tl::HeapArray<Block*>(
				&candidateStorage[0],
				BLOCK_ARRAY_SIZE
			);
			state->blockTree.root.query(ballFootprint, candidates);

			// check for collision between ball and blocks
			for (int j = 0; j < candidates.length(); j += 1)
			{
				Block* checkBlock = candidates.get(j);
				if (!checkBlock->exists) continue;
				blockBallCollisionResult = tl::CheckCollisionBetweenRects(*checkBlock, state->balls[i], minCollisionTime);
				if (blockBallCollisionResult.collisions[1].side != tl::None)
				{
					block = checkBlock;
					minCollisionTime = blockBallCollisionResult.time;
					hitBlockResult = blockBallCollisionResult;
					checkCollision = true;
				}
			}

			// Check for collision between ball and bat
			tl::Rect<float> player = state->player;

			tl::CollisionResult ballBatCollisionResult = tl::CheckCollisionBetweenRects(player, state->balls[i], minCollisionTime);

			// Check collision results in the reverse order in which they are calculated
			// (player, block, wall)
			Ball newBallState;
			newBallState.velocity = state->balls[i].velocity;
			newBallState.halfSize = state->balls[i].halfSize;
			newBallState.exists = state->balls[i].exists;
			if (ballBatCollisionResult.collisions[1].side != tl::None)
			{
				newBallState.position = ballBatCollisionResult.collisions[1].position;
				checkCollision = true;
				if (ballBatCollisionResult.collisions[1].side == tl::Top)
				{
					// Add a horizontal velocity to allow player to change ball direction
					float ballAngleFromNormal = GetThetaForBallPlayerCollision(player.position.x, state->balls[i].position.x, player.halfSize.x);
					float ballSpeed = tl::Length(state->balls[i].velocity);
					newBallState.velocity.x = (float)sinf(ballAngleFromNormal) * ballSpeed;
					newBallState.velocity.y = (float)cosf(ballAngleFromNormal) * ballSpeed;
				}
				else if (ballBatCollisionResult.collisions[1].side == tl::Left)
				{
					float ballVelocityX = (state->balls[i].velocity.x > 0) ? -state->balls[i].velocity.x : state->balls[i].velocity.x;
					newBallState.velocity.x = MinFloat(ballVelocityX, player.velocity.x);
				}
				else
				{
					float ballVelocityX = (state->balls[i].velocity.x < 0) ? -state->balls[i].velocity.x : state->balls[i].velocity.x;
					newBallState.velocity.x = MaxFloat(ballVelocityX, player.velocity.x);
				}
			}
			else if (hitBlockResult.collisions[1].side != tl::None)
			{
				newBallState.position = hitBlockResult.collisions[1].position;
				checkCollision = true;

				if (!state->isCometActive && (hitBlockResult.collisions[1].side == tl::Top || hitBlockResult.collisions[1].side == tl::Bottom))
				{
					newBallState.velocity.y = -newBallState.velocity.y;
				}
				else if (!state->isCometActive && (hitBlockResult.collisions[1].side == tl::Left || hitBlockResult.collisions[1].side == tl::Right))
				{
					newBallState.velocity.x = -newBallState.velocity.x;
				}

				block->exists = false;
				state->score += BLOCK_SCORE;

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
					newBallState.velocity.x = -newBallState.velocity.x;
				}
				else if (ballWallCollision.wall.side == Top)
				{
					newBallState.velocity.y = -newBallState.velocity.y;
				}
				else if (ballWallCollision.wall.side == Bottom && state->mode != StartingNextLevel)
				{
					newBallState.exists = false;

					bool anyBallsLeft = false;
					for (int k = 0; k < BALL_ARRAY_SIZE && !anyBallsLeft; k += 1)
					{
						if (state->balls[k].exists)
						{
							anyBallsLeft = true;
						}
					}

					if (!anyBallsLeft)
					{
						state->lives -= 1;

						if (state->lives <= 0)
						{
							state->mode = GameOver; // TODO: GAMEOVER. For now, restart.
							return;
						}
						else
						{
							ResetBalls(state);
							return;
						}
					}
				}
			}
			else
			{
				newBallState.position = tl::AddVectors(state->balls[i].position, tl::MultiplyVectorByScalar(state->balls[i].velocity, t1 - t0));
				checkCollision = false;
			}

			state->balls[i] = newBallState;

			t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
			collisionCheckCount += 1;
		}

		// final bounds check to make sure ball doesn't leave the world
		state->balls[i].position.x = ClampFloat(0 + state->balls[i].halfSize.x, state->balls[i].position.x, X_DIM_BASE - state->balls[i].halfSize.x);
		state->balls[i].position.y = ClampFloat(0 + state->balls[i].halfSize.y, state->balls[i].position.y, Y_DIM_BASE - state->balls[i].halfSize.y);
	}

	// Update power up state
	bool allBlocksGoneResult = true;
	for (int i = 0; i < BLOCK_ARRAY_SIZE; i += 1)
	{
		Block *block = &state->blocks[i];
		if (allBlocksGoneResult && block->exists)
		{
			allBlocksGoneResult = false;
		}
		if (!block->powerUp.exists) continue;

		block->powerUp.position = tl::AddVectors(block->powerUp.position, tl::MultiplyVectorByScalar(block->powerUp.velocity, dt));

		// Can get away with a super simple position check for the power up falling off screen here
		if (block->powerUp.position.y < Y_DIM_ORIGIN)
		{
			block->powerUp.exists = false;
		}
		else
		{
			tl::CollisionResult powerUpCollision = tl::CheckCollisionBetweenRects(state->player, block->powerUp, dt);

			if (powerUpCollision.collisions[1].side != tl::None)
			{
				block->powerUp.exists = false;

				switch (block->powerUp.type)
				{
					case Comet:
						state->isCometActive = true;
						break;
					case Multiball:
						// get the first ball that exists
						Ball* existingBall = state->balls;
						while (!existingBall->exists)
						{
							existingBall++;
						}

						for (int j = 0; j < BALL_ARRAY_SIZE; j += 1)
						{
							Ball* ball = &state->balls[j];
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
		StartLevel(state->level + 1, state);
	}
}
