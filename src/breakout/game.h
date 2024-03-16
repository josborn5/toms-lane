#ifndef GAME_H
#define GAME_H

#define BLOCK_ARRAY_SIZE 64
#define BALL_ARRAY_SIZE 3

#define ZERO_VECTOR tl::Vec2<float> { 0.0f, 0.0f }

#include "../tl-library.hpp"
#include "../platform/quad-tree.hpp"

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
	GameOver,
	Paused
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

struct BlockQuadTree
{
	tl::QuadTreeRectNode<Block*> root;
	tl::QuadTreeRectNode<Block*> storage[BLOCK_ARRAY_SIZE];
	tl::array<tl::QuadTreeRectNode<Block*>> descendents;
};

struct GameState {
	tl::Rect<float> player;
	tl::Rect<float> world;
	Ball balls[BALL_ARRAY_SIZE];
	Block blocks[BLOCK_ARRAY_SIZE];
	bool isCometActive;
	int level;
	int score;
	int lives;
	Mode mode;
	BlockQuadTree blockTree;
	tl::Rect<float> checkArea[BALL_ARRAY_SIZE];
};

#endif
