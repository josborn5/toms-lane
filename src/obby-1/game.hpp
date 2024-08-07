#ifndef OBBY1_GAME_H
#define OBBY1_GAME_H

#include "../tl-library.hpp"

#define BLOCK_ARRAY_SIZE 512
#define ZERO_VECTOR tl::Vec2<float> { 0.0f, 0.0f }

enum Side
{
	Top,
	Bottom,
	Left,
	Right
};

enum Mode
{
	ReadyToStart,
	Started,
	StartingNextLevel,
	GameOver
};

struct Boundary
{
	Side side;
	float position;
	float direction;
};

enum BlockType
{
	Regular,
	Checkpoint,
	Spawn,
	Killbrick
};

struct Block : tl::Rect<float> {
	int color;
	bool isCheckpoint;
	BlockType type;
	tl::SpriteC* sprite;
	tl::bitmap* bitmap;
};

struct PlayerMovement
{
	bool left;
	bool right;
	int availableJumps;
	bool inJump;
	bool wasInJump;
};

struct Player : tl::Rect<float>
{
	PlayerMovement movement;
	tl::Sprite sprite;
	tl::SpriteC spriteTest;
	tl::bitmap bitmap;
	float pixelHalfSize;
	tl::Vec2<float> prevPosition;
};

struct GameState
{
	Player player;
	Block blocks[BLOCK_ARRAY_SIZE];
	float blockPixelHalfSize;
	int level;
	int score;
	int lives;
	Mode mode;
	int blockCount = BLOCK_ARRAY_SIZE;
	const int blockCapacity = BLOCK_ARRAY_SIZE;
	tl::Rect<float> restartLevelButton = {0};
	tl::SpriteC regularBlockSprite;
	tl::bitmap regularBlockBitmap;
	tl::SpriteC checkpointBlockSprite;
	tl::bitmap checkpointBitmap;
	tl::Rect<float> world;
	tl::Rect<float> camera;
};

#endif
