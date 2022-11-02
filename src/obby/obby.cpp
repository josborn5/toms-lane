#include "./game.hpp"
#include "../platform/toms-lane-platform.hpp"

#include "./levels.cpp"
#include "./obby-win32.cpp"

#define BLOCK_AREA tl::Vec2<float> { 1000.0f, 200.0f }
#define BLOCK_AREA_POS tl::Vec2<float> { 100.0f, 300.0f }
#define BALL_HALF_SIZE tl::Vec2<float> { 10.0f, 10.0f }

/*

* checkpoint
* player can land on surface
* levels

*/


const float MIN_BALL_SPEED = 40.0f;
const float LEVEL_CHANGE_BALL_SPEED = 5.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

uint32_t playerColor = BAT_COLOR;

const int BLOCK_SCORE = 10;
const int NO_BLOCK_HIT_INDEX = -1;

const float FONT_SIZE = 20.0f;
const float BAT_WIDTH = 50.0f;
const float BAT_HEIGHT = 50.0f;

const int X_DIM_ORIGIN = 0;
const int X_DIM_BASE = 1280;
const int Y_DIM_ORIGIN = 0;
const int Y_DIM_BASE = 720;
const int STARTING_LIVES = 3;

const tl::Vec2<float> smallFontHalfSize = { 5.0f, 10.0f };
const tl::Vec2<float> titleFontHalfSize = { 15.0f, 30.0f };

tl::Vec2<int> GAME_RECT = { X_DIM_BASE, Y_DIM_BASE };

int rainbowColor = 0;
float minPlayerX;
float maxPlayerX;
float minPlayerY;
float maxPlayerY;

tl::Vec2<float> worldPosition = ZERO_VECTOR;
tl::Vec2<float> worldHalfSize = ZERO_VECTOR;

GameState gamestate = {0};

bool initialized = false;
bool isPaused = false;
bool allBlocksCleared = false;;

char debugStringBuffer[256];

static void StartLevel(int newLevel, const tl::Vec2<int> &pixelRect)
{
	allBlocksCleared = false;

	PopulateBlocksForLevel(
		newLevel,
		gamestate,
		pixelRect
	);
}

static void InitializeGameState(GameState *state, const tl::Vec2<int> &pixelRect, const tl::Input &input)
{
	state->mode = ReadyToStart;
	float worldHalfX = 0.5f * (float)X_DIM_BASE;
	float worldHalfY = 0.5f * (float)Y_DIM_BASE;
	worldHalfSize.x = worldHalfX;
	worldHalfSize.y = worldHalfY;
	worldPosition.x = worldHalfX;
	worldPosition.y = worldHalfY;

	state->player.halfSize.x = BAT_WIDTH;
	state->player.halfSize.y = BAT_HEIGHT;

	minPlayerX = 0.0f + state->player.halfSize.x;
	maxPlayerX = (float)X_DIM_BASE - state->player.halfSize.x;
	minPlayerY = 0.0f + state->player.halfSize.y;
	maxPlayerY = (float)Y_DIM_BASE - state->player.halfSize.y;

	state->player.position.x = 800;
	state->player.position.y = 500;
	state->player.velocity.x = 0.0f;
	state->player.velocity.y = 0.0f;

	state->score = 0;
	state->lives = STARTING_LIVES;
	state->level = 1;
	StartLevel(state->level, pixelRect);
}

static void UpdateGameState(GameState *state, tl::Vec2<int> pixelRect, const tl::Input &input, float dt)
{
	if (state->mode == ReadyToStart)
	{
		if (IsReleased(input, tl::KEY_S))
		{
			state->mode = Started;
		}
		return;
	}

	// Update player state
	tl::Rect<float> newPlayerState = {0};
	newPlayerState.position.x = state->player.position.x;
	newPlayerState.position.y = state->player.position.y;
	newPlayerState.velocity.x = state->player.velocity.x;
	newPlayerState.velocity.y = state->player.velocity.y;
	if (IsDown(input, tl::KEY_LEFT))
	{
		newPlayerState.position.x = state->player.position.x - 20;
	}
	if (IsDown(input, tl::KEY_RIGHT))
	{
		newPlayerState.position.x = state->player.position.x + 20;
	}
	if (IsDown(input, tl::KEY_SPACE))
	{
		newPlayerState.velocity.y = 300.0f;
	}


	float minCollisionTime = dt;

	// check for collision between player and blocks
	tl::CollisionSide collisionSide = tl::None;

	for (int j = 0; j < BLOCK_ARRAY_SIZE; j += 1)
	{
		Block block = state->blocks[j];
		if (!block.exists) continue;
		tl::CollisionResult collisionResult = tl::CheckCollisionBetweenRects(block, state->player, minCollisionTime);
		if (collisionResult.collisions[1].side != tl::None)
		{
			minCollisionTime = collisionResult.time;
			collisionSide = collisionResult.collisions[1].side;
			state->player.position = collisionResult.collisions[1].position;
		}
	}

	// Check for any horizontal collisions
	if (collisionSide == tl::Right || collisionSide == tl::Left)
	{
		newPlayerState.velocity.x = 0.0f;
		playerColor = 0xFF0000;
	}
	else
	{
		newPlayerState.velocity.x = (newPlayerState.position.x - state->player.position.x) / dt;
		playerColor = BAT_COLOR;
	}

	// Check for any vertical collisions
	if (collisionSide == tl::Top || collisionSide == tl::Overlap)
	{
		newPlayerState.velocity.y = 0.0f;
		playerColor = 0xFF0000;
	}
	else
	{
		newPlayerState.velocity.y = state->player.velocity.y - (10.0f * dt * state->player.velocity.y);
		playerColor = BAT_COLOR;
	}

	// newPlayerState.position.y = newPlayerState.position.y + (newPlayerState.velocity.y * dt);
	newPlayerState.position.y = state->player.position.y - 5.0f;
	newPlayerState.position.x = ClampFloat(minPlayerX, newPlayerState.position.x, maxPlayerX);
	newPlayerState.position.y = ClampFloat(minPlayerY, newPlayerState.position.y, maxPlayerY);

	state->player.position.x = newPlayerState.position.x;
	state->player.position.y = newPlayerState.position.y;
	state->player.velocity.x = newPlayerState.velocity.x;
	state->player.velocity.y = newPlayerState.velocity.y;
}

char* isla_avatar = "\
 0 0 0\n\
 00000\n\
  000\n\
   0\n\
0000000\n\
   0\n\
  000\n\
 0   0";
const int islaAvatarWidth = 7;
const int islaAvatarHeight = 8;

static void RenderGameState(const tl::RenderBuffer &renderBuffer, const GameState &state)
{
	if (state.mode == ReadyToStart)
	{
		tl::ClearScreen(renderBuffer, 0x050505);

		if (rainbowColor == (255 * 4))
		{
			rainbowColor = 0;
		}
		// Red inc, Green min, Blue dec
		// Red max, Green inc, Blue min
		// Red dec, Green max, Blue inc
		// Red min, Green dec, Blue max
		rainbowColor += 5;
		int rValue, gValue, bValue;
		if (rainbowColor < 255)
		{
			rValue = ClampInt(0, rainbowColor, 255);
			gValue = 0;
			bValue = ClampInt(0, -(rainbowColor - 255), 255);
		}
		else if (rainbowColor < (255 * 2))
		{
			rValue = 255;
			gValue = ClampInt(0, (255 * 2) - rainbowColor, 255);
			bValue = 0;
		}
		else if (rainbowColor < (255 * 3))
		{
			rValue = ClampInt(0, -(rainbowColor - (255 * 3)), 255);
			gValue = 255;
			bValue = ClampInt(0, (255 * 3) - rainbowColor, 255);
		}
		else
		{
			rValue = 0;
			gValue = ClampInt(0, -(rainbowColor - (255 * 4)), 255);
			bValue = 255;
		}
		uint32_t rainbowValue = (rValue << 16) | (gValue << 8) | (bValue << 0);
		tl::Rect<float> titleCharRect;
		titleCharRect.halfSize = titleFontHalfSize;
		titleCharRect.position = tl::Vec2<float> { 200.0f, 500.0f };
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"GIVE ME BOBUX",
			titleCharRect,
			rainbowValue
		);

		tl::Rect<float> smallCharRect;
		smallCharRect.halfSize = smallFontHalfSize;
		smallCharRect.position = tl::Vec2<float> { 100.0f, 100.0f };
		tl::DrawAlphabetCharacters(
			renderBuffer,
			"PRESS S TO START",
			smallCharRect,
			TEXT_COLOR
		);

		return;
	}

	// background
	tl::ClearScreen(renderBuffer, 0x000000);
	tl::Rect<float> worldRect;
	worldRect.position = worldPosition;
	worldRect.halfSize = worldHalfSize;
	tl::DrawRect(renderBuffer, BACKGROUND_COLOR, worldRect);

	// player
	tl::DrawRect(renderBuffer, playerColor, state.player);

	// blocks
	allBlocksCleared = true;
	for (int i = 0; i < BLOCK_ARRAY_SIZE; i += 1)
	{
		Block block = state.blocks[i];
		allBlocksCleared = false;
		if (!block.exists) continue;

		tl::DrawRect(renderBuffer, block.color, block);
	}
}

void tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{

}


void tl::UpdateAndRender(const GameMemory &gameMemory, const tl::Input &input, const RenderBuffer &renderBuffer, float dt)
{
	tl::Vec2<int> pixelRect;
	pixelRect.x = renderBuffer.width;
	pixelRect.y = renderBuffer.height;

	if (IsReleased(input, tl::KEY_R))
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		InitializeGameState(&gamestate, pixelRect, input);
		return;
	}

	if (IsReleased(input, tl::KEY_H))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, pixelRect, input, dt);
	}

	RenderGameState(renderBuffer, gamestate);
}

