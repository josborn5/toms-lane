cmake -S "%~dp0/." -B "%~dp0/cmake-build" || exit 1

SET CONFIG=release
SET DEBUG=%1
if "%DEBUG%"=="-d" (
	SET CONFIG=debug
)

cmake --build "%~dp0/cmake-build" --config %CONFIG% || exit 1

