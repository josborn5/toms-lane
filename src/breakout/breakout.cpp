/*

TODO (in no particular order):

-- Visual effects for power ups

-- Render the player, ball and blocks as sprites instead of rects.

-- Measure processing times for each frame. Measure processing times for the game state update & rendering steps of each frame.

-- Once measuring is in place, see where optimization is needed.

-- Inter-level screen

-- More levels!

*/

#include "../tl-library.hpp"
#include "../tl-application.hpp"
#include "game.hpp"
#include "update.hpp"
#include "render.hpp"


int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;

	GameState state = UpdateGameState(input, dt);

	RenderGameState(renderBuffer, state);

	return 0;
}

int tl::main(char* commandLine)
{
	startSound();

	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Breakout";
	settings.width = 1280;
	settings.height = 720;

	tl::console_interface_open();


	int clientX, clientY;
	int windowOpenResult = tl::OpenWindow(settings, clientX, clientY);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	InitializeGameState(clientX, clientY);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
