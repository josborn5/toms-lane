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
	return updateWindowCallback(tl::s_input, dt, tl::s_vb);
}

extern "C" unsigned int* tl_pixels()  { return tl::s_pixels; }

extern "C" void tl_set_mouse(int x, int y, int left)
{
	tl::s_input.mouse.x    = x;
	tl::s_input.mouse.y    = y;
}

extern "C" int tl_main()
{
	return breakout_main();
}
