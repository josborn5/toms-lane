#include <windows.h>
#include <stdint.h>
#include "mmeapi.h"
#include "../../platform/toms-lane-application.hpp"
#include "../win32-console.hpp"

#define WAVE_HEADER_COUNT 2

namespace tl
{

struct Win32MMSound
{
	int deviceID;
	UpdateSoundCallback updateSoundCallback;
	HWAVEOUT audioOutputDeviceHandle;
	WAVEHDR waveHeader[WAVE_HEADER_COUNT];
	int bytesPerSampleAcrossAllChannels;
	uint32_t sampleCountPerChannel = 32768;
};
static Win32MMSound win32Sound;

static int currentHeaderIndex = 0;

static int getSoundDevice(WAVEOUTCAPS& device)
{
	int numberOfSoundDevices = waveOutGetNumDevs();
	bool foundDevice = false;
	for (int i = 0; i < numberOfSoundDevices && !foundDevice; i += 1)
	{
		MMRESULT result = waveOutGetDevCaps(i, &device, sizeof(device));
		foundDevice = (result == S_OK);
		if (foundDevice)
		{
			win32Sound.deviceID = i;
		}
	}

	if (!foundDevice)
	{
		return -1;
	}

	return 0;
}

static int processSoundBuffer()
{
	WAVEHDR* currentWaveHeader = &win32Sound.waveHeader[currentHeaderIndex];

	if (currentWaveHeader->dwFlags & WHDR_PREPARED)
	{
		waveOutUnprepareHeader(
			win32Sound.audioOutputDeviceHandle,
			currentWaveHeader,
			sizeof(*currentWaveHeader)
		);
	}

	SoundBuffer soundBuffer;
	soundBuffer.sampleCount = win32Sound.sampleCountPerChannel;
	soundBuffer.samples = (int16_t*)currentWaveHeader->lpData;

	if (win32Sound.updateSoundCallback != nullptr)
	{
		win32Sound.updateSoundCallback(soundBuffer);
	}

	waveOutPrepareHeader(
		win32Sound.audioOutputDeviceHandle,
		currentWaveHeader,
		sizeof(*currentWaveHeader)
	);

	waveOutWrite(
		win32Sound.audioOutputDeviceHandle,
		currentWaveHeader,
		sizeof(*currentWaveHeader)
	);

	currentHeaderIndex += 1;
	currentHeaderIndex = currentHeaderIndex % WAVE_HEADER_COUNT;

	return 0;
}

static void CALLBACK waveOutProcProxy(
	HWAVEOUT hwo,
	UINT uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
)
{
	switch (uMsg) {
		case WOM_OPEN:
			console_interface_write("Received WOM open!!!\n");
			break;
		case WOM_DONE:
			console_interface_write("Received WOM done!!!\n");

			// continue playback
			processSoundBuffer();

			break;
	}
}

static int initializeSoundDevice(const WAVEOUTCAPS& device)
{
	int numberOfChannels = 1;
	int samplesPerSecond = 48000;
	int bytesPerSamplePerChannel = sizeof(int16_t);
	win32Sound.bytesPerSampleAcrossAllChannels = numberOfChannels * bytesPerSamplePerChannel;

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = (WORD)numberOfChannels;
	waveFormat.nSamplesPerSec = samplesPerSecond;
	waveFormat.wBitsPerSample = (WORD)(bytesPerSamplePerChannel * 8); // there are 8 bits in a byte
	waveFormat.nBlockAlign = (WORD)(win32Sound.bytesPerSampleAcrossAllChannels);
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
	waveFormat.cbSize = 0;

	MMRESULT deviceOpenResult = waveOutOpen(
		&win32Sound.audioOutputDeviceHandle,
		win32Sound.deviceID,
		&waveFormat,
		(DWORD_PTR)waveOutProcProxy,
		(DWORD_PTR)waveOutProcProxy,
		CALLBACK_FUNCTION
	);

	if (deviceOpenResult != MMSYSERR_NOERROR)
	{
		char errorCodeBuffer[256];
		wsprintf(errorCodeBuffer, "Sound error code: %d\n", deviceOpenResult);
		console_interface_write(errorCodeBuffer);
		return -2;
	}

	return 0;
}

static int initializeSoundBuffer()
{
	// Allocate memory for the sound buffer
	int bufferSizeInBytesPerHeader = win32Sound.bytesPerSampleAcrossAllChannels * win32Sound.sampleCountPerChannel;
	for (int i = 0; i < WAVE_HEADER_COUNT; i += 1)
	{
		win32Sound.waveHeader[i].lpData = (LPSTR)VirtualAlloc(
			0,
			bufferSizeInBytesPerHeader,
			MEM_RESERVE|MEM_COMMIT,
			PAGE_READWRITE
		);

		if (win32Sound.waveHeader[i].lpData == nullptr)
		{
			return -1;
		}

		win32Sound.waveHeader[i].dwBufferLength = bufferSizeInBytesPerHeader;
		win32Sound.waveHeader[i].dwBytesRecorded = 0;
		win32Sound.waveHeader[i].dwUser = 0;
		win32Sound.waveHeader[i].dwFlags = 0;
		win32Sound.waveHeader[i].dwLoops = 0;
	}
	return 0;
}

int win32_sound_interface_initialize(
	HWND window,
	UpdateSoundCallback updateSoundCallback
)
{
	WAVEOUTCAPS currentSoundDevice;
	int soundDeviceGetResult = getSoundDevice(currentSoundDevice);
	if (soundDeviceGetResult != 0)
	{
		return soundDeviceGetResult;
	}
	int soundDeviceInitializeResult = initializeSoundDevice(currentSoundDevice);
	if (soundDeviceInitializeResult != 0)
	{
		return soundDeviceInitializeResult;
	}

	int soundBufferInitializeResult = initializeSoundBuffer();
	if (soundBufferInitializeResult != 0)
	{
		return soundBufferInitializeResult;
	}
	win32Sound.updateSoundCallback = updateSoundCallback;

	// start playback
	processSoundBuffer();

	return 0;
}

int win32_sound_interface_frame_update(
	int gameUpdateHz,
	LARGE_INTEGER frameStartCounter,
	int targetMicroSecondsPerFrame
)
{
	// console_interface_write("HELLO from frame update!");
	return 0;
}

}
