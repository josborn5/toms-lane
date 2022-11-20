#include "./game.hpp"
#include "../platform/toms-lane-platform.hpp"
#include "./platform_common.cpp"

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
		endOfContent = (*blockLayout == NULL);

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