call "%~dp0../../compile.bat" %1

SET CONFIG=Release
if "%1"=="-d" (
	SET CONFIG=Debug
)

"%~dp0../../cmake-build/%CONFIG%/toms-lane-platform-test.exe"

