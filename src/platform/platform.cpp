#include "./platform.hpp"

namespace tl
{
	bool IsReleased(const Input &input, int button)
	{
		bool isReleased = !input.buttons[button].isDown && input.buttons[button].wasDown;
		return isReleased;
	}
}
