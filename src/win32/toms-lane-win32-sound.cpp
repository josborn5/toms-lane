#include <dsound.h>

namespace tl
{

static LPDIRECTSOUNDBUFFER globalSecondarySoundBuffer;

// Define a function type that mirrors the signature of the
// DirectSound function being called
typedef HRESULT WINAPI direct_sound_create(
	LPCGUID pcGuidDevice,
	LPDIRECTSOUND* ppDS,
	LPUNKNOWN pUnkOuter
);

struct SoundConfig
{
	int samplesPerSecond;
	int bytesPerSample;
	DWORD bufferSizeInBytes;
};

int Win32SoundSetup(int samplesPerSecond, HWND window, int bufferSizeInBytes)
{
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

	WORD numberOfChannels = 2;
	WORD bitsPerSample = 16;
	WORD blockAlign = (WORD)(numberOfChannels * bitsPerSample / 8);

	WAVEFORMATEX waveFormat = {};
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = numberOfChannels;
	waveFormat.nSamplesPerSec = samplesPerSecond;
	waveFormat.wBitsPerSample = bitsPerSample;
	waveFormat.nBlockAlign = blockAlign;
	waveFormat.nAvgBytesPerSec = (samplesPerSecond * blockAlign);
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

	if (!SUCCEEDED(directSound->CreateSoundBuffer(&directSoundBufferConfig, &globalSecondarySoundBuffer, 0)))
	{
		return -7;
	}

	return 0;
}

int Win32ClearSoundBuffer(const SoundConfig& soundConfig)
{
	VOID* region1;
	DWORD region1Size;
	VOID* region2;
	DWORD region2Size;

	if (!SUCCEEDED(globalSecondarySoundBuffer->Lock(
		0,
		soundConfig.bufferSizeInBytes,
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

	globalSecondarySoundBuffer->Unlock(
		region1,
		region1Size,
		region2,
		region2Size
	);

	return 0;
}

int Win32FillSoundBuffer(
	const SoundConfig& soundConfig,
	DWORD byteToLock,
	DWORD bytesToWrite,
	const SoundBuffer& sourceSound
)
{
	VOID* region1;
	DWORD region1Size;
	VOID* region2;
	DWORD region2Size;

	if (!SUCCEEDED(globalSecondarySoundBuffer->Lock(
		byteToLock,
		bytesToWrite,
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
	int16_t* sourceSample = sourceSound.samples;
	DWORD region1SampleCount = region1Size / soundConfig.bytesPerSample;
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
	DWORD region2SampleCount = region2Size / soundConfig.bytesPerSample;
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

	globalSecondarySoundBuffer->Unlock(
		region1,
		region1Size,
		region2,
		region2Size
	);

	return 0;
}

}