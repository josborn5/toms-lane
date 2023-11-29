call .\build-scripts\set-variables.bat %1

set NAME=obby-1
set APP_DIR=%~dp0\bin-obby-1

call .\build-scripts\compile.bat "%~dp0\src\obby\obby-1\obby-1.cpp" %APP_DIR% %1
call .\build-scripts\link.bat "%APP_DIR%\obby-1.obj" "%~dp0\bin-tl-platform\toms-lane-platform.obj" "%~dp0\bin-tl-win32\toms-lane-win32.obj"

xcopy "%CODE_DIR%\obby\brick.sprc" "%APP_DIR%\"
xcopy "%CODE_DIR%\obby\checkpoint.sprc" "%APP_DIR%\"
xcopy "%CODE_DIR%\obby\player.sprc" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\obby-1.exe %1
popd