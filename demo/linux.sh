cmake -DCMAKE_BUILD_TYPE=Release -S sdl2 -B sdl2/cmake-build || exit 1

cmake --build sdl2/cmake-build --config Release || exit 1


sdl2/cmake-build/demo-linux

