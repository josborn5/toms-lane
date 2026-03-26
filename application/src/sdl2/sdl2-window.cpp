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

static uint64_t get_elapsed_counters(uint64_t start) {
	uint64_t current = SDL_GetPerformanceCounter();
	return current - start;
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

	uint64_t start_frame_counter = SDL_GetPerformanceCounter();
	float delta_time_in_milliseconds = 1.0f / (float)targetFPS;

	while (is_running) {
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

		int window_pos_x = 0;
		int window_pos_y = 0;
		SDL_GetWindowPosition(global_window, &window_pos_x, &window_pos_y);

		int mouse_pos_x = 0;
		int mouse_pos_y = 0;
		SDL_GetGlobalMouseState(&mouse_pos_x, &mouse_pos_y);
		input.mouse.x = mouse_pos_x - window_pos_x;
		input.mouse.y = 0; // TODO: fix this!

		updateWindowCallback(input, delta_time_in_milliseconds, global_render_buffer);

		SDL_BlitSurface(frame_buffer_surface, nullptr, window_surface, nullptr);

		SDL_UpdateWindowSurface(global_window);

		input.reset();

		uint64_t counters_in_frame = get_elapsed_counters(start_frame_counter);

		if (counters_in_frame < target_counter_per_frame) {
			uint64_t wait_counters = target_counter_per_frame - counters_in_frame;
			uint32_t wait_milliseconds = wait_counters / counters_per_millisecond;
			SDL_Delay(wait_milliseconds);
		}

		delta_time_in_milliseconds = get_elapsed_counters(start_frame_counter) / counters_per_millisecond;
		start_frame_counter = SDL_GetPerformanceCounter();
	}

	SDL_FreeSurface(frame_buffer_surface);
	SDL_FreeSurface(window_surface);
	SDL_DestroyWindow(global_window);

	return 0;
}

}
