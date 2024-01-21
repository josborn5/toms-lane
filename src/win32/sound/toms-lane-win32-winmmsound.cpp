#include <windows.h>
#include <stdint.h>
#include "mmeapi.h"
#include "../../platform/toms-lane-application.hpp"
#include "../win32-console.hpp"

namespace tl
{

struct Win32MMSound
{
	int deviceID;
	UpdateSoundCallback updateSoundCallback;
	HWAVEOUT audioOutputDeviceHandle;
	WAVEHDR waveHeader;
	int totalBytesPerSample; // total number of bytes needed to store a single sample
	uint32_t sampleCount = 512;
};

static Win32MMSound win32Sound;

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

static void CALLBACK waveOutProcProxy(
	HWAVEOUT hwo,
	UINT uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
)
{
	console_interface_write("HELLO from waveProcCallback!\n");

	switch (uMsg) {
		case WOM_OPEN:
			console_interface_write("Received WOM open!!!\n");
			break;
		case WOM_DONE:
			console_interface_write("Received WOM done!!!\n");

			// continue playback
			waveOutPrepareHeader(
				win32Sound.audioOutputDeviceHandle,
				&win32Sound.waveHeader,
				sizeof(win32Sound.waveHeader)
			);
			waveOutWrite(
				win32Sound.audioOutputDeviceHandle,
				&win32Sound.waveHeader,
				sizeof(win32Sound.waveHeader)
			);

			break;
	}
}

static int initializeSoundDevice(const WAVEOUTCAPS& device)
{
	int numberOfChannels = 1;
	int samplesPerSecond = 48000;
	int bytesPerSamplePerChannel = sizeof(int16_t);
	win32Sound.totalBytesPerSample = numberOfChannels * bytesPerSamplePerChannel;

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = (WORD)numberOfChannels;
	waveFormat.nSamplesPerSec = samplesPerSecond;
	waveFormat.wBitsPerSample = (WORD)(bytesPerSamplePerChannel * 8); // there are 8 bits in a byte
	waveFormat.nBlockAlign = (WORD)(win32Sound.totalBytesPerSample);
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
	int bufferSizeInBytes = win32Sound.totalBytesPerSample * win32Sound.sampleCount;

	win32Sound.waveHeader.dwBufferLength = bufferSizeInBytes;
	win32Sound.waveHeader.dwBytesRecorded = 0;
	win32Sound.waveHeader.dwUser = 0;
	win32Sound.waveHeader.dwFlags = 0;
	win32Sound.waveHeader.dwLoops = 0;

	int16_t* buffer = (int16_t*)VirtualAlloc(
		0,
		bufferSizeInBytes,
		MEM_RESERVE|MEM_COMMIT,
		PAGE_READWRITE
	);

	win32Sound.waveHeader.lpData = (LPSTR)buffer;

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
	waveOutPrepareHeader(
		win32Sound.audioOutputDeviceHandle,
		&win32Sound.waveHeader,
		sizeof(win32Sound.waveHeader)
	);
	waveOutWrite(
		win32Sound.audioOutputDeviceHandle,
		&win32Sound.waveHeader,
		sizeof(win32Sound.waveHeader)
	);

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
