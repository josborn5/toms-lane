#include <windows.h>
#include "../../tl-application.hpp"

namespace tl
{

static void ProcessButtonState(Button* button, int isDown, int wasDown)
{
	button->isDown = (isDown != 0);
	button->wasDown = (wasDown != 0);
	button->keyUp = (!button->isDown && button->wasDown);
	button->keyDown = (button->isDown && !button->wasDown);
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
			TranslateMessage(&message); // Send it back to windows to translate and dispatch a WM_CHAR message if the key is a character key

			uint32_t vKCode = (uint32_t)message.wParam;
			bool wasDown = ((message.lParam & (1 << 30)) != 0); // Bit #30 of the LParam tells us what the previous key was
			bool isDown = ((message.lParam & (1 << 31)) == 0); // Bit #31 of the LParam tells us what the current key is

			if (vKCode >= 'A' && vKCode <= 'Z')
			{
				int tlKey = (KEY_A - 'A') + vKCode;
				Button* button = &input.buttons[tlKey];
				ProcessButtonState(button, isDown, wasDown);
			}
			else if (vKCode >= '0' && vKCode <= '9')
			{
				int tlKey = (KEY_0 - '0') + vKCode;
				Button* button = &input.buttons[tlKey];
				ProcessButtonState(button, isDown, wasDown);
			}
			else
			{
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_SPACE, KEY_SPACE);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_PRIOR, KEY_PAGEUP);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_NEXT, KEY_PAGEDOWN);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_HOME, KEY_HOME);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_END, KEY_END);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_UP, KEY_UP);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_DOWN, KEY_DOWN);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_LEFT, KEY_LEFT);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_RIGHT, KEY_RIGHT);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_ESCAPE, KEY_ESCAPE);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_RETURN, KEY_ENTER);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_TAB, KEY_TAB);

				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_CONTROL, KEY_CTRL);
				Win32_ProcessKeyboardMessage(input.buttons, isDown, wasDown, vKCode, VK_SHIFT, KEY_SHIFT);
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
		case WM_CHAR:
		{
			input.character = (char)message.wParam;
			return true;
		} break;
	}

	return false;
}

void win32_input_interface_reset(Input& input)
{
	for (int i = 0; i < KEY_COUNT; i += 1)
	{
		Button& button = input.buttons[i];
		// When holding down a button the first key-down message shows isDown:true
		// and wasDown:false. The next key-down message to come after than, that
		// shows isDown:true and wasDown:true only comes ~1 second after the first.
		// To support this being called in a frame rate much higher than 1 frames
		// per second, the reset function needs to compare the input state of a button
		// from the prior frame to determine if it's held down from the previous frame.
		if (button.keyDown)
		{
			button.wasDown = true;
			button.keyDown = false;
		}
		else if (button.keyUp)
		{

			button.wasDown = false;
			button.keyUp = false;
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

	input.character = 0;
}

}
