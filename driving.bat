set NAME=driving
set APP_DIR=%~dp0\bin-driving

call .\build-scripts\compile.bat "%~dp0\src\driving\driving.cpp", "%APP_DIR%" %1

call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\driving.obj"^
 "%~dp0\bin-tl-platform\toms-lane-platform.obj"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

xcopy "%~dp0\src\driving\test.bmp" "%APP_DIR%\"

pushd "%APP_DIR%\"
call ..\build-scripts\run.bat "%NAME%.exe" %1
popd

