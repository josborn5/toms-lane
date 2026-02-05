cmake -S ./src/test -B ./src/test/cmake-build || exit 1

cmake --build ./src/test/cmake-build --config Release || exit 1

./src/test/cmake-build/toms-lane-platform-test || exit 1

