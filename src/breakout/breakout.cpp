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
#include "../platform/toms-lane-platform.hpp"
#include "game.h"
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

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	float tSineWave = soundBuffer.firstSampleTime;
	int toneVolume = 1500;
	int wavePeriod = soundBuffer.samplesPerSecond / 256;
	int16_t* sampleOutput = soundBuffer.samples;

	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		float sineValue = sinf(tSineWave);
		int16_t sampleValue = (int16_t)(sineValue * toneVolume);
		*sampleOutput = sampleValue;
		sampleOutput++;
		tSineWave += 2.0f * 3.14159f * 1.0f / (float)wavePeriod;
	}
	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	tl::WindowSettings settings;
	settings.title = "Breakout";
	settings.width = 1280;
	settings.height = 720;
	settings.targetFPS = 60;
	settings.openConsole = true;
	settings.playSound = true;
	settings.updateSoundCallback = UpdateSound;

	int windowOpenResult = tl::OpenWindow(instance, settings);

	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	return tl::RunWindowUpdateLoop(settings.targetFPS, true, true);
}
