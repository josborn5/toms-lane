set NAME=obby-2
set APP_DIR=%~dp0\bin-obby-2

call .\build-scripts\compile.bat "%~dp0\src\obby-2\obby-2.cpp" %APP_DIR% %1
call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\obby-2.obj"^
 "%~dp0\bin-tl-platform\tl-library.lib"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

xcopy "%~dp0\src\obby-2\background.bmp" "%APP_DIR%\"
xcopy "%~dp0\src\obby-2\obby2-player.bmp" "%APP_DIR%\"
xcopy "%~dp0\bin-tl-platform\font-mono.tlsf" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\obby-2.exe %1
popd
