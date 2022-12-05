#include "../platform/toms-lane-platform.hpp"

static bool IsReleased(const tl::Input &input, int button)
{
	bool isReleased = !input.buttons[button].isDown && input.buttons[button].wasDown;
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
