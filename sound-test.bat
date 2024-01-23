set NAME=sound-test
set APP_DIR=%~dp0\bin-sound-test

call .\build-scripts\compile.bat "%~dp0\src\sound-test\sound-test.cpp", "%APP_DIR%" %1
call .\build-scripts\link.bat^
 "%APP_DIR%\sound-test.obj"^
 "%~dp0\bin-tl-win32-sound\toms-lane-win32-winmmsound.obj"^
 "%~dp0\bin-tl-win32-console\toms-lane-win32-console.obj"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
