#ifndef OBBY1_GAME_H
#define OBBY1_GAME_H

#include "../../platform/lib/tl-library.hpp"

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
	Side side = Top;
	float position = 0.0f;
	float direction = 0.0f;
};

enum BlockType
{
	Regular,
	Checkpoint,
	Spawn,
	Killbrick
};

struct Block : tl::Rect<float> {
	int color = 0;
	bool isCheckpoint = false;
	BlockType type = Regular;
	tl::bitmap* bitmap = nullptr;
};

struct PlayerMovement
{
	bool left = false;
	bool right = false;
	int availableJumps = 2;
	bool inJump = false;
	bool wasInJump = false;
};

struct Player : tl::Rect<float>
{
	PlayerMovement movement;
	tl::bitmap bitmap;
	float pixelHalfSize = 0.0f;
	tl::Vec2<float> prevPosition = { 0 };
};

struct GameState
{
	Player player;
	Block blocks[BLOCK_ARRAY_SIZE];
	float blockPixelHalfSize = 0.0f;
	int level = 0;
	int score = 0;
	int lives = 0;
	Mode mode = ReadyToStart;
	int blockCount = BLOCK_ARRAY_SIZE;
	const int blockCapacity = BLOCK_ARRAY_SIZE;
	tl::Rect<float> restartLevelButton = { 0 };
	tl::bitmap regularBlockBitmap;
	tl::bitmap checkpointBitmap;
	tl::Rect<float> world = { 0 };
	tl::Rect<float> camera = { 0 };
};

#endif
