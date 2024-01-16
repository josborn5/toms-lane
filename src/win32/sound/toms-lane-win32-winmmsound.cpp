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
	HWAVEOUT audioOutputDeviceHandle;
};

Win32MMSound win32Sound;

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
	waveFormat.wBitsPerSample = sizeof(int16_t) * 8;
	waveFormat.nBlockAlign = (WORD)(waveFormat.nChannels * waveFormat.wBitsPerSample / 8);
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
	waveFormat.cbSize = 0;

/* general error return values */
// #define MMSYSERR_NOERROR      0                    /* no error */
// #define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  /* unspecified error */
// #define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  /* device ID out of range */
// #define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  /* driver failed enable */
// #define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  /* device already allocated */
// #define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  /* device handle is invalid */
// #define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  /* no device driver present */
// #define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  /* memory allocation error */
// #define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  /* function isn't supported */
// #define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  /* error value out of range */
// #define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) /* invalid flag passed */
// #define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) /* invalid parameter passed */
// #define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) /* handle being used */
/* simultaneously on another */
/* thread (eg callback) */
// #define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) /* specified alias not found */
// #define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) /* bad registry database */
// #define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) /* registry key not found */
// #define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) /* registry read error */
// #define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) /* registry write error */
// #define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) /* registry delete error */
// #define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) /* registry value not found */
// #define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) /* driver does not call DriverCallback */
// #define MMSYSERR_MOREDATA     (MMSYSERR_BASE + 21) /* more data to be returned */
// #define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 21) /* last error in range */

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

int win32_sound_interface_initialize(
	HWND window,
	UpdateSoundCallback updateSoundCallback
)
{
	WAVEOUTCAPS currentSoundDevice;
	win32_sound_device_get(currentSoundDevice);
	win32_sound_device_initialize(currentSoundDevice);

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
