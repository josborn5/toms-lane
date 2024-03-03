#include "../platform/toms-lane-platform.hpp"
#include "./platform_common.hpp"

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

static void ClearBlock(Block* block)
{
	block->exists = false;
	block->color = 0;
	block->isCheckpoint = false;
	block->type = Regular;
}

int PopulateBlocksForLevelString(
	char* blockLayout,
	GameState &gameState,
	const tl::Vec2<int> &pixelRect
) {
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
						block->color = 0xAA5555;
						break;
					case 's':
						block->type = Spawn;
						block->color = 0x0000AA;
						break;
					case 'K':
						block->type = Killbrick;
						block->color = 0xF79226;
						break;
					default:
						block->type = Regular;
						block->color = 0xAAAAAA;
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
		if (!block.exists) continue;
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

void RenderBlocks(
	const tl::RenderBuffer& renderBuffer,
	const GameState& state
) {
	// blocks
	for (int i = 0; i < state.blockCount; i += 1)
	{
		Block block = state.blocks[i];
		if (!block.exists) continue;

		tl::DrawRect(renderBuffer, block.color, block);
	}
}

int LoadSpriteFromFile(
	char* fileName,
	tl::SpriteC& spriteTarget,
	tl::MemorySpace& permanent,
	tl::MemorySpace transient // Purposefully don't pass as a reference so as not to modify the transient space - it can be overwritten after the function call
) {
	uint64_t fileSize = 0;
	tl::file_interface_size_get(fileName, fileSize);
	if (tl::file_interface_read(fileName, transient) != tl::Success)
	{
		return 1;
	}
	tl::MemorySpace tempFileContentMemory = tl::CarveMemorySpace(fileSize, transient);

	// Generate SpriteCs in perm space
	char* spriteCharArray = (char*)tempFileContentMemory.content;
	spriteTarget.content = (tl::Color*)permanent.content;
	tl::LoadSpriteC(spriteCharArray, transient, spriteTarget);
	tl::CarveMemorySpace(GetSpriteSpaceInBytes(spriteTarget), permanent);

	return 0;
}
