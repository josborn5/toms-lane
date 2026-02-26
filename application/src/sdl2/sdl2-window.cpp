#include <SDL2/SDL.h>
#include <stdint.h>
#include "../tl-window.hpp"

namespace tl {

static SDL_Window* global_window = nullptr;
static SDL_Renderer* global_renderer = nullptr;
static SDL_Texture* global_texture = nullptr;

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

	global_renderer = SDL_CreateRenderer(
		global_window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	if (global_renderer == nullptr) {
		return -2;
	}

	global_texture = SDL_CreateTexture(
		global_renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		settings.width,
		settings.height
	);

	uint32_t* pixel_content = (uint32_t*)malloc(settings.width * settings.height * sizeof(uint32_t));

	global_render_buffer.pixels = pixel_content;
	global_render_buffer.width = settings.width;
	global_render_buffer.height = settings.height;

	return 0;
}

	


int RunWindowUpdateLoop(
	int targetFPS,
	UpdateWindowCallback updateWindowCallback
) {
	bool is_running = true;
	while (is_running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				is_running = false;
			}
		}

		// TODO: figure out input & time between calls
		Input input;
		updateWindowCallback(input, 0.1666666f, global_render_buffer);

		SDL_UpdateTexture(
			global_texture,
			nullptr,
			global_render_buffer.pixels,
			global_render_buffer.width * sizeof(uint32_t));

		SDL_RenderClear(global_renderer);

		SDL_RenderCopy(global_renderer, global_texture, nullptr, nullptr);

		SDL_RenderPresent(global_renderer);
	}

	free(global_render_buffer.pixels);
	SDL_DestroyTexture(global_texture);
	SDL_DestroyRenderer(global_renderer);
	SDL_DestroyWindow(global_window);

	return 0;
}

}
