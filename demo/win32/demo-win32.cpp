#include <windows.h>
#include "../../application/src/win32/win32-application.hpp"
#include "../src/demo.hpp"

static HINSTANCE _instance;

HINSTANCE tl::instance_handle_get() {
	return _instance;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode) {
	_instance = instance;

	return demo_main();
}

