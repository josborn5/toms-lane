set NAME=driving
set APP_DIR=%~dp0\bin-driving

call .\build-scripts\compile.bat "%~dp0\src\driving\driving.cpp", "%APP_DIR%" %1
call .\build-scripts\link.bat "%APP_DIR%\driving.obj" "%~dp0\bin-tl-platform\toms-lane-platform.obj" "%~dp0\bin-tl-win32\toms-lane-win32.obj"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1

