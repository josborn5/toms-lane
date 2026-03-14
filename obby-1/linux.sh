cmake -DCMAKE_BUILD_TYPE=Debug -S linux -B linux/cmake-build || exit 1

cmake --build linux/cmake-build --config Debug || exit 1

linux/cmake-build/obby-1-linux

