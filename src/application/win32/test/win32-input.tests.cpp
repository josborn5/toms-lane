#include <assert.h>
#include <stdio.h>
#include "../win32-input.hpp"

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

static void RunTestForButton(char virtualKeyCode, int tlKeyCode)
{
	tl::Input input = {0};
	MSG nothingPressed = GetMessageKeyEvent(WM_ACTIVATE, 0, false, false);
	MSG firstPressFrame = GetMessageKeyEvent(WM_KEYDOWN, virtualKeyCode, true, false);
	MSG secondPressFrame = GetMessageKeyEvent(WM_KEYDOWN, virtualKeyCode, true, true);
	MSG firstReleaseFrame = GetMessageKeyEvent(WM_KEYUP, virtualKeyCode, false, true);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(nothingPressed, input);

	assert(input.buttons[tlKeyCode].isDown == false);
	assert(input.buttons[tlKeyCode].wasDown == false);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(firstPressFrame, input);

	assert(input.buttons[tlKeyCode].isDown == true);
	assert(input.buttons[tlKeyCode].wasDown == false);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(secondPressFrame, input);

	assert(input.buttons[tlKeyCode].isDown == true);
	assert(input.buttons[tlKeyCode].wasDown == true);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(firstReleaseFrame, input);

	assert(input.buttons[tlKeyCode].isDown == false);
	assert(input.buttons[tlKeyCode].wasDown == true);

	win32_input_interface_reset(input);
	win32_input_interface_process_message(nothingPressed, input);

	assert(input.buttons[tlKeyCode].isDown == false);
	assert(input.buttons[tlKeyCode].wasDown == false);
}



void RunInputTests()
{
	printf("\n==== Input ====\n");

	RunTestForButton('C', KEY_C);
	RunTestForButton('5', KEY_5);
	RunTestForButton(VK_LEFT, KEY_LEFT);
	RunTestForButton(VK_SHIFT, KEY_SHIFT);
}

