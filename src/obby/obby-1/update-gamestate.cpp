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

	tl::Vec2<int> dimensions = tl::GetContentDimensions(blockLayout);

	// Check the block array size is big enough for the content
	int contentCount = dimensions.x * dimensions.y;
	if (contentCount > gameState.blockCount)
	{
		return 1;
	}

	float blockHeight = (float)pixelRect.y / dimensions.y;
	float blockWidth = (float)pixelRect.x / dimensions.x;
	tl::Vec2<float> blockHalfSize = {
		0.5f * blockWidth,
		0.5f * blockHeight
	};

	bool endOfContent = false;
	float originalX = blockHalfSize.x;
	tl::Vec2<float> blockPosition = {
		originalX,
		(float)pixelRect.y - blockHalfSize.y
	};
	for (int i = 0; i < gameState.blockCount; i += 1)
	{
		endOfContent = (*blockLayout == '\0');

		if (*blockLayout == '\n')
		{
			blockPosition.x = originalX;
			blockPosition.y -= blockHeight;
		}
		else
		{
			Block* block = &(gameState.blocks[i]);
			if (*blockLayout != ' ')
			{
				block->exists = true;
				bool isCheckpoint = (*blockLayout == 'c');
				block->color = (isCheckpoint) ? 0xAA5555 : 0xAAAAAA;
				block->isCheckpoint = isCheckpoint;
			}
			else
			{
				block->exists = false;
				block->color = 0;
			}

			block->halfSize = blockHalfSize;
			block->position = blockPosition;

			blockPosition.x += blockWidth;
		}

		if (!endOfContent)
		{
			blockLayout++;
		}
	}

	return 0;
}

static int StartLevel(int newLevel, const tl::Vec2<int> &pixelRect)
{
	return PopulateBlocksForLevel(
		newLevel,
		gamestate,
		pixelRect
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

	const float horizontalDeltaPosition = 10.0f;
	if (IsDown(input, tl::KEY_LEFT))
	{
		newVelocity.x = -horizontalDeltaPosition / dt;
	}
	if (IsDown(input, tl::KEY_RIGHT))
	{
		newVelocity.x = horizontalDeltaPosition / dt;
	}

	const float verticalAcceleration = -3.0f;
	newVelocity.y = prevVelocity.y + (verticalAcceleration / dt);

	if (IsDown(input, tl::KEY_SPACE))
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
			state->mode = Started;
		}
		return;
	}

	// Calculate velocity to apply to current player state
	tl::Vec2<float> currentPlayerVelocity = GetPlayerVelocity(input, state->player.velocity, dt);

	// Check for collisions based on the calculated velocity and current player position
	float minCollisionTime = dt;

	// check for collision between player and blocks
	tl::CollisionSide collisionSide = tl::None;
	tl::Rect<float> currentPlayerState = CopyRect(state->player);
	currentPlayerState.velocity = currentPlayerVelocity;

	bool isBlockCheckpoint = false;
	for (int j = 0; j < BLOCK_ARRAY_SIZE; j += 1)
	{
		Block block = state->blocks[j];
		if (!block.exists) continue;
		tl::CollisionResult collisionResult = tl::CheckCollisionBetweenRects(block, currentPlayerState, minCollisionTime);
		if (collisionResult.collisions[1].side != tl::None)
		{
			minCollisionTime = collisionResult.time;
			collisionSide = collisionResult.collisions[1].side;
			currentPlayerState.position = collisionResult.collisions[1].position;
			isBlockCheckpoint = block.isCheckpoint;
		}
	}

	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 500.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };
	tl::DrawAlphabetCharacters(renderBuffer, "COL", charFoot, 0x999999);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, collisionSide, charFoot, 0x999999);

	if (currentPlayerState.velocity.x <0)
	{
		int test=-8;
		test++;
	}

	if (collisionSide == tl::Top || collisionSide == tl::Bottom)
	{
		currentPlayerState.velocity.y = 0.0f;

		if (isBlockCheckpoint)
		{
			int nextlevel = state->level += 1;
			StartLevel(nextlevel, pixelRect);
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
