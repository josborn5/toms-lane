cmake -S . -B ./cmake-build

cmake --build ./cmake-build

call "%~dp0\build-scripts\run.bat" "%~dp0/cmake-build/Debug/breakout-win32.exe" %1
