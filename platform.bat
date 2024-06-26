@echo off

SET APP_DIR=%~dp0\bin-tl-platform

call .\build-scripts\compile.bat "@%~dp0\src\platform\file-manifest.txt" %APP_DIR% %1

call .\build-scripts\link.bat %APP_DIR% platform-tests^
 "%APP_DIR%\toms-lane-platform.tests.obj"^
 "%~dp0\bin-tl-platform\toms-lane-platform.obj"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

SET TEST=%1
if "%TEST%"=="-dt" (
	SET TEST=-t
)

pushd "%APP_DIR%"
if "%TEST%"=="-t" (
	call ..\build-scripts\run.bat "platform-tests.exe" %1
)
popd

