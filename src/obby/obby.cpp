#include "./game.hpp"
#include "../platform/toms-lane-platform.hpp"

#include "./levels.cpp"
#include "./obby-win32.cpp"

/*

* checkpoint
* player can land on surface
* levels

*/


const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

uint32_t playerColor = BAT_COLOR;

const int BLOCK_SCORE = 10;
const int NO_BLOCK_HIT_INDEX = -1;

const float BAT_WIDTH = 50.0f;
const float BAT_HEIGHT = 50.0f;

const int X_DIM_ORIGIN = 0;
const int X_DIM_BASE = 1280;
const int Y_DIM_ORIGIN = 0;
const int Y_DIM_BASE = 720;
const int STARTING_LIVES = 3;

const tl::Vec2<float> smallFontHalfSize = { 5.0f, 10.0f };
const tl::Vec2<float> titleFontHalfSize = { 15.0f, 30.0f };

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

static int StartLevel(int newLevel, const tl::Vec2<int> &pixelRect)
{
	return PopulateBlocksForLevel(
		newLevel,
		gamestate,
		pixelRect
	);
}

static int InitializeGameState(GameState *state, const tl::Vec2<int> &pixelRect, const tl::Input &input)
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
	return StartLevel(state->level, pixelRect);
}

static tl::Vec2<float> GetPlayerVelocity(
	const tl::Input &input,
	const tl::Vec2<float> &prevVelocity,
	float dt
)
{
	tl::Vec2<float> newVelocity = {0};

	const float horizontalDeltaPosition = 20.0f;
	if (IsDown(input, tl::KEY_LEFT))
	{
		newVelocity.x = -horizontalDeltaPosition / dt;
	}
	if (IsDown(input, tl::KEY_RIGHT))
	{
		newVelocity.x = horizontalDeltaPosition / dt;
	}

	if (IsDown(input, tl::KEY_SPACE))
	{
		newVelocity.y = 600.0f;
	}
	else
	{
		const float verticalAcceleration = -10.0f;
		newVelocity.y = prevVelocity.y + (verticalAcceleration / dt);
	}

	return newVelocity;
}

static void UpdateGameState(
	GameState *state,
	tl::Vec2<int> pixelRect,
	const tl::Input &input,
	float dt,
	const tl::RenderBuffer &renderBuffer
)
{
	if (state->mode == ReadyToStart)
	{
		if (IsReleased(input, tl::KEY_S))
		{
			state->mode = Started;
		}
		return;
	}

	// Calculate velocity to apply to current player state
	tl::Vec2<float> currentPlayerVelocity = GetPlayerVelocity(input, state->player.velocity, dt);

	// Check for collisions based on the calculated velocity and current player position
	float minCollisionTime = dt;

	// check for collision between player and blocks
	tl::CollisionSide collisionSide = tl::None;
	tl::Rect<float> currentPlayerState = {0};
	currentPlayerState.velocity = currentPlayerVelocity;
	currentPlayerState.position = state->player.position;
	for (int j = 0; j < BLOCK_ARRAY_SIZE; j += 1)
	{
		Block block = state->blocks[j];
		if (!block.exists) continue;
		tl::CollisionResult collisionResult = tl::CheckCollisionBetweenRects(block, currentPlayerState, minCollisionTime);
		if (collisionResult.collisions[1].side != tl::None)
		{
			minCollisionTime = collisionResult.time;
			collisionSide = collisionResult.collisions[1].side;
			currentPlayerState.position = collisionResult.collisions[1].position;
		}
	}

	if (collisionSide != tl::None)
	{
		playerColor = 0xFF0000;
	}
	else
	{
		playerColor = BAT_COLOR;
	}

	// Show info about z-position
	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 100.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };

	tl::DrawAlphabetCharacters(renderBuffer, "COL", charFoot, 0xFF0000);
	charFoot.position.y -= fontSize;
	tl::DrawNumber(renderBuffer, collisionSide, charFoot, 0xFF0000);

	tl::Rect<float> newPlayerState = {0};
	newPlayerState.position.x = currentPlayerState.position.x + (currentPlayerVelocity.x * dt);
	newPlayerState.position.y = currentPlayerState.position.y + (currentPlayerVelocity.y * dt);

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
	for (int i = 0; i < BLOCK_ARRAY_SIZE; i += 1)
	{
		Block block = state.blocks[i];
		if (!block.exists) continue;

		tl::DrawRect(renderBuffer, block.color, block);
	}
}

int tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{
	return 0;
}


int tl::UpdateAndRender(const GameMemory &gameMemory, const tl::Input &input, const RenderBuffer &renderBuffer, float dt)
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
		return InitializeGameState(&gamestate, pixelRect, input);
	}

	if (IsReleased(input, tl::KEY_H))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, pixelRect, input, dt, renderBuffer);
	}

	RenderGameState(renderBuffer, gamestate);

	return 0;
}

