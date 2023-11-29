call .\build-scripts\set-variables.bat %1

set NAME=sprite-editor
set APP_DIR=%~dp0\bin-sprite-editor

call .\build-scripts\compile.bat "%~dp0\src\sprite-editor\sprite-editor.cpp", "%APP_DIR%" %1
call .\build-scripts\link.bat "%APP_DIR%\sprite-editor.obj" "%~dp0\bin-tl-platform\toms-lane-platform.obj" "%~dp0\bin-tl-win32\toms-lane-win32.obj"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1

