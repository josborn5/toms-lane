call "%~dp0../compile.bat" %3

SET CONFIG=Release
SET TEST=false
if "%3"=="-d" (
	SET CONFIG=Debug
)
if "%3"=="-dt" (
	SET CONFIG=Debug
	SET TEST=true
)
if "%3"=="-t" (
	SET TEST=true
)

if %TEST%==true (
	"%~dp0../cmake-build/%CONFIG%/%2"
) else (
	"%~dp0../cmake-build/%CONFIG%/%1"
)

