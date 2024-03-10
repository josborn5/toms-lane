#include "./game.hpp"
#include "../../tl-library.hpp"

GameState gamestate = {};

bool initialized = false;
bool isPaused = false;

float minPlayerX;
float maxPlayerX;
float minPlayerY;
float maxPlayerY;

int LoadSprites(const tl::GameMemory& gameMemory)
{
	tl::MemorySpace permanent = gameMemory.permanent;
	tl::MemorySpace transient = gameMemory.transient;

	// Read spritec files
	LoadSpriteFromFile(
		".\\brick.sprc",
		gamestate.regularBlockSprite,
		permanent,
		transient
	);

	LoadSpriteFromFile(
		"checkpoint.sprc",
		gamestate.checkpointBlockSprite,
		permanent,
		transient
	);

	LoadSpriteFromFile(
		"player.sprc",
		gamestate.player.spriteTest,
		permanent,
		transient
	);

	return 0;
}

int PopulateBlocksForLevelString_(
	char* blockLayout,
	GameState& gameState
) {
	tl::Vec2<int> dimensions = tl::GetContentDimensions(blockLayout);

	// Check the block array size is big enough for the content
	int contentCount = dimensions.x * dimensions.y;
	if (contentCount > gameState.blockCount)
	{
		return 1;
	}

	float blockHeight = (gamestate.world.position.y + gamestate.world.halfSize.y) / dimensions.y;
	float blockWidth = (gamestate.world.position.x + gamestate.world.halfSize.x) / dimensions.x;
	tl::Vec2<float> blockHalfSize = {
		0.5f * blockWidth,
		0.5f * blockHeight
	};

	bool endOfContent = false;
	float originalX = blockHalfSize.x;
	tl::Vec2<float> blockPosition = {
		originalX,
		(float)gamestate.world.position.y + gamestate.world.halfSize.y - blockHalfSize.y
	};
	for (int i = 0; i < gameState.blockCount; i += 1)
	{
		ClearBlock(&(gameState.blocks[i]));
	}

	for (int i = 0; i < gameState.blockCount && !endOfContent; i += 1)
	{
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
				block->isCheckpoint = isCheckpoint;
				switch (*blockLayout)
				{
					case 'c':
						block->type = Checkpoint;
						block->color = 0x000000;
						block->sprite = &gamestate.checkpointBlockSprite;
						break;
					case 's':
						block->type = Spawn;
						block->color = 0x0000AA;
						block->sprite = nullptr;
						break;
					case 'K':
						block->type = Killbrick;
						block->color = 0xF79226;
						block->sprite = nullptr;
						break;
					default:
						block->type = Regular;
						block->color = 0x000000;
						block->sprite = &gamestate.regularBlockSprite;
				}
			}

			block->halfSize = blockHalfSize;
			block->position = blockPosition;

			blockPosition.x += blockWidth;
		}

		blockLayout++;
		endOfContent = (*blockLayout == NULL);
	}

	return 0;
}

int PopulateBlocksForLevel(
	int level,
	GameState &gameState
) {
	// Temporary hack - force level to be between 1 and 3
	if (level > 6)
	{
		level = 1;
	}

	// level is a 1-based index. levels array is 0-based.
	char* blockLayout = levels[level - 1]; // levels is a global var set by levels.cpp .. TODO: make not global

	return PopulateBlocksForLevelString_(blockLayout, gameState);
}

static int StartLevel(int newLevel)
{
	int returnVal = PopulateBlocksForLevel(
		newLevel,
		gamestate
	);

	tl::Vec2<float> playerStartPosition = GetPlayerStartPosition(gamestate.blocks, gamestate.blockCount);
	gamestate.player.position = playerStartPosition;
	gamestate.player.prevPosition = playerStartPosition;

	return returnVal;
}

static int InitializeGameState(GameState *state, const tl::Input &input)
{
	state->world.halfSize = { 640.0f, 360.0f };
	state->world.position = state->world.halfSize;
	state->camera.halfSize = state->world.halfSize;
	state->camera.position = state->camera.halfSize;

	state->mode = ReadyToStart;

	state->player.halfSize.x = state->player.spriteTest.width * state->player.pixelHalfSize;
	state->player.halfSize.y = state->player.spriteTest.height * state->player.pixelHalfSize;

	minPlayerX = 0.0f + state->player.halfSize.x;
	maxPlayerX = state->world.position.x + state->world.halfSize.x - state->player.halfSize.x;
	minPlayerY = 0.0f + state->player.halfSize.y;
	maxPlayerY = state->world.position.y + state->world.halfSize.y - state->player.halfSize.y;

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
	return StartLevel(state->level);
}

static void UpdateGameState(
	GameState *state,
	const tl::Input &input,
	float dt
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

	state->player.prevPosition.x = state->player.position.x;
	state->player.prevPosition.y = state->player.position.y;

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
				StartLevel(nextlevel);
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

GameState& GetNewState(const tl::Input& input, float dt)
{
	if (tl::IsReleased(input, tl::KEY_R))
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		InitializeGameState(&gamestate, input);
		return gamestate;
	}

	if (tl::IsReleased(input, tl::KEY_H))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, input, dt);
	}
	return gamestate;
}
