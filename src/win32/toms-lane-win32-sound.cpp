#include <dsound.h>

namespace tl
{

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

class Win32DirectSound
{
public:
	int initialize(
		HWND window
	)
	{
		_soundConfig.samplesPerSecond = 48000;
		_soundConfig.bytesPerSample = 2 * sizeof(int16_t);
		_soundConfig.bufferSizeInBytes = _soundConfig.samplesPerSecond * _soundConfig.bytesPerSample;

		int samplesPerSecond = _soundConfig.samplesPerSecond;
		int bufferSizeInBytes = _soundConfig.bufferSizeInBytes;

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

		if (!SUCCEEDED(directSound->CreateSoundBuffer(&directSoundBufferConfig, &_secondarySoundBuffer, 0)))
		{
			return -7;
		}

		clearSoundBuffer();
		_secondarySoundBuffer->Play(
			0,
			0,
			DSBPLAY_LOOPING
		);

		_samples = (int16_t*)VirtualAlloc(
			0,
			_soundConfig.bufferSizeInBytes,
			MEM_RESERVE|MEM_COMMIT,
			PAGE_READWRITE
		);

		return 0;
	}


	int processSound(
		int gameUpdateHz,
		LARGE_INTEGER frameStartCounter,
		int targetMicroSecondsPerFrame,
		const Win32Time& timer
	)
	{
		DWORD playCursor;
		DWORD writeCursor;
		if (_secondarySoundBuffer->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK)
		{
			return -1;
		}

		int runningSampleIndex = writeCursor / _soundConfig.bytesPerSample;

		DWORD expectedBytesPerFrame = _soundConfig.samplesPerSecond * _soundConfig.bytesPerSample / gameUpdateHz;

		int frameDurationToAudioStart = timer.getMicroSecondsElapsed(frameStartCounter);
		int microSecondsToFrameEnd = targetMicroSecondsPerFrame - frameDurationToAudioStart;

		DWORD expectedBytesToFrameEnd = (DWORD)((microSecondsToFrameEnd / targetMicroSecondsPerFrame) * expectedBytesPerFrame);
		DWORD expectedFrameEndByte = playCursor + expectedBytesToFrameEnd;

		DWORD safeWriteCursor = writeCursor;
		if (safeWriteCursor < playCursor)
		{
			safeWriteCursor += _soundConfig.bufferSizeInBytes;
		}

		DWORD targetCursor = expectedFrameEndByte + expectedBytesPerFrame;
		targetCursor = targetCursor % _soundConfig.bufferSizeInBytes;

		DWORD byteToLock = (runningSampleIndex * _soundConfig.bytesPerSample) % _soundConfig.bufferSizeInBytes;

		DWORD bytesToWrite = (byteToLock > targetCursor)
			? targetCursor - byteToLock + _soundConfig.bufferSizeInBytes
			: targetCursor - byteToLock;

		SoundBuffer soundBuffer = {0};
		soundBuffer.samplesPerSecond = _soundConfig.samplesPerSecond;
		soundBuffer.sampleCount = bytesToWrite / _soundConfig.bytesPerSample;

		bytesToWrite = soundBuffer.sampleCount * _soundConfig.bytesPerSample;
		soundBuffer.samples = _samples;

		// Call into the application to fill the sound buffer
		UpdateSound(soundBuffer);

		DWORD unwrappedWriteCursor = writeCursor;
		if (unwrappedWriteCursor < playCursor)
		{
			unwrappedWriteCursor += _soundConfig.bufferSizeInBytes;
		}

		fillSoundBuffer(
			byteToLock,
			bytesToWrite,
			soundBuffer
		);

		return 0;
	}


	int fillSoundBuffer(
		DWORD byteToLock,
		DWORD bytesToWrite,
		const SoundBuffer& sourceSound
	)
	{
		VOID* region1;
		DWORD region1Size;
		VOID* region2;
		DWORD region2Size;

		if (!SUCCEEDED(_secondarySoundBuffer->Lock(
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
		DWORD region1SampleCount = region1Size / _soundConfig.bytesPerSample;
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
		DWORD region2SampleCount = region2Size / _soundConfig.bytesPerSample;
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

		_secondarySoundBuffer->Unlock(
			region1,
			region1Size,
			region2,
			region2Size
		);

		return 0;
	}


	int clearSoundBuffer()
	{
		VOID* region1;
		DWORD region1Size;
		VOID* region2;
		DWORD region2Size;

		if (!SUCCEEDED(_secondarySoundBuffer->Lock(
			0,
			_soundConfig.bufferSizeInBytes,
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

		_secondarySoundBuffer->Unlock(
			region1,
			region1Size,
			region2,
			region2Size
		);

		return 0;
	}



private:
	LPDIRECTSOUNDBUFFER _secondarySoundBuffer;
	SoundConfig _soundConfig = {};
	int16_t* _samples = nullptr;
};

}
