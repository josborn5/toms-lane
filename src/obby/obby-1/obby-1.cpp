#include "../../tl-application.hpp"
#include "../../tl-library.hpp"
#include "./game.hpp"

#include "./levels.cpp"
#include "./update-gamestate.cpp"
#include "./render-gamestate.cpp"

tl::GameMemory appMemory;

int Initialize(const tl::GameMemory& gameMemory)
{
	LoadSprites(gameMemory);
	gamestate.blockPixelHalfSize = 2.0f;
	gamestate.player.pixelHalfSize = 2.0f;

	return 0;
}

int UpdateAndRender(const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	GameState state = GetNewState(input, dt);
	RenderGameState(renderBuffer, state);
	return 0;
}

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return UpdateAndRender(input, renderBuffer, dt);
}

int tl::main(char* commandLine)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Obby-1";
	settings.width = 1280;
	settings.height = 720;

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::InitializeMemory(
		2,
		1,
		appMemory
	);

	Initialize(appMemory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}

