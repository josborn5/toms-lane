set APP_DIR=%~dp0\bin-sprite-editor
set NAME=sprite-editor

call %~dp0\build-scripts\compile.bat "@%~dp0\src\sprite-editor\file-manifest.txt", "%APP_DIR%" %1

call %~dp0\build-scripts\link.bat %APP_DIR% tests^
 "%APP_DIR%\commands.test.obj"^
 "%APP_DIR%\utilities.obj"^
 "%APP_DIR%\sprite-commands.obj"

call %~dp0\build-scripts\link.bat %APP_DIR% %NAME%^
 "%APP_DIR%\sprite-editor.obj"^
 "%APP_DIR%\state.obj"^
 "%APP_DIR%\render.obj"^
 "%APP_DIR%\sprite-editor-palettes.obj"^
 "%APP_DIR%\sprite-commands.obj"^
 "%APP_DIR%\utilities.obj"^
 "%~dp0\bin-tl-platform\toms-lane-platform.obj"^
 "%~dp0\bin-tl-win32\tl-win32.lib"

SET TEST=%1
if "%TEST%"=="-dt" (
	SET TEST=-t
)

if "%TEST%"=="-t" (
	call %~dp0\build-scripts\run.bat "%APP_DIR%\tests.exe" %1
) else (
	call %~dp0\build-scripts\run.bat "%APP_DIR%\%NAME%.exe" %1
)

