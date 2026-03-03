cmake -DCMAKE_BUILD_TYPE=Debug -S sdl2 -B sdl2/cmake-build || exit 1

cmake --build sdl2/cmake-build --config Debug || exit 1


sdl2/cmake-build/demo-linux

