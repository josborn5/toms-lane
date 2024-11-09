set NAME=breakout
set APP_DIR=%~dp0\bin-breakout

call "%~dp0\build-scripts\compile.bat" "@%~dp0\src\breakout\filemanifest.txt", "%APP_DIR%" %1

call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\breakout.obj"^
 "%APP_DIR%\render.obj"^
 "%APP_DIR%\update_state.obj"^
 "%~dp0\bin-tl-platform\tl-library.lib"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

call "%~dp0\build-scripts\run.bat" "%APP_DIR%\%NAME%.exe" %1
