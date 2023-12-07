#ifndef TOMS_LANE_APPLICATION_H
#define TOMS_LANE_APPLICATION_H

#include <stdint.h>

namespace tl
{
	enum KEY
	{
		KEY_ENTER,
		KEY_SPACE,
		KEY_SHIFT,
		KEY_TAB,
		KEY_CTRL,
		KEY_L_ALT, KEY_R_ALT,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

		KEY_ESCAPE,

		KEY_COUNT
	};

	enum MOUSE_BUTTON
	{
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,

		MOUSE_BUTTON_COUNT
	};

	struct RenderBuffer
	{
		unsigned int* pixels;
		int width;
		int height;
		int pitch;
		int bytesPerPixel; // = 4;
		float* depth;
	};

	struct SoundBuffer
	{
		int samplesPerSecond;
		int sampleCount;
		int16_t* samples;
		float firstSampleTime; // 'buffer time' for first sample
		int runningSampleIndex;
	};

	// Define function signature of updating the sound buffer
	typedef int (*UpdateSoundCallback)(const SoundBuffer& soundBuffer);

	struct WindowSettings
	{
		int width;
		int height;
		char* title;
		int targetFPS;
		unsigned long permanentSpaceInMegabytes = 1;
		unsigned long transientSpaceInMegabytes = 1;
		bool openConsole = false;
		bool playSound = false;
		UpdateSoundCallback updateSoundCallback = nullptr;
	};

	struct Button
	{
		bool isDown;
		bool wasDown;
		bool keyUp;
	};

	struct Mouse
	{
		int x;
		int y;
		Button buttons[MOUSE_BUTTON_COUNT];
	};

	struct Input
	{
		Mouse mouse;
		Button buttons[KEY_COUNT];
	};

	struct MemorySpace
	{
		void* content;
		uint64_t sizeInBytes;
	};

	struct GameMemory
	{
		MemorySpace permanent;
		MemorySpace transient;
	};

	int Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer);
	int UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt);
}

#endif
