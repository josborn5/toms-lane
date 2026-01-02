#ifndef TOMS_LANE_WINDOW_HPP
#define TOMS_LANE_WINDOW_HPP

#include "./tl-input.hpp"
#include "../../platform/src/software-rendering.hpp"

namespace tl
{
	struct WindowSettings
	{
		int width;
		int height;
		char* title;
	};

	int OpenWindow(const WindowSettings& settings);
	int OpenWindow(const WindowSettings& settings, int& outClientX, int& outClientY);

	// Define function signature of updating the video buffer
	typedef int (*UpdateWindowCallback)(const Input& input, int dtInMilliseconds, RenderBuffer& renderBuffer);

	int RunWindowUpdateLoop(
		int targetFPS,
		UpdateWindowCallback updateWindowCallback
	);
}

#endif
