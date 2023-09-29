#include "../platform/toms-lane-platform.hpp"
#include "main_win32.cpp"

#include "driving.hpp"

State gameState = State();

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	for (int i = 0; i < gameState.tileCount; i += 1)
	{
		Tile tile = gameState.tiles[i];
		tl::DrawRect(renderBuffer, tile.color, tile.rect);
	}
	
	return 0;
}

int tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{
	float halfWidth = 10.0f;
	float tileSpacing = halfWidth * 2.0f;
	tl::Vec2<float> halfSize = { halfWidth, 1000.0f };
	tl::Rect<float> baseRect;
	baseRect.halfSize = halfSize;
	baseRect.position = { halfWidth, 0.0f };

	Tile leftGrass = Tile();
	leftGrass.rect = tl::CopyRect(baseRect);
	leftGrass.color = 0x00FF00;
	gameState.tiles[0] = leftGrass;

	Tile road = Tile();
	baseRect.position.x += tileSpacing;
	road.rect = tl::CopyRect(baseRect);
	road.color = 0x111111;
	gameState.tiles[1] = road;

	Tile rightGrass = Tile();
	baseRect.position.x += tileSpacing;
	rightGrass.rect = tl::CopyRect(baseRect);
	rightGrass.color = 0x00FF00;
	gameState.tiles[2] = rightGrass;

	return 0;
}

int tl::UpdateSound(const SoundBuffer& soundBuffer)
{
	return 0;
}
