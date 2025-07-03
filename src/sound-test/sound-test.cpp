#include <math.h>
#include <windows.h>
#include "../application/win32/win32-application.hpp"
#include "../tl-application.hpp"

static uint64_t sampleCounter = 0;
static const int sampleRate = 48000;
static const int samplesPerCallback = 4096;

static HINSTANCE _instance;
static HWND _window = 0;

HINSTANCE tl::instance_handle_get()
{
	return _instance;
}

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
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
	_instance = instance;

	tl::sound_interface_initialize(
		&UpdateSound,
		samplesPerCallback,
		sampleRate,
		1
	);

	while (true)
	{

	}
}
