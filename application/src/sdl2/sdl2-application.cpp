#include <SDL2/SDL.h>
#include "../tl-application.hpp"

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	return tl::main(argv[0]);	
}
