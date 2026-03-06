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

static RenderBuffer global_render_buffer = {0};

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
	if (window_surface == nullptr) {
		return -2;
	}

	frame_buffer_surface = SDL_CreateRGBSurfaceWithFormat(
		0,
		window_surface->w,
		window_surface->h,
		32,
		SDL_PIXELFORMAT_RGB888
	);
	if (frame_buffer_surface == nullptr) {
		return -3;
	}

	global_render_buffer.width = settings.width;
	global_render_buffer.height = settings.height;
	global_render_buffer.origin = frame_buffer_origin_top_left;
	global_render_buffer.pixels = (uint32_t*)frame_buffer_surface->pixels;

	return 0;
}


int OpenWindow(const WindowSettings& settings) {
	int _1, _2;
	return OpenWindow(settings, _1, _2);
}

int RunWindowUpdateLoop(
	int targetFPS,
	UpdateWindowCallback updateWindowCallback
) {
	bool is_running = true;
	Input input = {0};

	while (is_running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					is_running = false;
				} break;
				case SDL_KEYDOWN: {
					SDL_KeyboardEvent key_event = event.key;
					if (key_event.keysym.sym >= SDLK_0 && key_event.keysym.sym <= SDLK_9) {
						int key = key_event.keysym.sym - SDLK_0 + KEY_0;
						input.buttons[key].set_state(true, key_event.repeat != 0);
					}
					else if (key_event.keysym.sym >= SDLK_a && key_event.keysym.sym <= SDLK_z) {
						int key = key_event.keysym.sym - SDLK_a + KEY_A;
						input.buttons[key].set_state(true, key_event.repeat != 0);
					}
				} break;
				case SDL_KEYUP: {
					SDL_KeyboardEvent key_event = event.key;
					if (key_event.keysym.sym >= SDLK_0 && key_event.keysym.sym <= SDLK_9) {
						int key = key_event.keysym.sym - SDLK_0 + KEY_0;
						input.buttons[key].set_state(false, true);
					}
					else if (key_event.keysym.sym >= SDLK_a && key_event.keysym.sym <= SDLK_z) {
						int key = key_event.keysym.sym - SDLK_a + KEY_A;
						input.buttons[key].set_state(false, true);
					}
				} break;	
			}
		}

		// TODO: figure out input & time between calls
		updateWindowCallback(input, 0.01666666f, global_render_buffer);

		SDL_BlitSurface(frame_buffer_surface, nullptr, window_surface, nullptr);

		SDL_UpdateWindowSurface(global_window);

		input.reset();

		SDL_Delay(16);
	}

	SDL_FreeSurface(frame_buffer_surface);
	SDL_FreeSurface(window_surface);
	SDL_DestroyWindow(global_window);

	return 0;
}

}
