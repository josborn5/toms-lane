#include "./game.hpp"
#include "../../platform/src/tl-library.hpp"

GameState gamestate = {};

tl::Vec2<float> worldPosition = ZERO_VECTOR;
tl::Vec2<float> worldHalfSize = ZERO_VECTOR;

float minPlayerX;
float maxPlayerX;
float minPlayerY;
float maxPlayerY;

bool moveSwitch = false;
float moveCount = 0.0f;

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

int load_bitmap_from_embed(unsigned char data[], unsigned int data_size, tl::bitmap& bitmap)
{
	tl::MemorySpace embed_memory;
	embed_memory.content = &data[0];
	embed_memory.sizeInBytes = data_size;

	return tl::bitmap_interface_initialize(
		bitmap,
		embed_memory
	);
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

	state->player.movement.availableJumps = 2;
	state->player.movement.inJump = false;
	state->player.movement.wasInJump = false;
	state->player.sprite = islaSprite;

	if (state->lives <= 0)
	{
		state->score = 0;
		state->lives = 3;
		state->level = 1;
	}

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
		if (input.buttons[tl::KEY_S].keyUp)
		{
			state->mode = Started;
		}
		return;
	}

	// Translate input to player movement
	UpdatePlayerMovement(input, state->player.movement);

	// Calculate velocity to apply to current player state
	tl::Vec2<float> currentPlayerVelocity = GetPlayerVelocity(
		400.0f, // horizontalSpeed
		1600.0f, // jumpSpeed
		-3.0f, // gravity
		state->player,
		dt
	);
	tl::Rect<float> currentPlayerState;
	currentPlayerState.position = state->player.position;
	currentPlayerState.halfSize = state->player.halfSize;
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

			if (blockCollisionResult.south.isCheckpoint)
			{
				int nextlevel = state->level += 1;
				StartLevel(nextlevel, pixelRect);
				state->mode = StartingNextLevel;
				return;
			}
		}
		else
		{
			checkTime = -1.0f;
		}
	}

	tl::Vec2<float> newPlayerPosition;
	newPlayerPosition.x = currentPlayerState.position.x + (currentPlayerState.velocity.x * dt);
	newPlayerPosition.y = currentPlayerState.position.y + (currentPlayerState.velocity.y * dt);

	newPlayerPosition.x = ClampFloat(minPlayerX, newPlayerPosition.x, maxPlayerX);
	newPlayerPosition.y = ClampFloat(minPlayerY, newPlayerPosition.y, maxPlayerY);

	if (newPlayerPosition.y <= minPlayerY)
	{
		// restart current level
		StartLevel(state->level, pixelRect);
		state->mode = StartingNextLevel;
		return;
	}

	state->player.position.x = newPlayerPosition.x;
	state->player.position.y = newPlayerPosition.y;
	state->player.velocity.x = currentPlayerState.velocity.x;
	state->player.velocity.y = currentPlayerState.velocity.y;

	if (state->player.velocity.x == 0.0f)
	{
		state->player.sprite = islaSprite;
		moveCount = 0.0f;
	}
	else
	{
		if (moveCount > 0.15f)
		{
			moveCount = 0.0f;
			moveSwitch = !moveSwitch;
		}
		if (state->player.velocity.x > 0.0f)
		{
			state->player.sprite = (moveSwitch) ? islaSpriteMoveRight1 : islaSpriteMoveRight2;
		}
		else
		{
			state->player.sprite = (moveSwitch) ? islaSpriteMoveLeft1 : islaSpriteMoveLeft2;
		}
		
		moveCount += dt;
	}
}
