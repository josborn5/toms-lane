#include "../platform/toms-lane-platform.hpp"

static bool IsReleased(const tl::Input &input, int button)
{
	bool isReleased = !input.buttons[button].isDown && input.buttons[button].wasDown;
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

