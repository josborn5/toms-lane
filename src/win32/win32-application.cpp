#include <windows.h>
#include "./win32-application.hpp"

static HINSTANCE _instance;
static HWND _window = 0;

void window_handle_set(HWND windowHandle)
{
	_window = windowHandle;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	_instance = instance;

	return tl::main(commandLine);
}
