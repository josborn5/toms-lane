#include "./game.hpp"
#include "../../platform/lib/tl-library.hpp"

#include "./generated-assets.hpp"

GameState gamestate = {};

bool initialized = false;
bool isPaused = false;

float minPlayerX;
float maxPlayerX;
float minPlayerY;

struct BlockCollision
{
	bool any = false;
	bool isCheckpoint = false;
	bool isKillbrick = false;
	float time = 999999.9f;
	tl::Vec2<float> position;
};

struct BlockCollisionResult
{
	BlockCollision north;
	BlockCollision south;
	BlockCollision east;
	BlockCollision west;
};

static void UpdateBlockCollision(
	BlockCollision& toUpdate,
	const Block& block,
	const tl::Vec2<float>& position,
	float time
) {
	if (toUpdate.time > time)
	{
		toUpdate.any = true;
		toUpdate.time = time;
		toUpdate.isCheckpoint = (block.type == Checkpoint);
		toUpdate.isKillbrick = (block.type == Killbrick);
		toUpdate.position = position;
	}
	else if (toUpdate.time == time)
	{
		toUpdate.isCheckpoint = (toUpdate.isCheckpoint) ? true : (block.type == Checkpoint);
		toUpdate.isKillbrick = (toUpdate.isKillbrick) ? true : (block.type == Killbrick);
	}
}


static bool IsDown(const tl::Input &input, int button)
{
	bool isDown = input.buttons[button].isDown;
	return isDown;
}

static int ClampInt(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static float ClampFloat(float min, float val, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

void UpdatePlayerMovement(
	const tl::Input& input,
	PlayerMovement& player
) {
	player.left = IsDown(input, tl::KEY_LEFT);
	player.right = IsDown(input, tl::KEY_RIGHT);

	bool spaceIsDown = IsDown(input, tl::KEY_SPACE);
	if (spaceIsDown &&
		!player.inJump &&
		!player.wasInJump &&
		player.availableJumps > 0
	) {
		player.inJump = true;
		player.availableJumps -= 1;
	}
	else if (player.inJump && !player.wasInJump)
	{
		player.wasInJump = true;
	}
	else if (!spaceIsDown && player.inJump)
	{
		player.inJump = false;
		player.wasInJump = false;
	}
}

tl::Vec2<float> GetPlayerVelocity(
	float horizontalSpeed,
	float jumpSpeed,
	float gravity,
	const Player& player,
	float dt
) {
	tl::Vec2<float> newVelocity;
	newVelocity.x = (player.movement.left)
		? -horizontalSpeed
		: (player.movement.right)
			? horizontalSpeed
			: 0.0f;
	newVelocity.y = (player.movement.inJump && !player.movement.wasInJump)
		? jumpSpeed
		: player.velocity.y + (gravity / dt);

	return newVelocity;
}

BlockCollisionResult GetBlockCollisionResult(
	Block* blocks,
	tl::Rect<float>& currentPlayerState,
	int blockCount,
	float dt
) {
	BlockCollisionResult blockCollisionResult;

	BlockCollision north;
	BlockCollision south;
	BlockCollision east;
	BlockCollision west;

	float minCollisionTime = dt;

	for (int j = 0; j < blockCount; j += 1)
	{
		Block block = blocks[j];
		tl::CollisionResult collisionResult = tl::CheckCollisionBetweenRects(block, currentPlayerState, minCollisionTime);
		
		switch (collisionResult.collisions[1].side)
		{
			case tl::Top:
				minCollisionTime = collisionResult.time;

				UpdateBlockCollision(
					south,
					block,
					collisionResult.collisions[1].position,
					minCollisionTime
				);
				break;
			case tl::Right:
				minCollisionTime = collisionResult.time;

				UpdateBlockCollision(
					west,
					block,
					collisionResult.collisions[1].position,
					minCollisionTime
				);
				break;
			case tl::Left:
				minCollisionTime = collisionResult.time;

				UpdateBlockCollision(
					east,
					block,
					collisionResult.collisions[1].position,
					minCollisionTime
				);
				break;
		}
	}

	if (south.time <= minCollisionTime)
	{
		blockCollisionResult.south = south;
		currentPlayerState.position = south.position;
	}

	if (east.time <= minCollisionTime)
	{
		blockCollisionResult.east = east;
		currentPlayerState.position = east.position;
	}

	if (west.time <= minCollisionTime)
	{
		blockCollisionResult.west = west;
		currentPlayerState.position = west.position;
	}

	return blockCollisionResult;
}

tl::Vec2<float> GetPlayerStartPosition(Block* block, int arraySize)
{
	for (int i = 0; i < arraySize; i += 1)
	{
		Block checkBlock = block[i];
		if (checkBlock.type == Spawn)
		{
			tl::Vec2<float> playerStartPosition = checkBlock.position;
			playerStartPosition.y += (checkBlock.halfSize.y + 50.0f);
			return playerStartPosition;
		}
	}

	return tl::Vec2<float> { 0.0f, 0.0f };
}

static int load_bitmap_from_embed(unsigned char data[], unsigned int data_size, tl::bitmap& bitmap)
{
	tl::MemorySpace embed_memory;
	embed_memory.content = &data[0];
	embed_memory.sizeInBytes = data_size;

	return tl::bitmap_interface_initialize(
		bitmap,
		embed_memory
	);
}

int LoadSprites()
{
	load_bitmap_from_embed(
		brick_bmp,
		brick_bmp_len,
		gamestate.regularBlockBitmap
	);

	load_bitmap_from_embed(
		checkpoint_bmp,
		checkpoint_bmp_len,
		gamestate.checkpointBitmap
	);

	load_bitmap_from_embed(
		player_bmp,
		player_bmp_len,
		gamestate.player.bitmap
	);

	return 0;
}

static void ClearBlock_(Block& block)
{
	block.color = 0;
	block.isCheckpoint = false;
	block.type = Regular;
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

	float blockHeight = 50.0f;
	float blockWidth = 50.0f;
	tl::Vec2<float> blockHalfSize = {
		0.5f * blockWidth,
		0.5f * blockHeight
	};

	gamestate.world.halfSize.x = 0.5f * dimensions.x * blockWidth;
	gamestate.world.halfSize.y = 0.5f * dimensions.y * blockHeight;
	gamestate.world.position = gamestate.world.halfSize;

	minPlayerX = 0.0f + gamestate.player.halfSize.x;
	maxPlayerX = gamestate.world.x_max() - gamestate.player.halfSize.x;
	minPlayerY = 0.0f + gamestate.player.halfSize.y;

	bool endOfContent = false;
	float originalX = -blockHalfSize.x;
	tl::Vec2<float> blockPosition = {
		originalX,
		(float)(gamestate.world.y_max() - blockHalfSize.y)
	};
	for (int i = 0; i < gameState.blockCapacity; i += 1)
	{
		ClearBlock_(gameState.blocks[i]);
	}

	gamestate.blockCount = 0;
	for (int i = 0; i < gameState.blockCapacity && !endOfContent; i += 1)
	{
		if (*blockLayout == '\n')
		{
			blockPosition.x = originalX;
			blockPosition.y -= blockHeight;
		}
		else
		{
			Block* block = &(gameState.blocks[gamestate.blockCount]);
			blockPosition.x += blockWidth;
			if (*blockLayout != ' ')
			{
				gamestate.blockCount += 1;
				bool isCheckpoint = (*blockLayout == 'c');
				block->isCheckpoint = isCheckpoint;
				switch (*blockLayout)
				{
					case 'c':
						block->type = Checkpoint;
						block->color = 0x000000;
						block->bitmap = &gamestate.checkpointBitmap;
						break;
					case 's':
						block->type = Spawn;
						block->color = 0x0000AA;
						block->bitmap = nullptr;
						break;
					case 'K':
						block->type = Killbrick;
						block->color = 0xF79226;
						block->bitmap = nullptr;
						break;
					default:
						block->type = Regular;
						block->color = 0x000000;
						block->bitmap = &gamestate.regularBlockBitmap;
				}
				block->halfSize = blockHalfSize;
				block->position = blockPosition;
			}
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

	gamestate.camera.position = playerStartPosition;

	return returnVal;
}

static int InitializeGameState(GameState *state, const tl::Input &input)
{
	state->mode = ReadyToStart;

	state->player.halfSize.x = state->player.bitmap.dibs_header.width * state->player.pixelHalfSize;
	state->player.halfSize.y = state->player.bitmap.dibs_header.height * state->player.pixelHalfSize;

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
		if (input.buttons[tl::KEY_S].keyUp)
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

	tl::Vec2<float> newPlayerPosition;
	newPlayerPosition.x = currentPlayerState.position.x + (currentPlayerState.velocity.x * dt);
	newPlayerPosition.y = currentPlayerState.position.y + (currentPlayerState.velocity.y * dt);

	newPlayerPosition.x = ClampFloat(minPlayerX, newPlayerPosition.x, maxPlayerX);
	newPlayerPosition.y = ClampFloat(minPlayerY, newPlayerPosition.y, newPlayerPosition.y);

	if (newPlayerPosition.y <= minPlayerY)
	{
		state->mode = GameOver;
		state->lives -= 1;
	}

	state->player.position.x = newPlayerPosition.x;
	state->player.position.y = newPlayerPosition.y;
	state->player.velocity.x = currentPlayerState.velocity.x;
	state->player.velocity.y = currentPlayerState.velocity.y;

	state->camera.position.x = state->player.position.x;
}

GameState& GetNewState(const tl::Input& input, float dt)
{
	if (input.buttons[tl::KEY_R].keyUp)
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		InitializeGameState(&gamestate, input);
		return gamestate;
	}

	if (input.buttons[tl::KEY_H].keyUp)
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, input, dt);
	}
	return gamestate;
}
