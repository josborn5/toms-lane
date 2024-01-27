#include "../platform/toms-lane-platform.hpp"
#include "../win32/win32-sound.hpp"
#include <math.h>

struct Tone
{
	int durationCount;
	int sampleCounter = 0;
	int toneHz;
	double volume;
};

static Tone activeTone;

static const int samplesPerSecond = 44100;
static const int samplesPerCallback = 512; // TODO: work out sound card latency and optimize
static double pi = 3.14159;
static double max16BitValue = 32767;

static int samplesPerMillisecond = samplesPerSecond / 1000;

void playTone(int toneHz, int durationInMilliseconds)
{
	activeTone.durationCount = samplesPerMillisecond * durationInMilliseconds;
	activeTone.sampleCounter = 0;
	activeTone.toneHz = toneHz;
	activeTone.volume = 0.15;
}

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	int16_t* sampleOutput = soundBuffer.samples;

	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		double soundValueAs16Bit = 0;
		if (activeTone.sampleCounter < activeTone.durationCount)
		{
			double soundValue = activeTone.volume * sin(activeTone.sampleCounter * activeTone.toneHz * 2.0 * pi / (double)samplesPerSecond);
			soundValueAs16Bit = max16BitValue * soundValue;
		}

		*sampleOutput = (int16_t)soundValueAs16Bit;
		sampleOutput++;
		activeTone.sampleCounter += 1;
	}

	return 0;
}

int startSound()
{
	return tl::win32_sound_interface_initialize(
		0,
		&UpdateSound,
		samplesPerCallback,
		samplesPerSecond,
		1
	);
}

