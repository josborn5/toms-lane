set NAME=breakout
set APP_DIR=%~dp0\bin-breakout

call .\build-scripts\compile.bat "@%~dp0\src\breakout\filemanifest.txt", "%APP_DIR%" %1

call .\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\breakout.obj"^
 "%APP_DIR%\render.obj"^
 "%APP_DIR%\update_state.obj"^
 "%~dp0\bin-tl-platform\tl-library.lib"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

xcopy "%~dp0\bin-tl-platform\font-mono.tlsf" "%APP_DIR%\"

pushd "%APP_DIR%"
call ..\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
popd
