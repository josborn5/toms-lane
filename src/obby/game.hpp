#ifndef OBBY_GAME_H
#define OBBY_GAME_H

#include "../platform/toms-lane-platform.hpp"

#define BLOCK_ARRAY_SIZE 256
#define BALL_ARRAY_SIZE 9999
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

struct Block : tl::Rect<float> {
	int color;
	bool exists;
};

struct GameState
{
	tl::Rect<float> player;
	Block blocks[BLOCK_ARRAY_SIZE];
	int level;
	int score;
	int lives;
	Mode mode;
	float levelTransitionTimer;
	const int blockCount = BLOCK_ARRAY_SIZE;
};

#endif
