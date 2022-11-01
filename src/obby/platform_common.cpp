#include "../platform/toms-lane-platform.hpp"

static bool IsReleased(const tl::Input &input, int button)
{
	bool isReleased = !input.buttons[button].isDown && input.buttons[button].wasDown;
	return isReleased;
}

static bool IsDown(const tl::Input &input, int button)
{
	bool isReleased = input.buttons[button].isDown;
	return isReleased;
}

static int ClampInt(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static float ClampFloat(float min, float val, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static uint32_t MakeColorFromGrey(uint8_t grey)
{
	return (grey << 0) | (grey << 8) | (grey << 16);
}
