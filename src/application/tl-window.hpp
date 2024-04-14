#ifndef TOMS_LANE_WINDOW_HPP
#define TOMS_LANE_WINDOW_HPP

#include "./tl-input.hpp"

namespace tl
{
	struct RenderBuffer
	{
		unsigned int* pixels;
		int width;
		int height;
		int pitch;
		int bytesPerPixel; // = 4;
		float* depth;
	};

	struct WindowSettings
	{
		int width;
		int height;
		char* title;
	};

	int OpenWindow(const WindowSettings& settings);

	// Define function signature of updating the video buffer
	typedef int (*UpdateWindowCallback)(const Input& input, int dtInMilliseconds, RenderBuffer& renderBuffer);

	int RunWindowUpdateLoop(
		int targetFPS,
		UpdateWindowCallback updateWindowCallback
	);
}

#endif
