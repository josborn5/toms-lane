#include <dsound.h>
#include <stdint.h>
#include "../win32-time.hpp"
#include "../../platform/toms-lane-application.hpp"

namespace tl
{

// Define a function type that mirrors the signature of the
// DirectSound function being called
typedef HRESULT WINAPI direct_sound_create(
	LPCGUID pcGuidDevice,
	LPDIRECTSOUND* ppDS,
	LPUNKNOWN pUnkOuter
);

struct Win32Sound
{
	LPDIRECTSOUNDBUFFER _secondarySoundBuffer;
	int16_t* _samples = nullptr;
	DWORD _byteToLock = 0;
	DWORD _bytesToWrite = 0;
	int _bufferSizeInMilliseconds = 0;
	int _bytesPerSample = 0;
	int _samplesPerMillisecond = 0;
	UpdateSoundCallback _updateSoundCallback = nullptr;
};

static Win32Sound sound;

static DWORD win32_sound_buffer_size_get()
{
	return sound._bufferSizeInMilliseconds * sound._samplesPerMillisecond * sound._bytesPerSample;
}

static int win32_sound_buffer_clear()
{
	VOID* region1;
	DWORD region1Size;
	VOID* region2;
	DWORD region2Size;

	int bufferSizeInBytes = win32_sound_buffer_size_get();

	if (!SUCCEEDED(sound._secondarySoundBuffer->Lock(
		0,
		bufferSizeInBytes,
		&region1,
		&region1Size,
		&region2,
		&region2Size,
		0
	)))
	{
		return -1;
	}

	uint8_t* destinationSample = (uint8_t*)region1;
	for (DWORD byteIndex = 0; byteIndex < region1Size; byteIndex += 1)
	{
		*destinationSample = 0;
		destinationSample++;
	}

	destinationSample = (uint8_t*)region2;
	for (DWORD byteIndex = 0; byteIndex < region2Size; byteIndex += 1)
	{
		*destinationSample = 0;
		destinationSample++;
	}

	sound._secondarySoundBuffer->Unlock(
		region1,
		region1Size,
		region2,
		region2Size
	);

	return 0;
}

int win32_sound_interface_initialize(
	HWND window,
	UpdateSoundCallback updateSoundCallback,
	int samplesToProcessPerCallback,
	int samplesPerSecond,
	int numberOfChannels
)
{
	sound._bufferSizeInMilliseconds = 1000;
	sound._samplesPerMillisecond = samplesPerSecond / 1000;
	sound._bytesPerSample = numberOfChannels * sizeof (sound._samplesPerMillisecond);
	sound._updateSoundCallback = updateSoundCallback;

	int bufferSizeInBytes = win32_sound_buffer_size_get();

	HMODULE directSoundLibrary = LoadLibraryA("dsound.dll");
	if (!directSoundLibrary)
	{
		return -1;
	}

	direct_sound_create* directSoundCreateFunction = (direct_sound_create*)GetProcAddress(directSoundLibrary, "DirectSoundCreate");

	LPDIRECTSOUND directSound;

	if (!directSoundCreateFunction)
	{
		return -2;
	}

	if (!SUCCEEDED(directSoundCreateFunction(0, &directSound, 0)))
	{
		return -3;
	}

	WORD bitsPerSample = 16;
	WORD blockAlign = (WORD)(numberOfChannels * bitsPerSample / 8);

	WAVEFORMATEX waveFormat = {};
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = (WORD)numberOfChannels;
	waveFormat.nSamplesPerSec = sound._samplesPerMillisecond * 1000;
	waveFormat.wBitsPerSample = bitsPerSample;
	waveFormat.nBlockAlign = blockAlign;
	waveFormat.nAvgBytesPerSec = (sound._samplesPerMillisecond * 1000 * blockAlign);
	waveFormat.cbSize = 0;

	if (!SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
	{
		return -4;
	}

	DSBUFFERDESC primaryBufferDescription = {};
	primaryBufferDescription.dwSize = sizeof(primaryBufferDescription);
	primaryBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

	LPDIRECTSOUNDBUFFER primaryBuffer;
	if (!SUCCEEDED(directSound->CreateSoundBuffer(
		&primaryBufferDescription,
		&primaryBuffer,
		0
	)))
	{
		return -5;
	}

	HRESULT formatSetResult = primaryBuffer->SetFormat(&waveFormat);
	if (!SUCCEEDED(formatSetResult))
	{
		return -6;
	}

	DSBUFFERDESC directSoundBufferConfig = {};
	directSoundBufferConfig.dwSize = sizeof(directSoundBufferConfig);
	directSoundBufferConfig.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
	directSoundBufferConfig.dwBufferBytes = bufferSizeInBytes;
	directSoundBufferConfig.lpwfxFormat = &waveFormat;

	if (!SUCCEEDED(directSound->CreateSoundBuffer(&directSoundBufferConfig, &sound._secondarySoundBuffer, 0)))
	{
		return -7;
	}

	win32_sound_buffer_clear();
	sound._secondarySoundBuffer->Play(
		0,
		0,
		DSBPLAY_LOOPING
	);

	sound._samples = (int16_t*)VirtualAlloc(
		0,
		bufferSizeInBytes,
		MEM_RESERVE|MEM_COMMIT,
		PAGE_READWRITE
	);

	return 0;
}

static int win32_sound_interface_buffer_initialize(
	int gameUpdateHz,
	LARGE_INTEGER frameStartCounter,
	int targetMicroSecondsPerFrame,
	SoundBuffer& soundBuffer
)
{
	DWORD expectedBytesPerFrame = sound._samplesPerMillisecond * 1000 * sound._bytesPerSample / gameUpdateHz;

	int frameDurationToAudioStart = win32_time_interface_elapsed_microseconds_get(frameStartCounter);
	int microSecondsToFrameEnd = targetMicroSecondsPerFrame - frameDurationToAudioStart;

	DWORD expectedBytesToFrameEnd = (DWORD)(microSecondsToFrameEnd * expectedBytesPerFrame / targetMicroSecondsPerFrame);

	DWORD playCursor;
	DWORD writeCursor;
	if (sound._secondarySoundBuffer->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK)
	{
		return -1;
	}


	DWORD expectedFrameEndByte = playCursor + expectedBytesToFrameEnd;
	DWORD targetCursor = 0;

	// | current frame         | next frame          | 
	// +-----------------------+---------------------+
	// |--------|--------------|---------------------|
	// play     |              frame end byte        target cursor
	// cursor   write cursor
	if (expectedFrameEndByte > writeCursor)
	{
		targetCursor = expectedFrameEndByte + expectedBytesPerFrame;
	}

	// | current frame         | next frame          | next frame
	// +-----------------------+---------------------+-----------
	// |-----------------------|-----------|--------------------|
	// play                    frame       |                    target cursor
	// cursor                  end byte    write cursor
	else
	{
		targetCursor = writeCursor + expectedBytesPerFrame;
	}
	int bufferSizeInBytes = win32_sound_buffer_size_get();
	targetCursor = targetCursor % bufferSizeInBytes;

	int runningSampleIndex = writeCursor / sound._bytesPerSample;
	sound._byteToLock = (runningSampleIndex * sound._bytesPerSample) % bufferSizeInBytes;

	sound._bytesToWrite = (sound._byteToLock > targetCursor)
		? targetCursor - sound._byteToLock + bufferSizeInBytes
		: targetCursor - sound._byteToLock;

	soundBuffer.samplesPerSecond = sound._samplesPerMillisecond * 1000;
	soundBuffer.sampleCount = sound._bytesToWrite / sound._bytesPerSample;
	soundBuffer.runningSampleIndex = runningSampleIndex;
	soundBuffer.firstSampleTime = (float)runningSampleIndex / (float)(sound._bufferSizeInMilliseconds * sound._samplesPerMillisecond);

	sound._bytesToWrite = soundBuffer.sampleCount * sound._bytesPerSample;
	soundBuffer.samples = sound._samples;

	return 0;
}


static int win32_sound_interface_buffer_process(
	const SoundBuffer& soundBuffer
)
{
	VOID* region1;
	DWORD region1Size;
	VOID* region2;
	DWORD region2Size;

	if (!SUCCEEDED(sound._secondarySoundBuffer->Lock(
		sound._byteToLock,
		sound._bytesToWrite,
		&region1,
		&region1Size,
		&region2,
		&region2Size,
		0
	)))
	{
		return -1;
	}

	int16_t* targetSample = (int16_t*)region1;
	int16_t* sourceSample = soundBuffer.samples;
	DWORD region1SampleCount = region1Size / sound._bytesPerSample;
	for (
		DWORD sampleIndex = 0;
		sampleIndex < region1SampleCount;
		sampleIndex += 1
	)
	{
		// Left Channel
		*targetSample = *sourceSample;
		targetSample++;
		sourceSample++;

		// Right Channel
		*targetSample = *sourceSample;
		targetSample++;
		sourceSample++;
	}

	targetSample = (int16_t*)region2;
	DWORD region2SampleCount = region2Size / sound._bytesPerSample;
	for (
		DWORD sampleIndex = 0;
		sampleIndex < region2SampleCount;
		sampleIndex += 1
	)
	{
		// Left Channel
		*targetSample = *sourceSample;
		targetSample++;
		sourceSample++;

		// Right Channel
		*targetSample = *sourceSample;
		targetSample++;
		sourceSample++;
	}

	sound._secondarySoundBuffer->Unlock(
		region1,
		region1Size,
		region2,
		region2Size
	);

	return 0;
}

int win32_sound_interface_frame_update(
	int gameUpdateHz,
	LARGE_INTEGER frameStartCounter,
	int targetMicroSecondsPerFrame
)
{
	SoundBuffer soundBuffer = {0};
	win32_sound_interface_buffer_initialize(
		gameUpdateHz,
		frameStartCounter,
		targetMicroSecondsPerFrame,
		soundBuffer
	);

	// Call into the application to fill the sound buffer
	sound._updateSoundCallback(soundBuffer);

	win32_sound_interface_buffer_process(
		soundBuffer
	);

	return 0;
}

//https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee416967(v=vs.85) 
}
