cmake -S ./lib/test -B ./lib/test/cmake-build || exit 1

cmake --build ./lib/test/cmake-build --config Release || exit 1

./lib/test/cmake-build/toms-lane-platform-test || exit 1

