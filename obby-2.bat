set NAME=obby-2
set APP_DIR=%~dp0\bin-obby-2

call .\build-scripts\compile.bat "%~dp0\src\obby\obby-2\obby-2.cpp" %APP_DIR% %1
call .\build-scripts\link.bat "%APP_DIR%\obby-2.obj"^
 "%~dp0\bin-tl-platform\toms-lane-platform.obj"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

xcopy "%~dp0\src\obby\background.sprc" "%APP_DIR%\"
xcopy "%~dp0\src\obby\obby2-player.sprc" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\obby-2.exe %1
popd
