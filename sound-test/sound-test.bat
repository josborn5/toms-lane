SET CONFIG=Release

cmake -S "%~dp0/." -B "%~dp0/cmake-build" || exit 1

cmake --build "%~dp0/cmake-build" --config %CONFIG% || exit 1

"%~dp0/cmake-build/%CONFIG%/sound-test.exe"

