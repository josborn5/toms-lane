#include "../game.hpp"
#include "../../platform/toms-lane-platform.hpp"

GameState gamestate = {0};

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
	if (level > 3)
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

	state->player.halfSize.x = 25.0f;
	state->player.halfSize.y = 25.0f;

	minPlayerX = 0.0f + state->player.halfSize.x;
	maxPlayerX = (float)pixelRect.x - state->player.halfSize.x;
	minPlayerY = 0.0f + state->player.halfSize.y;
	maxPlayerY = (float)pixelRect.y - state->player.halfSize.y;

	state->player.position.x = state->player.halfSize.x;
	state->player.position.y = 500;
	state->player.velocity.x = 0.0f;
	state->player.velocity.y = 0.0f;

	state->score = 0;
	state->lives = 3;
	state->level = 1;
	return StartLevel(state->level, pixelRect);
}

static tl::Vec2<float> GetPlayerVelocity(
	const tl::Input &input,
	const tl::Vec2<float> &prevVelocity,
	float dt
)
{
	tl::Vec2<float> newVelocity = {0};

	const float horizontalDeltaPosition = 5.0f;
	if (IsDown(input, tl::KEY_LEFT))
	{
		newVelocity.x = -horizontalDeltaPosition / dt;
	}
	if (IsDown(input, tl::KEY_RIGHT))
	{
		newVelocity.x = horizontalDeltaPosition / dt;
	}

	const float verticalAcceleration = -1.0f;
	newVelocity.y = prevVelocity.y + (verticalAcceleration / dt);

	if (IsPressed(input, tl::KEY_SPACE))
	{
		newVelocity.y = 600.0f;
	}

	return newVelocity;
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
		if (IsReleased(input, tl::KEY_S))
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

	// Calculate velocity to apply to current player state
	tl::Vec2<float> currentPlayerVelocity = GetPlayerVelocity(input, state->player.velocity, dt);

	tl::Rect<float> currentPlayerState = CopyRect(state->player);
	currentPlayerState.velocity = currentPlayerVelocity;

	// Check for collisions based on the calculated velocity and current player position
	BlockCollisionResult blockCollisionResult = GetBlockCollisionResult(
		state->blocks,
		currentPlayerState,
		state->blockCount,
		dt
	);

	if (blockCollisionResult.south.any)
	{
		currentPlayerState.velocity.y = 0.0f;

		if (blockCollisionResult.south.isCheckpoint)
		{
			int nextlevel = state->level += 1;
			StartLevel(nextlevel, pixelRect);
			state->mode = StartingNextLevel;
			return;
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
	}

	state->player.position.x = newPlayerState.position.x;
	state->player.position.y = newPlayerState.position.y;
	state->player.velocity.x = newPlayerState.velocity.x;
	state->player.velocity.y = newPlayerState.velocity.y;
}
