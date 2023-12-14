#ifndef TOMS_LANE_WIN32_SOUND
#define TOMS_LANE_WIN32_SOUND

#include "../platform/toms-lane-application.hpp"

namespace tl
{

int win32_sound_interface_initialize(
	HWND window,
	UpdateSoundCallback updateSoundCallback
);

int win32_sound_interface_frame_update(
	int gameUpdateHz,
	LARGE_INTEGER frameStartCounter,
	int targetMicroSecondsPerFrame
);

}

#endif