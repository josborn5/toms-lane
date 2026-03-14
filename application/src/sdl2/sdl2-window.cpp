#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "../tl-window.hpp"
#include "../tl-application.hpp"

namespace tl {

static SDL_Window* global_window = nullptr;
static SDL_Renderer* global_renderer = nullptr;
static SDL_Texture* global_texture = nullptr;

static SDL_Surface* window_surface = nullptr;
static SDL_Surface* frame_buffer_surface = nullptr;

static RenderBuffer global_render_buffer;

int OpenWindow(const WindowSettings& settings, int& outClientX, int& outClientY) {
	global_window = SDL_CreateWindow(
		settings.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		settings.width,
		settings.height,
		SDL_WINDOW_SHOWN
	);

	if (global_window == nullptr) {
		return -1;
	}

	window_surface = SDL_GetWindowSurface(global_window);
	if (window_surface == nullptr || window_surface->pixels == nullptr) {
		return -2;
	}

	outClientX = settings.width;
	outClientY = settings.height;

	frame_buffer_surface = SDL_CreateRGBSurfaceWithFormat(
		0,
		window_surface->w,
		window_surface->h,
		32,
		SDL_PIXELFORMAT_RGB888
	);
	if (frame_buffer_surface == nullptr || frame_buffer_surface->pixels == nullptr) {
		return -3;
	}

	global_render_buffer.init(
		(uint32_t*)frame_buffer_surface->pixels,
		settings.width,
		settings.height,
		frame_buffer_origin_top_left
	);
	return 0;
}


int OpenWindow(const WindowSettings& settings) {
	int _1, _2;
	return OpenWindow(settings, _1, _2);
}

static int map_sdl_key_to_tl_key(int sdl_key) {
	if (sdl_key >= SDLK_0 && sdl_key <= SDLK_9) {
		return sdl_key - SDLK_0 + KEY_0;
	}

	if (sdl_key >= SDLK_a && sdl_key <= SDLK_z) {
		return sdl_key - SDLK_a + KEY_A;
	}

	if (sdl_key == SDLK_LSHIFT || sdl_key == SDLK_RSHIFT) {
		return KEY_SHIFT;
	}

	if (sdl_key == SDLK_UP) return KEY_UP;
	if (sdl_key == SDLK_DOWN) return KEY_DOWN;
	if (sdl_key == SDLK_LEFT) return KEY_LEFT;
	if (sdl_key == SDLK_RIGHT) return KEY_RIGHT;
	if (sdl_key == SDLK_SPACE) return KEY_SPACE;

	return -1;
}

int RunWindowUpdateLoop(
	int targetFPS,
	UpdateWindowCallback updateWindowCallback
) {
	bool is_running = true;
	Input input = {0};

	uint64_t performance_counter_per_second = SDL_GetPerformanceFrequency();
	uint64_t counters_per_millisecond = performance_counter_per_second / 1000;
	uint64_t target_counter_per_frame = performance_counter_per_second / targetFPS;
	uint64_t previous_start_callback_counter = SDL_GetPerformanceCounter();

	while (is_running) {
		uint64_t start_frame_counter = SDL_GetPerformanceCounter();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					is_running = false;
				} break;
				case SDL_KEYDOWN: {
					SDL_KeyboardEvent key_event = event.key;
					int key = map_sdl_key_to_tl_key(key_event.keysym.sym);
					if (key > 0) {
						input.buttons[key].set_state(true, key_event.repeat != 0);
					}
				} break;
				case SDL_KEYUP: {
					SDL_KeyboardEvent key_event = event.key;
					int key = map_sdl_key_to_tl_key(key_event.keysym.sym);
					if (key > 0) {
						input.buttons[key].set_state(false, true);
					}
				} break;	
			}
		}

		uint64_t start_callback_counter = SDL_GetPerformanceCounter();
		uint64_t counters_since_last_callback = start_callback_counter - previous_start_callback_counter;
		float delta_time_in_milliseconds = counters_since_last_callback / counters_per_millisecond;
		updateWindowCallback(input, delta_time_in_milliseconds, global_render_buffer);

		SDL_BlitSurface(frame_buffer_surface, nullptr, window_surface, nullptr);

		SDL_UpdateWindowSurface(global_window);

		input.reset();

		uint64_t end_frame_counter = SDL_GetPerformanceCounter();

		uint64_t remaining_counters_in_frame = end_frame_counter - start_frame_counter;
		if (remaining_counters_in_frame > 0) {
			uint32_t milliseconds_to_wait = remaining_counters_in_frame / counters_per_millisecond;
			SDL_Delay(milliseconds_to_wait);
		}
	}

	SDL_FreeSurface(frame_buffer_surface);
	SDL_FreeSurface(window_surface);
	SDL_DestroyWindow(global_window);

	return 0;
}

}
