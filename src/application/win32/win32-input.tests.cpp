#include <assert.h>
#include <stdio.h>
#include "./win32-input.hpp"

static MSG GetMessageKeyEvent(UINT messageType, char key, bool isDown, bool wasDown)
{
	MSG message;
	message.message = messageType;
	message.wParam = key;
	LPARAM wasDownMask = wasDown ? (1 << 30) : (0 << 30);
	LPARAM isDownMask = isDown ? (0 << 31) : (1 << 31);
	message.lParam = wasDownMask | isDownMask;

	return message;
}

void RunInputTests()
{
	printf("\n==== Input ====\n");

	tl::Input input = {0};
	MSG nothingPressed = GetMessageKeyEvent(WM_ACTIVATE, 0, false, false);
	MSG firstPressFrame = GetMessageKeyEvent(WM_KEYDOWN, 'C', true, false);
	MSG secondPressFrame = GetMessageKeyEvent(WM_KEYDOWN, 'C', true, true);
	MSG firstReleaseFrame = GetMessageKeyEvent(WM_KEYUP, 'C', false, true);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(nothingPressed, input);

	assert(input.buttons[KEY_C].isDown == false);
	assert(input.buttons[KEY_C].wasDown == false);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(firstPressFrame, input);

	assert(input.buttons[KEY_C].isDown == true);
	assert(input.buttons[KEY_C].wasDown == false);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(secondPressFrame, input);

	assert(input.buttons[KEY_C].isDown == true);
	assert(input.buttons[KEY_C].wasDown == true);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(firstReleaseFrame, input);

	assert(input.buttons[KEY_C].isDown == false);
	assert(input.buttons[KEY_C].wasDown == true);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(nothingPressed, input);

	assert(input.buttons[KEY_C].isDown == false);
	assert(input.buttons[KEY_C].wasDown == false);
}

