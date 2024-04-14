#include <windows.h>
#include <stdint.h>

#include "../tl-window.hpp"
#include "./win32-time.hpp"
#include "./win32-application.hpp"
#include "./win32-input.hpp"

namespace tl
{

static bool IsRunning = false;
static RenderBuffer globalRenderBuffer = {0};
static BITMAPINFO bitmapInfo = {0};	// platform dependent
static HWND globalWindow;

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

static void Win32_ProcessPendingMessages(Input* input)
{
	// flush the queue of Messages from windows in this loop
	MSG Message;
	while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		if (Message.message == WM_KEYDOWN)
		{
			bool altKeyDown = ((Message.lParam & (1 << 29)) != 0);
			if((Message.wParam == VK_F4) && altKeyDown)
			{
				IsRunning = false;
				return;
			}
		}

		if(Message.message == WM_QUIT)
		{
			IsRunning = false;
			return;
		}

		if (!win32_input_interface_process_message(Message, *input))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
}

int OpenWindow(HINSTANCE instance, const WindowSettings& settings)
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

	// Initialize Visual
	Win32_SizeglobalRenderBufferToCurrentWindow(globalWindow);

	return 0;
}

int OpenWindow(const WindowSettings& settings)
{
	HINSTANCE instanceHandle = instance_handle_get();
	return OpenWindow(instanceHandle, settings);
}

int RunWindowUpdateLoop(
	int targetFPS,
	UpdateWindowCallback updateWindowCallback
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

	IsRunning = true;

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

		int updateResult = updateWindowCallback(gameInput, (int)(lastDtInSeconds * 1000.0f), globalRenderBuffer);
		if (updateResult != 0)
		{
			return updateResult;
		}

		win32_input_interface_reset(gameInput);

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

}
