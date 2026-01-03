SET CONFIG=Release
SET TEST=false
if "%1"=="-t" (
	SET CONFIG=Debug
	SET TEST=true
)

cmake -S "%~dp0/." -B "%~dp0/cmake-build" || exit 1

cmake --build "%~dp0/cmake-build" --config %CONFIG% || exit 1

if %TEST%==true (
	"%~dp0/cmake-build/src/test/%CONFIG%/sprite-editor-tests.exe" || exit 1
) else (
	"%~dp0/cmake-build/%CONFIG%/sprite-editor-win32.exe"
)

