call ./platform.bat %1

set NAME=toms-lane-win32.tests
set APP_DIR=%~dp0\bin-win32-test

call .\build-scripts\compile.bat "%~dp0\src\win32\toms-lane-win32.tests.cpp" %APP_DIR% %1
call .\build-scripts\link.bat "%APP_DIR%\toms-lane-win32.tests.obj"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1