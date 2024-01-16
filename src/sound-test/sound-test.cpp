#include <windows.h>
#include <math.h>
#include "../win32/win32-console.hpp"
#include "../win32/win32-sound.hpp"

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
	tl::console_interface_open();

	tl::win32_sound_interface_initialize(0, &UpdateSound);

	while (true)
	{

	}
}
