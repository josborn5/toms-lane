@echo off

call .\build-scripts\compile.bat "%~dp0\src\platform\toms-lane-platform.cpp" "%~dp0\bin-tl-platform" %1

call .\build-scripts\compile.bat "@%~dp0\src\win32\file-manifest.txt" "%~dp0\bin-tl-win32" %1

call .\build-scripts\compile.bat "%~dp0\src\win32\console\toms-lane-win32-console.cpp" "%~dp0\bin-tl-win32-console" %1

call .\build-scripts\compile.bat "%~dp0\src\win32\sound\toms-lane-win32-winmmsound.cpp" "%~dp0\bin-tl-win32-sound" %1

lib.exe /OUT:"%~dp0\bin-tl-win32\tl-win32.lib"^
 "%~dp0\bin-tl-win32\toms-lane-win32.obj"^
 "%~dp0\bin-tl-win32\toms-lane-win32-file.obj"^
 "%~dp0\bin-tl-win32\toms-lane-win32-time.obj"
