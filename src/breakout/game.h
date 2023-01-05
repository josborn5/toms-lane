#ifndef GAME_H
#define GAME_H

#define BLOCK_ARRAY_SIZE 64
#define BALL_ARRAY_SIZE 3

#define ZERO_VECTOR tl::Vec2<float> { 0.0f, 0.0f }

#include "../platform/toms-lane-platform.hpp"

enum PowerUpType
{
	Nothing = 0,
	Multiball = 1,
	Comet = 2
};

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

struct PowerUp : tl::Rect<float>
{
	PowerUpType type;
	bool exists;		// flips to true when the block containing it is hit. i.e. it's a flag to render the power-up and include it in collision detection
	int color;
};

struct Ball : tl::Rect<float> {
	bool exists;
};

struct Block : tl::Rect<float> {
	int color;
	bool exists;
	PowerUp powerUp;
};

struct GameState {
	tl::Rect<float> player;
	Ball balls[BALL_ARRAY_SIZE];
	Block blocks[BLOCK_ARRAY_SIZE];
	bool isCometActive;
	int level;
	int score;
	int lives;
	Mode mode;
	float levelTransitionTimer;
};

#endif
