#include "./game.hpp"
#include "../platform/toms-lane-platform.hpp"
#include "./platform_common.cpp"

const float BLOCK_WIDTH = 60.0f;
const float BLOCK_HEIGHT = 30.0f;

char *levels[10] = {
"\
0 0 0 0 0 0 0 0 0 0 0 0 0 0",

"\
     00\n\
    0000\n\
  00000000\n\
CCCCCCCCCCCC",

"\
000000000000\n\
  00C00C00\n\
    0000\n\
     MM"
};

static void PopulateBlocksForLevel(int level, Block* block, int blockArraySize, tl::Vec2<float> blockArea, tl::Vec2<float> blockAreaPosition)
{
	// clear out any remaining blocks in the block array
	Block* firstBlock = block;
	tl::Vec2<float> originVector = tl::Vec2<float> { 0.0f, 0.0f };
	for (int i = 0; i < blockArraySize; i += 1)
	{
		firstBlock->exists = 0;
		firstBlock->halfSize = originVector;
		firstBlock->position = originVector;
		firstBlock->color = 0;
		firstBlock++;
	}

	// Temporary hack - force level to be between 1 and 3
	if (level > 3)
	{
		level = 1;
	}

	char* blockLayoutForLevel = levels[level - 1];

	tl::Vec2<float> blockHalfSize;
	blockHalfSize.x = 0.5f * BLOCK_WIDTH;
	blockHalfSize.y = 0.5f * BLOCK_HEIGHT;

	int blockCount = 0;
	tl::Vec2<float> blockPosition = blockAreaPosition;
	blockPosition.x += blockHalfSize.x;
	blockPosition.y -= blockHalfSize.y;
	float originalX = blockPosition.x;
	while (*blockLayoutForLevel)
	{
		if (*blockLayoutForLevel == '\n')
		{
			blockPosition.x = originalX;
			blockPosition.y -= BLOCK_HEIGHT;
		}
		else
		{
			if (*blockLayoutForLevel != ' ')
			{
				block->exists = 1;
				block->halfSize = blockHalfSize;
				block->position = blockPosition;
				block->color = MakeColorFromGrey((uint8_t)(blockCount * 20));
				blockCount += 1;
			}

			blockPosition.x += BLOCK_WIDTH;
		}

		block++;
		blockLayoutForLevel++;
	}
}
