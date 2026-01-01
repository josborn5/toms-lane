#include <windows.h>
#include "../../tl-application.hpp"

static HINSTANCE _instance;
static HWND _window = 0;

namespace tl
{

HINSTANCE instance_handle_get()
{
	return _instance;
}

}

// This needs to be defined outside the tl namespace so the linker can recognize the entry point
int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	_instance = instance;

	return tl::main(commandLine);
}


