
set NAME=%1
set ENTRY_POINT_REL_PATH_FROM_CODE_DIR=%2
set DEBUG_FLAG=%3

SHIFT
SHIFT
SHIFT

call .\build-scripts\set-vcvars.bat
call .\build-scripts\set-variables.bat %DEBUG_FLAG%

set APP_DIR=%OUTPUT_DIR%_%NAME%

rmdir /S /Q "%APP_DIR%"
mkdir "%APP_DIR%"

echo cl.exe %COMPILER_FLAGS% /Fo"%APP_DIR%\\" /c "%CODE_DIR%\%ENTRY_POINT_REL_PATH_FROM_CODE_DIR%"

REM build the given cpp file
cl.exe %COMPILER_FLAGS% /Fo"%APP_DIR%\\" /c "%CODE_DIR%\%ENTRY_POINT_REL_PATH_FROM_CODE_DIR%"

