set NAME=obby-2
set APP_DIR=%~dp0\bin-obby-2

pushd "%~dp0\src\obby-2"
xxd -i "background.bmp" > "background-asset.cpp"
xxd -i "obby2-player.bmp" > "player-asset.cpp"
popd

call .\build-scripts\compile.bat "%~dp0\src\obby-2\obby-2.cpp" %APP_DIR% %1
call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\obby-2.obj"^
 "%~dp0\bin-tl-platform\tl-library.lib"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

call "%~dp0\build-scripts\run.bat" "%APP_DIR%\obby-2.exe" %1
