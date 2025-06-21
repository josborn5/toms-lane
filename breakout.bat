pushd cmake-build

cmake ../

cmake --build .

popd

call "%~dp0\build-scripts\run.bat" "%~dp0/cmake-build/Debug/breakout-win32.exe" %1
