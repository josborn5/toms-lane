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
#include "../win32/win32-sound.hpp"
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

static uint32_t sampleCounter = 0;
static const int samplesPerSecond = 44100;
static const int samplesPerCallback = 4096;
static bool soundOn = true;

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	double toneHz = 256.0;
	double pi = 3.14159;
	double max16BitValue = 32767;

	int16_t* sampleOutput = soundBuffer.samples;

	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		double soundValueAs16Bit = 0;
		if (soundOn)
		{
			double soundValue = 0.1 * sin(sampleCounter * toneHz * 2.0 * pi / (double)samplesPerSecond);
			soundValueAs16Bit = max16BitValue * soundValue;
		}

		*sampleOutput = (int16_t)soundValueAs16Bit;
		sampleOutput++;
		sampleCounter += 1;

		if (sampleCounter == samplesPerSecond)
		{
			soundOn = !soundOn;
			sampleCounter = 0;
		}
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
	settings.playSound = true;

	tl::console_interface_open();

	int windowOpenResult = tl::OpenWindow(instance, settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::win32_sound_interface_initialize(
		0,
		&UpdateSound,
		samplesPerCallback,
		samplesPerSecond,
		1
	);

	return tl::RunWindowUpdateLoop(settings.targetFPS);
}
