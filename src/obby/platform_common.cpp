#include "../platform/toms-lane-platform.hpp"
#include "./platform_common.hpp"

static bool IsReleased(const tl::Input &input, int button)
{
	bool isReleased = !input.buttons[button].isDown && input.buttons[button].wasDown;
	return isReleased;
}

static bool IsPressed(const tl::Input &input, int button)
{
	bool isReleased = input.buttons[button].isDown && !input.buttons[button].wasDown;
	return isReleased;
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

static void ClearBlock(Block* block)
{
	block->exists = false;
	block->color = 0;
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
		endOfContent = (*blockLayout == NULL);

		if (*blockLayout == '\n')
		{
			blockPosition.x = originalX;
			blockPosition.y -= blockHeight;
		}
		else
		{
			Block* block = &(gameState.blocks[i]);
			if (endOfContent)
			{
				ClearBlock(block);
			}
			else if (*blockLayout != ' ')
			{
				block->exists = true;
				bool isCheckpoint = (*blockLayout == 'c');
				block->color = (isCheckpoint) ? 0xAA5555 : 0xAAAAAA;
				block->isCheckpoint = isCheckpoint;
				switch (*blockLayout)
				{
					case 'c':
						block->type = Checkpoint;
						break;
					case 's':
						block->type = Spawn;
						break;
				}
			}
			else
			{
				ClearBlock(block);
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
	float time
) {
	if (toUpdate.time > time)
	{
		toUpdate.any = true;
		toUpdate.time = time;
		toUpdate.isCheckpoint = block.isCheckpoint;
	}
	else if (toUpdate.time == time)
	{
		toUpdate.isCheckpoint = (toUpdate.isCheckpoint) ? true : block.isCheckpoint;
	}
}

void UpdatePlayerMovement(
	const tl::Input& input,
	PlayerMovement& player
) {
	player.left = IsDown(input, tl::KEY_LEFT);
	player.right = IsDown(input, tl::KEY_RIGHT);

	bool spaceIsDown = IsPressed(input, tl::KEY_SPACE);
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
		blocks[j].color = (blocks[j].isCheckpoint) ? 0xAA5555 : 0xAAAAAA;
	}
	for (int j = 0; j < blockCount; j += 1)
	{
		Block block = blocks[j];
		if (!block.exists) continue;
		tl::CollisionResult collisionResult = tl::CheckCollisionBetweenRects(block, currentPlayerState, minCollisionTime);
		
		switch (collisionResult.collisions[1].side)
		{
			case tl::Top:
				minCollisionTime = collisionResult.time;
				currentPlayerState.position = collisionResult.collisions[1].position;
				blocks[j].color = 0xAA0000;

				UpdateBlockCollision(
					south,
					block,
					minCollisionTime
				);
				break;
		}
	}

	if (south.time <= minCollisionTime)
	{
		blockCollisionResult.south = south;
	}

	return blockCollisionResult;
}
