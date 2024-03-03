call ./platform.bat %1

set NAME=toms-lane-win32.tests
set APP_DIR=%~dp0\bin-win32-test

del "./test-file.dat"

call .\build-scripts\compile.bat "%~dp0\src\application\win32\win32-file.tests.cpp" %APP_DIR% %1
call .\build-scripts\link.bat "%APP_DIR%\win32-file.tests.obj" "%~dp0\bin-tl-win32\tl-win32.lib"
call .\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
