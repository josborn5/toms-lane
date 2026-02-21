#ifndef OBBY_GAME_H
#define OBBY_GAME_H

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
	bool exists;
	bool isCheckpoint;
	BlockType type;
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
	tl::bitmap bitmap;
	tl::Sprite sprite;
	float pixelHalfSize;
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
	float levelTransitionTimer;
	const int blockCount = BLOCK_ARRAY_SIZE;
	int collision;
	tl::Rect<float> restartLevelButton = {0};
};

#endif
