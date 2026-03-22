config=Release

cmake -DCMAKE_BUILD_TYPE=$config -S linux -B linux/cmake-build || exit 1

cmake --build linux/cmake-build --config $config || exit 1

linux/cmake-build/breakout-linux

