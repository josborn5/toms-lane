cmake -S "%~dp0/lib/test" -B "%~dp0/lib/test/cmake-build" || exit 1

cmake --build "%~dp0/lib/test/cmake-build" --config Release || exit 1

"%~dp0/lib/test/cmake-build/Release/toms-lane-platform-test.exe" || exit 1

