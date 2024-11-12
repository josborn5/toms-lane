set NAME=obby-1
set APP_DIR=%~dp0\bin-obby-1

pushd "%~dp0\src\obby-1"
xxd -i "brick.bmp" > "brick.cpp"
xxd -i "checkpoint.bmp" > "checkpoint.cpp"
xxd -i "player.bmp" > "player.cpp"
popd

call "%~dp0\build-scripts\compile.bat" "%~dp0\src\obby-1\obby-1.cpp" %APP_DIR% %1
call "%~dp0\build-scripts\link.bat" %APP_DIR% %NAME%^
 "%APP_DIR%\obby-1.obj"^
 "%~dp0\bin-tl-platform\tl-library.lib"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

call "%~dp0\build-scripts\run.bat" "%APP_DIR%\obby-1.exe" %1
