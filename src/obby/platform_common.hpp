#define MAX_FLOAT 999999.9f

struct BlockCollision
{
	bool any = false;
	bool isCheckpoint = false;
	bool isKillbrick = false;
	float time = MAX_FLOAT;
	tl::Vec2<float> position;
};

struct BlockCollisionResult
{
	BlockCollision north;
	BlockCollision south;
	BlockCollision east;
	BlockCollision west;
};

BlockCollisionResult GetBlockCollisionResult(
	Block* blocks,
	int blockCount,
	const tl::Rect<float>& playerState,
	float dt
);

void RenderBlocksAndPlayer(
	const tl::RenderBuffer& renderBuffer,
	const tl::Sprite& sprite,
	const GameState& state,
	uint32_t playerColor
);
