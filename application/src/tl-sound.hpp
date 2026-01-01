#ifndef TOMS_LANE_SOUND_HPP
#define TOMS_LANE_SOUND_HPP

#include <stdint.h>

namespace tl
{
	struct SoundBuffer
	{
		int sampleCount;
		int16_t* samples;
	};

	// Define function signature of updating the sound buffer
	typedef int (*UpdateSoundCallback)(const SoundBuffer& soundBuffer);

	int sound_interface_initialize(
		UpdateSoundCallback updateSoundCallback,
		int samplesToProcessPerCallback,
		int samplesPerSecond,
		int numberOfChannels
	);
}

#endif
