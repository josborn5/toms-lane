#include <windows.h>
#include <stdint.h>

#include "../../platform/toms-lane-application.hpp"
#include "../win32-time.hpp"

namespace tl
{

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)

static bool IsRunning = false;
static RenderBuffer globalRenderBuffer = {0};
static BITMAPINFO bitmapInfo = {0};	// platform dependent

static HWND globalWindow;
static GameMemory gameMemory;
static bool playSound;

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

int OpenWindow(HINSTANCE instance, const WindowSettings &settings)
{
	WNDCLASSA windowClass = {0};
	windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	windowClass.lpfnWndProc = Win32_MainWindowCallback;
	windowClass.hInstance = instance;
	windowClass.lpszClassName = "Window Class";

	if(!RegisterClassA(&windowClass))
	{
		// Handle windows window registration failure
		return -1;
	}

	globalWindow = CreateWindowExA(
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

	if(!globalWindow)
	{
		// Handle unable to create window
		return -2;
	}

	IsRunning = true;

	// Initialize Visual
	Win32_SizeglobalRenderBufferToCurrentWindow(globalWindow);

	// Initialize general use memory
	gameMemory.permanent.sizeInBytes = Megabytes(settings.permanentSpaceInMegabytes);
	gameMemory.transient.sizeInBytes = Megabytes((uint64_t)settings.transientSpaceInMegabytes);

	uint64_t totalStorageSpace = gameMemory.permanent.sizeInBytes + gameMemory.transient.sizeInBytes;
	gameMemory.permanent.content = VirtualAlloc(0, (size_t)totalStorageSpace, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if(gameMemory.permanent.content == NULL)
	{
		DisplayLastWin32Error();
		return -1;
	}

	gameMemory.transient.content = (uint8_t*)gameMemory.permanent.content + gameMemory.permanent.sizeInBytes;

	int initResult = Initialize(gameMemory, globalRenderBuffer);
	return initResult;
}

int RunWindowUpdateLoop(
	int targetFPS
)
{
	// Set the Windows schedular granularity to 1ms to help our Sleep() function call be granular
	UINT DesiredSchedulerMS = 1;
	MMRESULT setSchedularGranularityResult = timeBeginPeriod(DesiredSchedulerMS);
	bool SleepIsGranular = (setSchedularGranularityResult == TIMERR_NOERROR);

	int gameUpdateHz = (targetFPS >= 10) ? targetFPS : 30;
	int targetMicroSecondsPerFrame = 1000000 / gameUpdateHz;

	// Initialize input state
	Input gameInput = {0};

	// Initialize frame timers
	float lastDtInSeconds = 1.0f / (float)gameUpdateHz;
	LARGE_INTEGER frameStartCounter = win32_time_interface_wallclock_get();
	win32_time_interface_initialize();


	// Main loop
	while (IsRunning)
	{
		Win32_ProcessPendingMessages(&gameInput);

		// Get mouse position
		POINT mousePointer;
		GetCursorPos(&mousePointer);	// mousePointer in screen coord
		ScreenToClient(globalWindow, &mousePointer);	// convert screen coord to window coord
		gameInput.mouse.x = mousePointer.x;
		gameInput.mouse.y = globalRenderBuffer.height - mousePointer.y;


		LARGE_INTEGER appFrameStartCounter = win32_time_interface_wallclock_get();
		int updateResult = UpdateAndRender(gameMemory, gameInput, globalRenderBuffer, lastDtInSeconds);
		if (updateResult != 0)
		{
			return updateResult;
		}

		ResetButtons(&gameInput);

		// render visual
		HDC deviceContext = GetDC(globalWindow);
		Win32_DisplayglobalRenderBufferInWindow(deviceContext);
		ReleaseDC(globalWindow, deviceContext);

		// wait before starting next frame
		int microSecondsElapsedForFrame = win32_time_interface_elapsed_microseconds_get(frameStartCounter);
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
				microSecondsElapsedForFrame = win32_time_interface_elapsed_microseconds_get(frameStartCounter);
			}
		}
		else
		{
			// TODO MISSED FRAME RATE
		}

		// Work out elapsed time for current frame
		lastDtInSeconds = (float)win32_time_interface_elapsed_seconds_get(frameStartCounter);
		// Reset measurementsfor next frame
		frameStartCounter = win32_time_interface_wallclock_get();
	}

	return 0;
}

int Win32Main(HINSTANCE instance, const WindowSettings &settings = WindowSettings())
{
	int openResult = OpenWindow(instance, settings);
	if (openResult != 0)
	{
		return openResult;
	}

	return RunWindowUpdateLoop(
		settings.targetFPS
	);
}

int Win32Main(HINSTANCE instance)
{
	WindowSettings settings = {0};
	settings.width = 1280;
	settings.height = 720;
	return Win32Main(instance, settings);
}

}
