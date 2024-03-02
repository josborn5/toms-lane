set NAME=obby-1
set APP_DIR=%~dp0\bin-obby-1

call .\build-scripts\compile.bat "%~dp0\src\obby\obby-1\obby-1.cpp" %APP_DIR% %1
call .\build-scripts\link.bat "%APP_DIR%\obby-1.obj"^
 "%~dp0\bin-tl-platform\toms-lane-platform.obj"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

xcopy "%~dp0\src\obby\brick.sprc" "%APP_DIR%\"
xcopy "%~dp0\src\obby\checkpoint.sprc" "%APP_DIR%\"
xcopy "%~dp0\src\obby\player.sprc" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\obby-1.exe %1
popd
