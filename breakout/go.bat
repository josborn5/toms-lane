SET CONFIG=Release

cmake -S "%~dp0/win32" -B "%~dp0/win32/cmake-build" || exit 1

cmake --build "%~dp0/win32/cmake-build" --config %CONFIG% || exit 1

"%~dp0/win32/cmake-build/src/%CONFIG%/breakout-win32.exe"

