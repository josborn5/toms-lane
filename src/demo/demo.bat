call "%~dp0../../compile.bat"

SET TEST=%1
if "%TEST%"=="-dt" (
	SET TEST=-t
)

if "%TEST%"=="-t" (
	call %~dp0../../build-scripts/run.bat "%~dp0../../cmake-build/Debug/demo-tests.exe" %1
) else (

	call %~dp0../../build-scripts/run.bat "%~dp0../../cmake-build/Debug/demo-win32.exe" %1
)
