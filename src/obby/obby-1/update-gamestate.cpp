#include "../game.hpp"
#include "../../platform/toms-lane-platform.hpp"

GameState gamestate = {};

tl::Vec2<float> worldPosition = ZERO_VECTOR;
tl::Vec2<float> worldHalfSize = ZERO_VECTOR;

float minPlayerX;
float maxPlayerX;
float minPlayerY;
float maxPlayerY;

int PopulateBlocksForLevel(
	int level,
	GameState &gameState,
	const tl::Vec2<int> &pixelRect
) {
	// Temporary hack - force level to be between 1 and 3
	if (level > 6)
	{
		level = 1;
	}

	// level is a 1-based index. levels array is 0-based.
	char* blockLayout = levels[level - 1]; // levels is a global var set by levels.cpp .. TODO: make not global

	return PopulateBlocksForLevelString(blockLayout, gameState, pixelRect);
}

static int StartLevel(int newLevel, const tl::Vec2<int> &pixelRect)
{
	int returnVal = PopulateBlocksForLevel(
		newLevel,
		gamestate,
		pixelRect
	);

	tl::Vec2<float> playerStartPosition = GetPlayerStartPosition(gamestate.blocks, gamestate.blockCount);
	gamestate.player.position = playerStartPosition;

	return returnVal;
}

static int InitializeGameState(GameState *state, const tl::Vec2<int> &pixelRect, const tl::Input &input)
{
	state->mode = ReadyToStart;
	float worldHalfX = 0.5f * (float)pixelRect.x;
	float worldHalfY = 0.5f * (float)pixelRect.y;
	worldHalfSize.x = worldHalfX;
	worldHalfSize.y = worldHalfY;
	worldPosition.x = worldHalfX;
	worldPosition.y = worldHalfY;

	state->player.halfSize.x = state->player.spriteTest.width * state->player.pixelHalfSize;
	state->player.halfSize.y = state->player.spriteTest.height * state->player.pixelHalfSize;

	minPlayerX = 0.0f + state->player.halfSize.x;
	maxPlayerX = (float)pixelRect.x - state->player.halfSize.x;
	minPlayerY = 0.0f + state->player.halfSize.y;
	maxPlayerY = (float)pixelRect.y - state->player.halfSize.y;

	state->player.position.x = state->player.halfSize.x;
	state->player.position.y = 500;
	state->player.velocity.x = 0.0f;
	state->player.velocity.y = 0.0f;

	state->player.movement.availableJumps = 2;
	state->player.movement.inJump = false;
	state->player.movement.wasInJump = false;

	state->restartLevelButton.position = { 700.0f, 200.0f };
	state->restartLevelButton.halfSize = { 100.0f, 50.0f };

	state->score = 0;
	state->lives = 3;
	state->level = 1;
	return StartLevel(state->level, pixelRect);
}

static void UpdateGameState(
	GameState *state,
	tl::Vec2<int> pixelRect,
	const tl::Input &input,
	float dt,
	const tl::RenderBuffer &renderBuffer
)
{
	if (state->mode == ReadyToStart)
	{
		if (tl::IsReleased(input, tl::KEY_S))
		{
			state->mode = StartingNextLevel;
		}
		return;
	}
	else if (state->mode == StartingNextLevel)
	{
		state->mode = Started;
		return;
	}
	else if (state->mode == GameOver)
	{
		return;
	}

	// Translate input to player movement
	UpdatePlayerMovement(input, state->player.movement);

	// Calculate velocity to apply to current player state
	tl::Vec2<float> currentPlayerVelocity = GetPlayerVelocity(
		300.0f, // horizontalSpeed
		900.0f, // jumpSpeed
		-1.0f, // gravity
		state->player,
		dt
	);
	tl::Rect<float> currentPlayerState = CopyRect(state->player);
	currentPlayerState.velocity = currentPlayerVelocity;

	float checkTime = dt;
	while (checkTime > 0)
	{
		// Check for collisions based on the calculated velocity and current player position
		BlockCollisionResult blockCollisionResult = GetBlockCollisionResult(
			state->blocks,
			currentPlayerState,
			state->blockCount,
			checkTime
		);

		if (blockCollisionResult.south.any)
		{
			checkTime = dt - blockCollisionResult.south.time;
			currentPlayerState.velocity.y = 0.0f;
			state->player.movement.availableJumps = 2;

			if (blockCollisionResult.south.isKillbrick)
			{
				state->mode = GameOver;
				state->lives -= 1;
				return;
			}

			if (blockCollisionResult.south.isCheckpoint)
			{
				int nextlevel = state->level += 1;
				StartLevel(nextlevel, pixelRect);
				state->mode = StartingNextLevel;
				return;
			}
		}

		if (blockCollisionResult.east.any)
		{
			checkTime = dt - blockCollisionResult.east.time;
			currentPlayerState.velocity.x = 0.0f;
		}

		if (blockCollisionResult.west.any)
		{
			checkTime = dt - blockCollisionResult.west.time;
			currentPlayerState.velocity.x = 0.0f;
		}

		if (!blockCollisionResult.south.any && !blockCollisionResult.east.any && !blockCollisionResult.west.any)
		{
			checkTime = -1.0f;
		}
	}

	tl::Rect<float> newPlayerState = CopyRect(currentPlayerState);
	newPlayerState.position.x = currentPlayerState.position.x + (currentPlayerState.velocity.x * dt);
	newPlayerState.position.y = currentPlayerState.position.y + (currentPlayerState.velocity.y * dt);

	newPlayerState.position.x = ClampFloat(minPlayerX, newPlayerState.position.x, maxPlayerX);
	newPlayerState.position.y = ClampFloat(minPlayerY, newPlayerState.position.y, maxPlayerY);

	if (newPlayerState.position.y <= minPlayerY)
	{
		state->mode = GameOver;
		state->lives -= 1;
	}

	state->player.position.x = newPlayerState.position.x;
	state->player.position.y = newPlayerState.position.y;
	state->player.velocity.x = newPlayerState.velocity.x;
	state->player.velocity.y = newPlayerState.velocity.y;
}
