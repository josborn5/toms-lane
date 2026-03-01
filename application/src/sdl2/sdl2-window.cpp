#include <SDL2/SDL.h>
#include <stdint.h>
#include "../tl-window.hpp"
#include "../tl-application.hpp"

namespace tl {

static SDL_Window* global_window = nullptr;
static SDL_Renderer* global_renderer = nullptr;
static SDL_Texture* global_texture = nullptr;

static SDL_Surface* global_surface = nullptr;

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


	global_surface = SDL_GetWindowSurface(global_window);

	global_render_buffer.width = settings.width;
	global_render_buffer.height = settings.height;

	global_render_buffer.pixels = (uint32_t*)global_surface->pixels;

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

	while (is_running) {
		Input input;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					is_running = false;
				} break;
				case SDL_KEYDOWN: {
					SDL_KeyboardEvent key_event = event.key;
					if (key_event.keysym.sym >= SDLK_0 && key_event.keysym.sym <= SDLK_9) {
						int key = SDLK_0 + '0' + key_event.keysym.sym;
						input.buttons[key].isDown = true;
					}
				} break;	
			}
		}

		// TODO: figure out input & time between calls
		SDL_LockSurface(global_surface);
		updateWindowCallback(input, 0.01666666f, global_render_buffer);

		SDL_UnlockSurface(global_surface);
		SDL_UpdateWindowSurface(global_window);

		SDL_Delay(16);
	}

	SDL_DestroyWindow(global_window);

	return 0;
}

}
