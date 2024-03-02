#ifndef TOMS_LANE_WIN32_SOUND
#define TOMS_LANE_WIN32_SOUND

#include <windows.h>
#include "../tl-sound.hpp"

namespace tl
{

int win32_sound_interface_initialize(
	HWND window,
	UpdateSoundCallback updateSoundCallback,
	int samplesToProcessPerCallback,
	int samplesPerSecond,
	int numberOfChannels
);

}

#endif
