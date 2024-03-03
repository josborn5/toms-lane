#include "../tl-application.hpp"
#include "../tl-library.hpp"

#include "driving.hpp"

State gameState = State();

int UpdateAndRender(const tl::RenderBuffer& renderBuffer)
{
	for (int i = 0; i < gameState.tileCount; i += 1)
	{
		Tile tile = gameState.tiles[i];
		tl::DrawRect(renderBuffer, tile.color, tile.rect);
	}
	
	return 0;
}

int Initialize()
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

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	return UpdateAndRender(renderBuffer);
}

int tl::main(char* commandLine)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Driving";
	settings.width = 1280;
	settings.height = 720;

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	Initialize();

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
