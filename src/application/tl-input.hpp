#ifndef TOMS_LANE_INPUT_HPP
#define TOMS_LANE_INPUT_HPP

namespace tl
{
	enum KEY
	{
		KEY_ENTER,
		KEY_SPACE,
		KEY_SHIFT,
		KEY_TAB,
		KEY_CTRL,
		KEY_L_ALT, KEY_R_ALT,

		KEY_ESCAPE,

		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_PAGEUP,
		KEY_PAGEDOWN,
		KEY_END,
		KEY_HOME,

		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

		KEY_COUNT
	};

	enum MOUSE_BUTTON
	{
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,

		MOUSE_BUTTON_COUNT
	};


/**
 * time --->
 *           _____        _____
 *                |______|
 *
 * button values per frame:
 * isDown  |0000001111111000000
 * wasDown |0000000111111100000
 * keyUp   |0000000000000100000
 * keyDown |0000001000000000000
 *
 */
	struct Button
	{
		bool isDown;
		bool wasDown;
		bool keyUp;
		bool keyDown;
	};

	struct Mouse
	{
		int x;
		int y;
		Button buttons[MOUSE_BUTTON_COUNT];
	};

	struct Input
	{
		Mouse mouse;
		Button buttons[KEY_COUNT];
	};
}

#endif
