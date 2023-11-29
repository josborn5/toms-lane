@echo off
call .\build-scripts\compile.bat "%~dp0\src\platform\toms-lane-platform.cpp" "%~dp0\bin-tl-platform" %1

call .\build-scripts\compile.bat "%~dp0\src\win32\toms-lane-win32.cpp" "%~dp0\bin-tl-win32" %1
