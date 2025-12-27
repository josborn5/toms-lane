SET CONFIG=Release
if "%1"=="-d" (
	SET CONFIG=Debug
)

cmake -S "%~dp0/win32/test" -B "%~dp0/win32/test/cmake-build" || exit 1

cmake --build "%~dp0/win32/test/cmake-build" --config %CONFIG% || exit 1

"%~dp0/win32/test/cmake-build/%CONFIG%/toms-lane-win32-application-tests.exe"

