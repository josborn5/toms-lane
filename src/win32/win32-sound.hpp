#ifndef TOMS_LANE_WIN32_SOUND
#define TOMS_LANE_WIN32_SOUND

#include "../platform/toms-lane-application.hpp"

namespace tl
{

int win32_sound_interface_initialize(
	HWND window
);

int win32_sound_interface_buffer_initialize(
	int gameUpdateHz,
	LARGE_INTEGER frameStartCounter,
	int targetMicroSecondsPerFrame,
	SoundBuffer& soundBuffer
);

int win32_sound_interface_buffer_process(
	const SoundBuffer& soundBuffer
);

}

#endif