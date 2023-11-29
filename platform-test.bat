call ./platform.bat %1

set NAME=toms-lane-platform.tests
set APP_DIR=%~dp0\bin-platform-test

call .\build-scripts\compile.bat "%~dp0\src\platform\toms-lane-platform.tests.cpp" %APP_DIR% %1
call .\build-scripts\link.bat "%APP_DIR%\toms-lane-platform.tests.obj" "%~dp0\bin-tl-platform\toms-lane-platform.obj"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
