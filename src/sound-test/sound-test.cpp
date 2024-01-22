#include <windows.h>
#include <math.h>
#include "../win32/win32-console.hpp"
#include "../win32/win32-sound.hpp"

static uint64_t sampleCounter = 0;
static const int sampleRate = 48000;
static const int samplesPerCallback = 48000;

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	tl::console_interface_write("### hello from callback!!!\n");

	double toneHz = 440.0;
	double pi = 3.14159;
	double max16BitValue = 32767;

	int16_t* sampleOutput = soundBuffer.samples;

	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		double soundValue = 0.1 * sin(sampleCounter * toneHz * 2.0 * pi / (double)sampleRate);
		double soundValueAs16Bit = max16BitValue * soundValue;

		*sampleOutput = (int16_t)soundValueAs16Bit;
		sampleOutput++;
		sampleCounter += 1;
	}
	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	tl::console_interface_open();

	tl::win32_sound_interface_initialize(
		0,
		&UpdateSound,
		samplesPerCallback,
		sampleRate,
		1
	);

	while (true)
	{

	}
}
