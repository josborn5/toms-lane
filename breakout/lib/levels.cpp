#include "game.hpp"

const tl::Vec2<float> POWER_UP_VELOCITY = tl::Vec2<float> { 0.0f, -50.0f };
const tl::Vec2<float> POWER_UP_HALF_SIZE = tl::Vec2<float> { 5.0f, 5.0f };

const char* levels[10] = {
"\
000000000000\n\
000000000000\n\
MMMMMMMMMMMM",

"\
     00\n\
    0000\n\
  00000000\n\
CCCCCCCCCCCC",

"\
000000000000\n\
  00C00C00\n\
    0000\n\
     MM",

"\
MMMMMMMMMMMMM\n\
  CCCCCCCCC\n\
    MMMMMMMMMMMMMMM\n\
     CCCCCCCCCCC"
};

static void PopulateBlocksForLevel(
	int level,
	tl::array<Block>& blocks,
	tl::Rect<float>& blockAreaFootprint,
	tl::rect_tree& blockTree
)
{
	// clear out any remaining blocks in the block array
	tl::rect_tree_clear(blockTree);
	blocks.clear();

	// Temporary hack - force level to be between 1 and 4
	if (level > 4)
	{
		level = 1;
	}

	const char* blockLayoutForLevel = levels[level - 1];

	// count the newlines to work out how many rows of blocks we have
	const char* firstCharacter = blockLayoutForLevel; // keep a reference to the first character so we can iterate through it again

	int rowCount = 1;
	int columnCounter = 0;
	int columnCount = 0;
	while (*firstCharacter)
	{
		if (*firstCharacter == '\n')
		{
			rowCount += 1;
			if (columnCounter > columnCount)
			{
				columnCount = columnCounter;
			}
			columnCounter = 0;
		}
		else
		{
			columnCounter += 1;
		}
		firstCharacter++;
	}
	if (columnCounter > columnCount)
	{
		columnCount = columnCounter;
	}

	// Calculate the block size from the row & column counts
	float blockHeight = (2.0f * blockAreaFootprint.halfSize.y) / (float)rowCount;
	float blockWidth = (2.0f * blockAreaFootprint.halfSize.x) / (float)columnCount;
	tl::Vec2<float> blockHalfSize;
	blockHalfSize.x = 0.5f * blockWidth;
	blockHalfSize.y = 0.5f * blockHeight;

	tl::Vec2<float> blockPosition = blockAreaFootprint.position;
	blockPosition.x += blockHalfSize.x;
	blockPosition.y -= blockHalfSize.y;
	float originalX = blockPosition.x;
	while (*blockLayoutForLevel)
	{
		if (*blockLayoutForLevel == '\n')
		{
			blockPosition.x = originalX;
			blockPosition.y -= blockHeight;
		}
		else
		{
			if (*blockLayoutForLevel != ' ')
			{
				Block block;
				block.exists = true;
				block.halfSize = blockHalfSize;
				block.position = blockPosition;
				block.color = MakeColorFromGrey((uint8_t)blocks.length() * 20);
				block.ogColor = block.color;
				switch (*blockLayoutForLevel)
				{
					case 'M':
						block.powerUp.type = Multiball;
						block.powerUp.position = { block.position.x, block.position.y };
						block.powerUp.halfSize = POWER_UP_HALF_SIZE;
						block.powerUp.velocity = POWER_UP_VELOCITY;
						block.powerUp.exists = false;
						block.powerUp.color = 0xFF0000;
						break;
					case 'C':
						block.powerUp.type = Comet;
						block.powerUp.position = { block.position.x, block.position.y };
						block.powerUp.halfSize = POWER_UP_HALF_SIZE;
						block.powerUp.velocity = POWER_UP_VELOCITY;
						block.powerUp.exists = false;
						block.powerUp.color = 0xFFFF00;
						break;
					default:
						block.powerUp.type = Nothing;
						block.powerUp.position = { 0.0f, 0.0f };
						block.powerUp.halfSize = { 0.0f, 0.0f };
						block.powerUp.velocity = { 0.0f, 0.0f };
						block.powerUp.exists = false;
						block.powerUp.color = 0x000000;
						break;
				}

				blocks.append(block);
				tl::rect_node_value block_value;
				block_value.footprint = block;
				block_value.value = blocks.getTailPointer();
				tl::rect_tree_insert(blockTree, block_value);
			}

			blockPosition.x += blockWidth;
		}

		blockLayoutForLevel++;
	}
}
