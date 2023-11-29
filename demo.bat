set NAME=demo
set APP_DIR=%~dp0\bin-demo

call .\build-scripts\compile.bat "%~dp0\src\demo\demo.cpp" "%APP_DIR%" %1
call .\build-scripts\link.bat "%APP_DIR%\demo.obj" "%~dp0\bin-tl-platform\toms-lane-platform.obj" "%~dp0\bin-tl-win32\toms-lane-win32.obj"

xcopy "%~dp0\src\demo\teapot.obj" "%APP_DIR%\"

pushd "%APP_DIR%\"
call .\demo.exe %1
popd
