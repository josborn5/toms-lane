cmake -S src -B src/cmake-build || exit 1

cmake --build src/cmake-build --config Release || exit 1

