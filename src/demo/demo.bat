SET CONFIG=Release
SET TEST=false
if "%1"=="-d" (
	SET CONFIG=Debug
)
if "%1"=="-dt" (
	SET CONFIG=Debug
	SET TEST=true
)
if "%1"=="-t" (
	SET TEST=true
)

cmake -S "%~dp0/." -B "%~dp0/cmake-build" || exit 1

cmake --build "%~dp0/cmake-build" --config %CONFIG% || exit 1

if %TEST%==true (
	"%~dp0/cmake-build/test/%CONFIG%/demo-tests.exe"
) else (
	"%~dp0/cmake-build/%CONFIG%/demo-win32.exe"
)


