set NAME=obby-1
set APP_DIR=%~dp0\bin-obby-1

call .\build-scripts\compile.bat "%~dp0\src\obby-1\obby-1.cpp" %APP_DIR% %1
call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\obby-1.obj"^
 "%~dp0\bin-tl-platform\tl-library.lib"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

xcopy "%~dp0\src\obby-1\brick.bmp" "%APP_DIR%\"
xcopy "%~dp0\src\obby-1\checkpoint.bmp" "%APP_DIR%\"
xcopy "%~dp0\src\obby-1\player.bmp" "%APP_DIR%\"
xcopy "%~dp0\bin-tl-platform\font-mono.tlsf" "%APP_DIR%\"

pushd "%APP_DIR%\"
call ..\build-scripts\run.bat "obby-1.exe" %1
popd
