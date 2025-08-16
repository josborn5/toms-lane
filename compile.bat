cmake -S "%~dp0/." -B "%~dp0/cmake-build" || exit 1

cmake --build "%~dp0/cmake-build" || exit 1

