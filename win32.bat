@echo off

call .\build-scripts\compile.bat "@%~dp0\src\win32\file-manifest.txt" "%~dp0\bin-tl-win32" %1

lib.exe /OUT:"%~dp0\bin-tl-win32\tl-win32.lib"^
 "%~dp0\bin-tl-win32\win32-window.obj"^
 "%~dp0\bin-tl-win32\win32-file.obj"^
 "%~dp0\bin-tl-win32\win32-time.obj"^
 "%~dp0\bin-tl-win32\win32-console.obj"^
 "%~dp0\bin-tl-win32\win32-winmmsound.obj"^
 "%~dp0\bin-tl-win32\win32-memory.obj"^
 "%~dp0\bin-tl-win32\win32-application.obj"
