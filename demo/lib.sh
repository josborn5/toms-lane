cmake -S lib/test -B lib/test/cmake-build || exit 1

cmake --build lib/test/cmake-build --config Release || exit 1

lib/cmake-build/test/demo-tests || exit 1

