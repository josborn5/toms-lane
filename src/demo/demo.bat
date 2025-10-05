call "%~dp0../../compile.bat"

SET TEST=%1
if "%TEST%"=="-dt" (
	SET TEST=-t
)

if "%TEST%"=="-t" (
	call %~dp0../../build-scripts/run.bat "%~dp0../../cmake-build/Debug/demo-tests.exe" %1
) else (
	pushd "%~dp0../../cmake-build/Debug"
	call .\demo-win32.exe
	popd
)
