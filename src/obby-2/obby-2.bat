call "%~dp0../../compile.bat" %1

SET CONFIG=Release
if "%1"=="-d" (
	SET CONFIG=Debug
)

"%~dp0../../cmake-build/%CONFIG%/obby-2-win32.exe"

