#include <windows.h>
#include <dsound.h>
#include <stdint.h>

#include "toms-lane-win32.hpp"

#include "toms-lane-win32-file.cpp"
#include "toms-lane-win32-console.cpp"

namespace tl
{

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)

static bool IsRunning = false;
static RenderBuffer globalRenderBuffer = {0};
static BITMAPINFO bitmapInfo = {0};	// platform dependent
static int64_t win32PerformanceCountsPerSecond;
LPDIRECTSOUNDBUFFER globalSecondarySoundBuffer;
static int maxAppFrameTimeInMicroSeconds = 0;

static void Win32_SizeglobalRenderBufferToCurrentWindow(HWND window)
{
	RECT clientRect = {0};
	GetClientRect(window, &clientRect);

	globalRenderBuffer.width = clientRect.right - clientRect.left;
	globalRenderBuffer.height = clientRect.bottom - clientRect.top;
	globalRenderBuffer.bytesPerPixel = sizeof(uint32_t);

	if (globalRenderBuffer.pixels)
	{
		VirtualFree(globalRenderBuffer.pixels, 0, MEM_RELEASE);
	}
	if (globalRenderBuffer.depth)
	{
		VirtualFree(globalRenderBuffer.depth, 0, MEM_RELEASE);
	}

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = globalRenderBuffer.width;
	bitmapInfo.bmiHeader.biHeight = globalRenderBuffer.height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	int bitmapPixelCount = globalRenderBuffer.width * globalRenderBuffer.height;
	int bitmapMemorySize = bitmapPixelCount * globalRenderBuffer.bytesPerPixel;
	globalRenderBuffer.pitch = globalRenderBuffer.width * globalRenderBuffer.bytesPerPixel;
	globalRenderBuffer.pixels = (uint32_t *)VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

	int depthBufferMemorySize = bitmapPixelCount * sizeof(float);
	globalRenderBuffer.depth = (float *)VirtualAlloc(0, depthBufferMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

static void Win32_DisplayglobalRenderBufferInWindow(HDC deviceContext)
{
	StretchDIBits(deviceContext,
		0, 0, globalRenderBuffer.width, globalRenderBuffer.height,
		0, 0, globalRenderBuffer.width, globalRenderBuffer.height,
		globalRenderBuffer.pixels, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32_MainWindowCallback(HWND window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = -1;

	switch(Message)
	{
		case WM_SIZE:
		{
			Win32_SizeglobalRenderBufferToCurrentWindow(window);
		} break;
		case WM_DESTROY:
		{
			IsRunning = false;
		} break;
		case WM_CLOSE:
		{
			IsRunning = false;
		} break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{

		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT paint = {0};
			HDC deviceContext = BeginPaint(window, &paint);
			Win32_DisplayglobalRenderBufferInWindow(deviceContext);
			EndPaint(window, &paint);
		} break;
		default:
		{
			// use windows default callback handler
			Result = DefWindowProc(window, Message, wParam, lParam);
		} break;
	}

	return Result;
}

static void ProcessButtonState(Button* button, int isDown, int wasDown)
{
	button->isDown = (isDown != 0);
	button->wasDown = (wasDown != 0);
	button->keyUp = (!button->isDown && button->wasDown);
}

static void Win32_ProcessKeyboardMessage(Button* inputButton, int isDown, int wasDown, int vkCode, int vkButton, KEY key)
{
	if (vkCode == vkButton)
	{
		Button* button = &inputButton[key];
		ProcessButtonState(button, isDown, wasDown);
	}
}

static void ResetButtons(Input *gameInput)
{
	for (int i = 0; i < KEY_COUNT; i += 1)
	{
		if (gameInput->buttons[i].keyUp)
		{
			gameInput->buttons[i].wasDown = false;
			gameInput->buttons[i].keyUp = false;
		}
	}

	for (int i = 0; i < MOUSE_BUTTON_COUNT; i += 1)
	{
		if (gameInput->mouse.buttons[i].keyUp)
		{
			gameInput->mouse.buttons[i].wasDown = false;
			gameInput->mouse.buttons[i].keyUp = false;
		}
	}
}

static void Win32_ProcessPendingMessages(Input* input)
{
	// flush the queue of Messages from windows in this loop
	MSG Message;
	while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		if(Message.message == WM_QUIT)
		{
			IsRunning = false;
		}

		switch(Message.message)
		{
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN: // https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-messages
			case WM_KEYUP:
			{
				uint32_t vKCode = (uint32_t)Message.wParam;
				bool wasDown = ((Message.lParam & (1 << 30)) != 0); // Bit #30 of the LParam tells us what the previous key was
				bool isDown = ((Message.lParam & (1 << 31)) == 0); // Bit #31 of the LParam tells us what the current key is
				if (wasDown != isDown)
				{
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'A', KEY_A);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'B', KEY_B);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'C', KEY_C);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'D', KEY_D);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'E', KEY_E);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'F', KEY_F);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'G', KEY_G);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'H', KEY_H);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'I', KEY_I);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'J', KEY_J);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'K', KEY_K);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'L', KEY_L);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'M', KEY_M);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'N', KEY_N);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'O', KEY_O);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'P', KEY_P);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'Q', KEY_Q);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'R', KEY_R);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'S', KEY_S);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'T', KEY_T);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'U', KEY_U);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'V', KEY_V);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'W', KEY_W);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'X', KEY_X);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'Y', KEY_Y);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, 'Z', KEY_Z);

					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '0', KEY_0);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '1', KEY_1);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '2', KEY_2);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '3', KEY_3);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '4', KEY_4);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '5', KEY_5);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '6', KEY_6);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '7', KEY_7);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '8', KEY_8);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, '9', KEY_9);

					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_SPACE, KEY_SPACE);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_UP, KEY_UP);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_DOWN, KEY_DOWN);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_LEFT, KEY_LEFT);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_RIGHT, KEY_RIGHT);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_ESCAPE, KEY_ESCAPE);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_RETURN, KEY_ENTER);
					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_TAB, KEY_TAB);

					Win32_ProcessKeyboardMessage(input->buttons, isDown, wasDown, vKCode, VK_CONTROL, KEY_CTRL);
				}

				bool altKeyDown = ((Message.lParam & (1 << 29)) != 0);
				if((vKCode == VK_F4) && altKeyDown)
				{
					IsRunning = false;
				}
			} break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			{
				bool wasMouseDown = ((Message.lParam & (1 << 30)) != 0); // Bit #30 of the LParam tells us what the previous key was
				bool isMouseDown = ((Message.lParam & (1 << 31)) == 0); // Bit #31 of the LParam tells us what the current key is
				ProcessButtonState(&input->mouse.buttons[MOUSE_BUTTON_LEFT], isMouseDown, wasMouseDown);
			} break;
			default: {
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			} break;

		}
	}
}

void DisplayLastWin32Error()
{
	DWORD ErrorCode = GetLastError();
	char ErrorCodeBuffer[256];
	wsprintf(ErrorCodeBuffer, "VirtualAlloc error code: %d\n", ErrorCode);
}

inline LARGE_INTEGER Win32_GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return Result;
}

inline double Win32_GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	uint64_t counterElapsed = End.QuadPart - Start.QuadPart;
	double secondsElapsedForWork = ((double)counterElapsed / (double)win32PerformanceCountsPerSecond);
	return secondsElapsedForWork;
}

inline int Win32_GetMicroSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
	double secondsElapsed = Win32_GetSecondsElapsed(start, end);
	return (int)(secondsElapsed * 1000000.0f);
}

// Define a function type that mirrors the signature of the
// DirectSound function being called
typedef HRESULT WINAPI direct_sound_create(
	LPCGUID pcGuidDevice,
	LPDIRECTSOUND* ppDS,
	LPUNKNOWN pUnkOuter
);

struct SoundOutput
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

	DSBUFFERDESC secondaryBufferDescription = {};
	secondaryBufferDescription.dwSize = sizeof(secondaryBufferDescription);
	secondaryBufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
	secondaryBufferDescription.dwBufferBytes = bufferSizeInBytes;
	secondaryBufferDescription.lpwfxFormat = &waveFormat;

	if (!SUCCEEDED(directSound->CreateSoundBuffer(&secondaryBufferDescription, &globalSecondarySoundBuffer, 0)))
	{
		return -7;
	}

	return 0;
}

int Win32ClearSoundBuffer(SoundOutput& soundOutput)
{
	VOID* region1;
	DWORD region1Size;
	VOID* region2;
	DWORD region2Size;

	if (!SUCCEEDED(globalSecondarySoundBuffer->Lock(
		0,
		soundOutput.bufferSizeInBytes,
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
	DWORD byteToLock,
	DWORD bytesToWrite,
	SoundBuffer& sourceSound
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
	for (DWORD byteIndex = 0; byteIndex < region1Size; byteIndex += 1)
	{
		*targetSample = *sourceSample;
		targetSample++;
		sourceSample++;
	}

	targetSample = (int16_t*)region2;
	for (DWORD byteIndex = 0; byteIndex < region2Size; byteIndex += 1)
	{
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

int Win32Main(HINSTANCE instance, const WindowSettings &settings = WindowSettings())
{
	LARGE_INTEGER perfCounterFrequencyResult;
	QueryPerformanceFrequency(&perfCounterFrequencyResult);
	win32PerformanceCountsPerSecond = perfCounterFrequencyResult.QuadPart;

	// Set the Windows schedular granularity to 1ms to help our Sleep() function call be granular
	UINT DesiredSchedulerMS = 1;
	MMRESULT setSchedularGranularityResult = timeBeginPeriod(DesiredSchedulerMS);
	bool SleepIsGranular = (setSchedularGranularityResult == TIMERR_NOERROR);

	WNDCLASSA windowClass = {0};
	windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	windowClass.lpfnWndProc = Win32_MainWindowCallback;
	windowClass.hInstance = instance;
	windowClass.lpszClassName = "Window Class";

	int gameUpdateHz = (settings.targetFPS >= 10) ? settings.targetFPS : 30;
	int targetMicroSecondsPerFrame = 1000000 / gameUpdateHz;

	if(RegisterClassA(&windowClass))
	{
		HWND window = CreateWindowExA(
			0,
			windowClass.lpszClassName,
			settings.title,
			WS_VISIBLE|WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			settings.width,
			settings.height,
			0, 0, 0, 0
		);

		if(window)
		{
			IsRunning = true;

			// Open console if settings indicate it
			if (settings.openConsole)
			{
				openConsole();
			}

			// Initialize Visual
			Win32_SizeglobalRenderBufferToCurrentWindow(window);

			// Initialize sound
			// https://learn.microsoft.com/en-us/windows/win32/coreaudio/rendering-a-stream 
			SoundOutput soundOutput = {};
			int soundSetupResult = 1;
			if (settings.playSound)
			{
				soundOutput.samplesPerSecond = 48000;
				soundOutput.bytesPerSample = 2 * sizeof(int16_t);
				soundOutput.bufferSizeInBytes = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;

				soundSetupResult = Win32SoundSetup(
					soundOutput.samplesPerSecond,
					window,
					soundOutput.bufferSizeInBytes
				);

				if (soundSetupResult == 0)
				{
					Win32ClearSoundBuffer(soundOutput);
					globalSecondarySoundBuffer->Play(
						0,
						0,
						DSBPLAY_LOOPING
					);
				}
			}

			// Initialize general use memory
			GameMemory GameMemory;
			GameMemory.permanent.sizeInBytes = Megabytes(settings.permanentSpaceInMegabytes);
			GameMemory.transient.sizeInBytes = Megabytes((uint64_t)settings.transientSpaceInMegabytes);

			uint64_t totalStorageSpace = GameMemory.permanent.sizeInBytes + GameMemory.transient.sizeInBytes;
			GameMemory.permanent.content = VirtualAlloc(0, (size_t)totalStorageSpace, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			if(GameMemory.permanent.content == NULL)
			{
				DisplayLastWin32Error();
				return -1;
			}

			GameMemory.transient.content = (uint8_t*)GameMemory.permanent.content + GameMemory.permanent.sizeInBytes;

			int16_t* samples = nullptr;
			if (soundSetupResult == 0)
			{
				samples = (int16_t*)VirtualAlloc(
					0,
					soundOutput.bufferSizeInBytes,
					MEM_RESERVE|MEM_COMMIT,
					PAGE_READWRITE
				);
			}

			// Initialize input state
			Input gameInput = {0};

			int initResult = Initialize(GameMemory, globalRenderBuffer);
			if (initResult != 0)
			{
				return initResult;
			}

			// Initialize frame timers
			float lastDtInSeconds = (float)targetMicroSecondsPerFrame / 1000000.0f;
			LARGE_INTEGER frameStartCounter = Win32_GetWallClock();

			// Main loop
			while (IsRunning)
			{
				Win32_ProcessPendingMessages(&gameInput);

				// Get mouse position
				POINT mousePointer;
				GetCursorPos(&mousePointer);	// mousePointer in screen coord
				ScreenToClient(window, &mousePointer);	// convert screen coord to window coord
				gameInput.mouse.x = mousePointer.x;
				gameInput.mouse.y = globalRenderBuffer.height - mousePointer.y;


				LARGE_INTEGER appFrameStartCounter = Win32_GetWallClock();
				int updateResult = UpdateAndRender(GameMemory, gameInput, globalRenderBuffer, lastDtInSeconds);
				int appFrameTimeInMicroSeconds = Win32_GetMicroSecondsElapsed(appFrameStartCounter, Win32_GetWallClock());
				int waitTimeInMicroSeconds = targetMicroSecondsPerFrame - appFrameTimeInMicroSeconds;
				if (updateResult != 0)
				{
					return updateResult;
				}

				ResetButtons(&gameInput);

				// render visual
				HDC deviceContext = GetDC(window);
				Win32_DisplayglobalRenderBufferInWindow(deviceContext);
				ReleaseDC(window, deviceContext);

				// Audio
				if (settings.playSound)
				{
					DWORD playCursor;
					DWORD writeCursor;
					if (globalSecondarySoundBuffer->GetCurrentPosition(&playCursor, &writeCursor) == DS_OK)
					{
						int runningSampleIndex = writeCursor / soundOutput.bytesPerSample;

						DWORD expectedBytesPerFrame = soundOutput.samplesPerSecond * soundOutput.bytesPerSample / gameUpdateHz;

						int frameDurationToAudioStart = Win32_GetMicroSecondsElapsed(frameStartCounter, Win32_GetWallClock());
						int microSecondsToFrameEnd = targetMicroSecondsPerFrame - frameDurationToAudioStart;

						DWORD expectedBytesToFrameEnd = (DWORD)((microSecondsToFrameEnd / targetMicroSecondsPerFrame) * expectedBytesPerFrame);
						DWORD expectedFrameEndByte = playCursor + expectedBytesToFrameEnd;

						DWORD safeWriteCursor = writeCursor;
						if (safeWriteCursor < playCursor)
						{
							safeWriteCursor += soundOutput.bufferSizeInBytes;
						}


						DWORD targetCursor = expectedFrameEndByte + expectedBytesPerFrame;
						targetCursor = targetCursor % soundOutput.bufferSizeInBytes;

						DWORD byteToLock = (runningSampleIndex * soundOutput.bytesPerSample) % soundOutput.bufferSizeInBytes;

						DWORD bytesToWrite = (byteToLock > targetCursor)
							? targetCursor - byteToLock + soundOutput.bufferSizeInBytes
							: targetCursor - byteToLock;

						SoundBuffer soundBuffer = {0};
						soundBuffer.samplesPerSecond = soundOutput.samplesPerSecond;
						soundBuffer.sampleCount = bytesToWrite / soundOutput.bytesPerSample;

						bytesToWrite = soundBuffer.sampleCount * soundOutput.bytesPerSample;
						soundBuffer.samples = samples;

						// Call into the application to fill the sound buffer
						UpdateSound(soundBuffer);

						DWORD unwrappedWriteCursor = writeCursor;
						if (unwrappedWriteCursor < playCursor)
						{
							unwrappedWriteCursor += soundOutput.bufferSizeInBytes;
						}

						Win32FillSoundBuffer(
							byteToLock,
							bytesToWrite,
							soundBuffer
						);
					}
				}

				// wait before starting next frame
				int microSecondsElapsedForFrame = Win32_GetMicroSecondsElapsed(frameStartCounter, Win32_GetWallClock());
				if (microSecondsElapsedForFrame < targetMicroSecondsPerFrame)
				{
					if (SleepIsGranular)
					{
						DWORD sleepMS = (DWORD)(targetMicroSecondsPerFrame - microSecondsElapsedForFrame) / 1000;
						if (sleepMS > 0)
						{
							Sleep(sleepMS);
						}
					}
					while(microSecondsElapsedForFrame < targetMicroSecondsPerFrame)
					{
						microSecondsElapsedForFrame = Win32_GetMicroSecondsElapsed(frameStartCounter, Win32_GetWallClock());
					}
				}
				else
				{
					// TODO MISSED FRAME RATE
				}

				// Output frame time information
				if (settings.openConsole)
				{
					if (appFrameTimeInMicroSeconds > maxAppFrameTimeInMicroSeconds)
					{
						maxAppFrameTimeInMicroSeconds = appFrameTimeInMicroSeconds;
					}
					TCHAR writeBuffer[256];
					wsprintf(
						writeBuffer,
						"max: %d\nactual: %d, target: %d, wait: %d\n",
						maxAppFrameTimeInMicroSeconds,
						appFrameTimeInMicroSeconds,
						targetMicroSecondsPerFrame,
						waitTimeInMicroSeconds
					);
					writeToConsole(writeBuffer);
				}

				// Take end of frame measurements
				LARGE_INTEGER frameEndCounter = Win32_GetWallClock();

				// Work out elapsed time for current frame
				lastDtInSeconds = (float)Win32_GetSecondsElapsed(frameStartCounter, frameEndCounter);
				// Reset measurementsfor next frame
				frameStartCounter = frameEndCounter;
			}
		}
		else
		{
			// Handle unable to create window
		}
	}
	else
	{
		// Handle windows window registration failure
	}

	return (0);
}

int Win32Main(HINSTANCE instance)
{
	WindowSettings settings = {0};
	settings.width = 1280;
	settings.height = 720;
	return Win32Main(instance, settings);
}

}
