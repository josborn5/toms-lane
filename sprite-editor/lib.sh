cmake -S lib -B lib/cmake-build || exit 1

cmake --build lib/cmake-build --config Release || exit 1

lib/cmake-build/test/sprite-editor-tests || exit 1

