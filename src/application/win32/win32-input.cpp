#include <windows.h>
#include "../../tl-application.hpp"

namespace tl
{

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

bool win32_input_interface_process_message(const MSG& message, Input& input)
{
	switch(message.message)
	{
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN: // https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-messages
		case WM_KEYUP:
		{
			uint32_t vKCode = (uint32_t)message.wParam;
			bool wasDown = ((message.lParam & (1 << 30)) != 0); // Bit #30 of the LParam tells us what the previous key was
			bool isDown = ((message.lParam & (1 << 31)) == 0); // Bit #31 of the LParam tells us what the current key is
			if (wasDown != isDown)
			{
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'A', KEY_A);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'B', KEY_B);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'C', KEY_C);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'D', KEY_D);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'E', KEY_E);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'F', KEY_F);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'G', KEY_G);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'H', KEY_H);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'I', KEY_I);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'J', KEY_J);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'K', KEY_K);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'L', KEY_L);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'M', KEY_M);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'N', KEY_N);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'O', KEY_O);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'P', KEY_P);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'Q', KEY_Q);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'R', KEY_R);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'S', KEY_S);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'T', KEY_T);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'U', KEY_U);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'V', KEY_V);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'W', KEY_W);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'X', KEY_X);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'Y', KEY_Y);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, 'Z', KEY_Z);

				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '0', KEY_0);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '1', KEY_1);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '2', KEY_2);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '3', KEY_3);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '4', KEY_4);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '5', KEY_5);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '6', KEY_6);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '7', KEY_7);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '8', KEY_8);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, '9', KEY_9);

				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_SPACE, KEY_SPACE);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_UP, KEY_UP);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_DOWN, KEY_DOWN);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_LEFT, KEY_LEFT);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_RIGHT, KEY_RIGHT);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_ESCAPE, KEY_ESCAPE);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_RETURN, KEY_ENTER);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_TAB, KEY_TAB);

				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_CONTROL, KEY_CTRL);
			}


			return true;
		} break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		{
			bool wasMouseDown = ((message.lParam & (1 << 30)) != 0); // Bit #30 of the LParam tells us what the previous key was
			bool isMouseDown = ((message.lParam & (1 << 31)) == 0); // Bit #31 of the LParam tells us what the current key is
			ProcessButtonState(&input.mouse.buttons[MOUSE_BUTTON_LEFT], isMouseDown, wasMouseDown);
			return true;
		} break;
	}

	return false;
}

void win32_input_interface_reset(Input& input)
{
	for (int i = 0; i < KEY_COUNT; i += 1)
	{
		if (input.buttons[i].keyUp)
		{
			input.buttons[i].wasDown = false;
			input.buttons[i].keyUp = false;
		}
	}

	for (int i = 0; i < MOUSE_BUTTON_COUNT; i += 1)
	{
		if (input.mouse.buttons[i].keyUp)
		{
			input.mouse.buttons[i].wasDown = false;
			input.mouse.buttons[i].keyUp = false;
		}
	}
}

}
