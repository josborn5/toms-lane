#ifndef GAME_H
#define GAME_H

#include "../../platform/src/tl-library.hpp"


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
	ReadyToStartLevel,
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
	int ogColor;
	bool exists;
	PowerUp powerUp;
};

struct GameState {
	tl::Rect<float> player;
	tl::Rect<float> world;
	tl::array<Ball> balls;
	tl::array<Block> blocks;
	bool isCometActive;
	int level;
	int score;
	int lives;
	Mode mode;
	tl::rect_tree blockTree;
};

#endif
