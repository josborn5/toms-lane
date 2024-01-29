#include <windows.h>
#include "../win32/toms-lane-win32.hpp"
#include "../platform/toms-lane-platform.hpp"

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

tl::GameMemory garbage;

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return tl::UpdateAndRender(garbage, input, renderBuffer, dt);
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Driving";
	settings.width = 1280;
	settings.height = 720;

	int windowOpenResult = tl::OpenWindow(instance, settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::RenderBuffer garbagerender;
	tl::Initialize(garbage, garbagerender);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
