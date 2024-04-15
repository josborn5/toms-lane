#include <assert.h>
#include <stdio.h>
#include "./win32-input.hpp"

static MSG GetMessageKeyEvent(char key, bool isDown, bool wasDown)
{
	MSG message;
	message.wParam = key;
	LPARAM wasDownMask = wasDown ? (0 >> 30) : (1 >> 30);
	LPARAM isDownMask = isDown ? (0 >> 31) : (1 >> 31);
	message.lParam = wasDownMask | isDownMask;

	return message;
}

void RunInputTests()
{
	printf("==== Input ====\n");

	tl::Input input;
	MSG firstMessage = GetMessageKeyEvent('C', false, false);

	win32_input_interface_process_message(firstMessage, input);

	assert(input.buttons[KEY_C].isDown == false);
}

