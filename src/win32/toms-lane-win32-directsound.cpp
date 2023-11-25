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

//https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee416967(v=vs.85) 
class Win32DirectSound
{
public:
	int initialize(
		HWND window
	)
	{
		_samplesPerMillisecond = 48;
		_bytesPerSample = 2 * sizeof(int16_t);
		_bufferSizeInBytes = _bufferSizeInMilliseconds * _samplesPerMillisecond * _bytesPerSample;

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
		waveFormat.nSamplesPerSec = _samplesPerMillisecond * 1000;
		waveFormat.wBitsPerSample = bitsPerSample;
		waveFormat.nBlockAlign = blockAlign;
		waveFormat.nAvgBytesPerSec = (_samplesPerMillisecond * 1000 * blockAlign);
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
		directSoundBufferConfig.dwBufferBytes = _bufferSizeInBytes;
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
			_bufferSizeInBytes,
			MEM_RESERVE|MEM_COMMIT,
			PAGE_READWRITE
		);

		return 0;
	}


	int initializeSoundBuffer(
		int gameUpdateHz,
		LARGE_INTEGER frameStartCounter,
		int targetMicroSecondsPerFrame,
		const Win32Time& timer,
		SoundBuffer& soundBuffer
	)
	{
		DWORD expectedBytesPerFrame = _samplesPerMillisecond * 1000 * _bytesPerSample / gameUpdateHz;

		int frameDurationToAudioStart = timer.getMicroSecondsElapsed(frameStartCounter);
		int microSecondsToFrameEnd = targetMicroSecondsPerFrame - frameDurationToAudioStart;

		DWORD expectedBytesToFrameEnd = (DWORD)(microSecondsToFrameEnd * expectedBytesPerFrame / targetMicroSecondsPerFrame);

		DWORD playCursor;
		DWORD writeCursor;
		if (_secondarySoundBuffer->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK)
		{
			return -1;
		}

		_playCursor = playCursor;
		_writeCursor = writeCursor;


		DWORD expectedFrameEndByte = playCursor + expectedBytesToFrameEnd;
		DWORD targetCursor = 0;
		_expectedFrameEndByte = expectedFrameEndByte;

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
		targetCursor = targetCursor % _bufferSizeInBytes;
		_targetCursor = targetCursor;

		int runningSampleIndex = writeCursor / _bytesPerSample;
		_byteToLock = (runningSampleIndex * _bytesPerSample) % _bufferSizeInBytes;

		_bytesToWrite = (_byteToLock > targetCursor)
			? targetCursor - _byteToLock + _bufferSizeInBytes
			: targetCursor - _byteToLock;

		soundBuffer.samplesPerSecond = _samplesPerMillisecond * 1000;
		soundBuffer.sampleCount = _bytesToWrite / _bytesPerSample;
		soundBuffer.runningSampleIndex = runningSampleIndex;
		soundBuffer.firstSampleTime = (float)runningSampleIndex / (float)(_bufferSizeInMilliseconds * _samplesPerMillisecond);

		_bytesToWrite = soundBuffer.sampleCount * _bytesPerSample;
		soundBuffer.samples = _samples;

		return 0;
	}

	int processSoundBuffer(
		const SoundBuffer& sourceSound
	)
	{
		VOID* region1;
		DWORD region1Size;
		VOID* region2;
		DWORD region2Size;

		if (!SUCCEEDED(_secondarySoundBuffer->Lock(
			_byteToLock,
			_bytesToWrite,
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
		DWORD region1SampleCount = region1Size / _bytesPerSample;
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
		DWORD region2SampleCount = region2Size / _bytesPerSample;
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

	int byteToLock()
	{
		return (int)_byteToLock;
	}

	int bytesToWrite()
	{
		return (int)_bytesToWrite;
	}

	uint32_t bufferSizeInBytes()
	{
		return (uint32_t)_bufferSizeInBytes;
	}

	int playCursor()
	{
		return _playCursor;
	}

	int writeCursor()
	{
		return _writeCursor;
	}

	int expectedFrameEndByte()
	{
		return _expectedFrameEndByte;
	}

	int targetCursor()
	{
		return _targetCursor;
	}

private:
	LPDIRECTSOUNDBUFFER _secondarySoundBuffer;
	int16_t* _samples = nullptr;
	DWORD _byteToLock;
	DWORD _bytesToWrite;
	int _bufferSizeInMilliseconds = 1000;
	int _bytesPerSample;
	int _samplesPerMillisecond;
	DWORD _bufferSizeInBytes;
	DWORD _minByte;
	DWORD _playCursor;
	DWORD _writeCursor;
	DWORD _expectedFrameEndByte;
	DWORD _targetCursor;

	int clearSoundBuffer()
	{
		VOID* region1;
		DWORD region1Size;
		VOID* region2;
		DWORD region2Size;

		if (!SUCCEEDED(_secondarySoundBuffer->Lock(
			0,
			_bufferSizeInBytes,
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
};

}
