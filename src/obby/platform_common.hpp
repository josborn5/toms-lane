#define MAX_FLOAT 999999.9f

struct BlockCollision
{
	bool any = false;
	bool isCheckpoint = false;
	float time = MAX_FLOAT;
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
