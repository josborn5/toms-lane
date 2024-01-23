set NAME=breakout
set APP_DIR=%~dp0\bin-breakout

call .\build-scripts\compile.bat "%~dp0\src\breakout\breakout.cpp", "%APP_DIR%" %1
call .\build-scripts\link.bat^
 "%APP_DIR%\breakout.obj"^
 "%~dp0\bin-tl-platform\toms-lane-platform.obj"^
 "%~dp0\bin-tl-win32\tl-win32.lib"^
 "%~dp0\bin-tl-win32-sound\toms-lane-win32-winmmsound.obj"^
 "%~dp0\bin-tl-win32-console\toms-lane-win32-console.obj"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
