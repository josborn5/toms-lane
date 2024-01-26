#include "../platform/toms-lane-platform.hpp"

struct Tone
{
	int durationCount;
	int sampleCounter;
	int toneHz;
	double volume;
};

Tone activeTone = { 44100, 0, 256, 0.1 };

static const int samplesPerSecond = 44100;
static const int samplesPerCallback = 4096;
static bool soundOn = true;
static double pi = 3.14159;
static double max16BitValue = 32767;

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	int16_t* sampleOutput = soundBuffer.samples;

	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		double soundValueAs16Bit = 0;
		if (soundOn)
		{
			double soundValue = activeTone.volume * sin(activeTone.sampleCounter * activeTone.toneHz * 2.0 * pi / (double)samplesPerSecond);
			soundValueAs16Bit = max16BitValue * soundValue;
		}

		*sampleOutput = (int16_t)soundValueAs16Bit;
		sampleOutput++;
		activeTone.sampleCounter += 1;

		if (activeTone.sampleCounter == activeTone.durationCount)
		{
			soundOn = !soundOn;
			activeTone.sampleCounter = 0;
		}
	}


	return 0;
}


