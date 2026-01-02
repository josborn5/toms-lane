SET CONFIG=Release
if "%1"=="-d" (
	SET CONFIG=Debug
)

cmake -S "%~dp0/src/test" -B "%~dp0/src/test/cmake-build" || exit 1

cmake --build "%~dp0/src/test/cmake-build" --config %CONFIG% || exit 1

"%~dp0/src/test/cmake-build/%CONFIG%/toms-lane-platform-test.exe"

