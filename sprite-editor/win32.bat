cmake -S "%~dp0/win32" -B "%~dp0/win32/cmake-build" || exit 1

cmake --build "%~dp0/win32/cmake-build" --config Release || exit 1

"%~dp0/win32/cmake-build/Release/sprite-editor-win32.exe"

