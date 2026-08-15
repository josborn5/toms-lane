#include "../lib/breakout.hpp"
#include "../../std-lib-functions/tl-math-functions.hpp"
#include "../../application/src/tl-application.hpp"
#include <stddef.h>

namespace tl
{
	// Static pixel buffer — no malloc needed
	static unsigned int s_pixels[1280 * 960]; // max 1280x960
	static RenderBuffer s_vb;
	static Input        s_input;
}

// ----- functions imported from JS (see index.html) -----
extern "C" float sinf(float);
extern "C" float cosf(float);
extern "C" float sqrtf(float);

extern "C" void open_window(float width, float height);

extern "C" void console_log(const char* text, unsigned int len);
extern "C" void console_log_value(const char* text, unsigned int len, float value);

// implement the std-lib-functions module with the JS import functions
float tl::sine(float input) {
	return sinf(input);
}

float tl::cosine(float input) {
	return cosf(input);
}

float tl::square_root(float input) {
	return sqrtf(input);
}

void tl::put_string(const char* log) {
	unsigned int counter = 0;
	const char* copy = log;
	while (*copy) {
		counter += 1;
		copy += 1;
	}
	console_log(log, counter);
}

void tl::print_int(const char* log, int num) {
	unsigned int counter = 0;
	const char* copy = log;
	while (*copy) {
		counter += 1;
		copy += 1;
	}
	console_log_value(log, counter, (float)num);
}

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
	unsigned char* d = (unsigned char*)dest;
	const unsigned char* s = (const unsigned char*)src;
	while (n--) *d++ = *s++;
	return dest;
} 

extern "C" void* memset(void* dest, int c, size_t n) {
	unsigned char* d = (unsigned char*)(dest);
	while (n--) *d++ = (unsigned char)(c);
	return dest;
} 

// ---- Stub out tl-application functions ----

int tl::sound_interface_initialize(
	UpdateSoundCallback updateSoundCallback,
	int samplesToProcessPerCallback,
	int samplesPerSecond,
	int numberOfChannels
) {
	return 0;
}

int tl::console_interface_open() {
	return 0;
}
int tl::console_interface_write(char* message) {
	return 0;
}

int tl::OpenWindow(const tl::WindowSettings& settings, int& outClientX, int& outClientY) {
	open_window((float)settings.width, (float)settings.height);
	outClientX = settings.width;
	outClientY = settings.height;

	tl::s_vb.init(
		tl::s_pixels,
		settings.width,
		settings.height,
		tl::frame_buffer_origin_top_left
	);
	tl::s_vb.color_format = tl::abgr;

	return 0;
}

// invoking the update window callback is controlled by the html, so this becomes a no-op
int tl::RunWindowUpdateLoop(
	int targetFPS,
	tl::UpdateWindowCallback updateWindowCallback
) { return 0; }


// ---- Exports called by JS ----

extern "C" int tl_tick(int dt)
{
	int return_val = updateWindowCallback(tl::s_input, dt, tl::s_vb);
	tl::s_input.reset();
	return return_val;
}

extern "C" unsigned int* tl_pixels()  { return tl::s_pixels; }

extern "C" void tl_set_mouse(int x, int y, int left)
{
	tl::s_input.mouse.x    = x;
	tl::s_input.mouse.y    = y;
}



extern "C" void tl_set_keyup(char key_code) {
	if (key_code >= 'a' && key_code <= 'z') {
		char tl_key_index = tl::KEY_A + key_code - 'a';
		tl::s_input.buttons[tl::KEY_S].set_state(false, true);
		return;
	}

	if (key_code >= 'A' && key_code <= 'Z') {
		char tl_key_index = tl::KEY_A + key_code - 'A';
		tl::s_input.buttons[tl_key_index].set_state(false, true);
		return;
	}

	if (key_code == ' ') {
		tl::s_input.buttons[tl::KEY_SPACE].set_state(false, true);
	}
}
extern "C" void tl_set_keydown(char key_code) {
	if (key_code >= 'a' && key_code <= 'z') {
		char tl_key_index = tl::KEY_A + key_code - 'a';
		tl::Button button = tl::s_input.buttons[tl_key_index];
		button.set_state(true, button.isDown);
		return;
	}

	if (key_code >= 'A' && key_code <= 'Z') {
		char tl_key_index = tl::KEY_A + key_code - 'A';
		tl::Button button = tl::s_input.buttons[tl_key_index];
		button.set_state(true, button.isDown);
		return;
	}

	if (key_code == ' ') {
		tl::Button button = tl::s_input.buttons[tl::KEY_SPACE];
		button.set_state(true, button.isDown);
	}
}

extern "C" void __wasm_call_ctors();

extern "C" void tl_main()
{
/*
 * This call to __wasm_call_ctors is important because of wasm-ld behavior.
 * Without it, the wasm linker will wrap all exported functions with a '.command_export'.
 * This command_export re-initializes all variables in c/c++ code.
 *
 * The effect of this is any state in c/c++ code is lost between calls.
 * Calling the same c/c++ function twice from JS will result in each function
 * behaving as if it were called for the first time ever.
 *
 * Calling __wasm_call_ctors() here is detected by the wasm linker and stops it
 * from wrapping exported functions with '.command_export'.
 *
 * The way to detect if command_export wrapping is taking place on an exported
 * function is to run this command and check its output:
 *
 * wasm2wat web/cmake-build/tl-window.wasm | grep -A3 'func $tl_tick'
 *
 * This command outputs the 'readable' assembly for the exported tl_tick function
 * that is defined in the tl-window.wasm file. If command_export is in the assembly,
 * the wrapping is taking place.
 */
	__wasm_call_ctors();

	breakout_main();
}
