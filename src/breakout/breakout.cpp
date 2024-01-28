/*

TODO (in no particular order):

-- Visual effects for power ups

-- Render the player, ball and blocks as sprites instead of rects.

-- Measure processing times for each frame. Measure processing times for the game state update & rendering steps of each frame.

-- Once measuring is in place, see where optimization is needed.

-- Inter-level screen

-- Sound! Check out the JavidX9 youtube videos for some ideas on how to implement this.

-- More levels!

*/

#include <windows.h>
#include "../win32/toms-lane-win32.hpp"
#include "../win32/win32-console.hpp"
#include "../platform/toms-lane-platform.hpp"
#include "game.h"
#include "sound.cpp"
#include "update_state.cpp"
#include "render.cpp"


int tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{
	InitializeGameState();
	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	GameState* state = UpdateGameState(input, dt);

	RenderGameState(renderBuffer, *state);

	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	startSound();

	tl::WindowSettings settings;
	settings.title = "Breakout";
	settings.width = 1280;
	settings.height = 720;
	settings.targetFPS = 60;

	tl::console_interface_open();

	int windowOpenResult = tl::OpenWindow(instance, settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	InitializeGameState();

	return tl::RunWindowUpdateLoop(settings.targetFPS);
}
