cmake -S ./sdl2 -B ./sdl2/cmake-build || exit 1

cmake --build ./sdl2/cmake-build --config Release || exit 1

