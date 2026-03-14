config=Debug

cmake -DCMAKE_BUILD_TYPE=$config -S sdl2 -B sdl2/cmake-build || exit 1

cmake --build sdl2/cmake-build --config $config || exit 1


sdl2/cmake-build/demo-linux

