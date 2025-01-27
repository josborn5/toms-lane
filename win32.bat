@echo off

set NAME=toms-lane-win32-tests
set APP_DIR=%~dp0\bin-tl-win32

call .\build-scripts\compile.bat "@%~dp0\src\application\win32\file-manifest.txt" %APP_DIR% %1

lib.exe /OUT:"%APP_DIR%\tl-win32.lib"^
 "%APP_DIR%\win32-window.obj"^
 "%APP_DIR%\win32-file.obj"^
 "%APP_DIR%\win32-time.obj"^
 "%APP_DIR%\win32-console.obj"^
 "%APP_DIR%\win32-winmmsound.obj"^
 "%APP_DIR%\win32-memory.obj"^
 "%APP_DIR%\win32-application.obj"^
 "%APP_DIR%\win32-text.obj"^
 "%APP_DIR%\win32-input.obj"^
 user32.lib^
 Gdi32.lib^
 winmm.lib

call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\win32-tests.obj"^
 "%APP_DIR%\win32-file.obj"^
 "%APP_DIR%\win32-input.obj"^
 user32.lib

SET TEST=%1
if "%1"=="-dt" (
	SET TEST=-t
)

if "%TEST%"=="-t" (
	del "./test-file.dat"
	call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
	del "./test-file.dat"
)

@echo off

