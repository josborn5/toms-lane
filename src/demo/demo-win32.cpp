#include <windows.h>
#include "../application/win32/win32-application.hpp"
#include "./demo.hpp"

static HINSTANCE _instance;

HINSTANCE tl::instance_handle_get() {
	return _instance;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode) {
	_instance = instance;

	return demo_main();
}

