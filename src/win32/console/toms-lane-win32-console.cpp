#include <windows.h>

static HANDLE consoleWriteHandle;

namespace tl
{
	int win32_console_interface_open()
	{
		if (AllocConsole())
		{
			consoleWriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			return 0;
		}

		return -1;
	}

	int win32_console_interface_write(char* message)
	{
		if (consoleWriteHandle == INVALID_HANDLE_VALUE)
		{
			return -1;
		}

		DWORD written = 0;
		WriteConsoleA(
			consoleWriteHandle,
			(CONST VOID*)message,
			(DWORD)strlen(message),
			&written,
			NULL
		);

		return 0;
	}
}
