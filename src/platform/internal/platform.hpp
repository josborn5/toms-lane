#ifndef TOMS_LANE_PLATFORM_H
#define TOMS_LANE_PLATFORM_H

namespace tl
{
	struct WindowSettings
	{
		int width;
		int height;
		char* title;
		int targetFPS;
		unsigned long permanentSpaceInMegabytes = 1;
		unsigned long transientSpaceInMegabytes = 1;
	};

	enum KEY
	{
		KEY_ENTER,
		KEY_SPACE,
		KEY_L_SHIFT, KEY_R_SHIFT,
		KEY_TAB,
		KEY_L_CTRL, KEY_R_CTRL,
		KEY_L_ALT, KEY_R_ALT,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,

		BUTTON_COUNT
	};

	struct RenderBuffer
	{
		unsigned int* pixels;
		int width;
		int height;
		int pitch;
		int bytesPerPixel; // = 4;
		float* depth;
	};

	struct Button
	{
		bool isDown;
		bool wasDown;
		bool keyUp;
	};

	struct Mouse
	{
		int x;
		int y;
	};

	struct Input
	{
		Mouse mouse;
		Button buttons[BUTTON_COUNT];
	};

	bool IsReleased(const Input &input, int button);

	struct MemorySpace
	{
		void* content;
		unsigned long sizeInBytes;
	};

	struct GameMemory
	{
		MemorySpace permanent;
		MemorySpace transient;
	};
}
#endif
