#ifndef GAME_H
#define GAME_H

#include "../../platform/lib/tl-library.hpp"


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
	bool exists = false;		// flips to true when the block containing it is hit. i.e. it's a flag to render the power-up and include it in collision detection
	int color = 0;
};

struct Ball : tl::Rect<float> {
	bool exists = false;
};

struct Block : tl::Rect<float> {
	int color = 0;
	int ogColor = 0;
	bool exists = false;
	PowerUp powerUp = { 0 };
};

struct GameState {
	tl::Rect<float> player = { 0 };
	tl::Rect<float> world = { 0 };
	tl::array<Ball> balls;
	tl::array<Block> blocks;
	bool isCometActive;
	int level = 0;
	int score = 0;
	int lives = 0;
	Mode mode;
	tl::rect_tree blockTree;
	tl::bitmap block_bitmap = { 0 };
};

#endif
