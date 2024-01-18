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
	int16_t* buffer;
	uint32_t sampleCount = 512;
};

static Win32MMSound win32Sound;

static int win32_sound_device_get(WAVEOUTCAPS& device)
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
	console_interface_write("HELLO from waveProcCallback!");
}

static int win32_sound_device_initialize(const WAVEOUTCAPS& device)
{
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 48000;
	waveFormat.wBitsPerSample = sizeof(int16_t) * 8; // sizeof returns size in bytes and there are 8 bits per byte
	waveFormat.nBlockAlign = (WORD)(waveFormat.nChannels * waveFormat.wBitsPerSample / 8);
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

	// Allocate memory for the sound buffer
	int bufferSizeInBytes = waveFormat.nBlockAlign * win32Sound.sampleCount;
	win32Sound.buffer = (int16_t*)VirtualAlloc(
		0,
		bufferSizeInBytes,
		MEM_RESERVE|MEM_COMMIT,
		PAGE_READWRITE
	);

	return 0;
}

int win32_sound_interface_initialize(
	HWND window,
	UpdateSoundCallback updateSoundCallback
)
{
	WAVEOUTCAPS currentSoundDevice;
	int soundDeviceGetResult = win32_sound_device_get(currentSoundDevice);
	if (soundDeviceGetResult != 0)
	{
		return soundDeviceGetResult;
	}
	int soundDeviceInitializeResult = win32_sound_device_initialize(currentSoundDevice);
	if (soundDeviceInitializeResult != 0)
	{
		return soundDeviceInitializeResult;
	}
	win32Sound.updateSoundCallback = updateSoundCallback;

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
