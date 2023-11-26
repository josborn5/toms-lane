@echo off

set NAME=%1
set ENTRY_POINT_REL_PATH_FROM_CODE_DIR=%2
set DEBUG_FLAG=%3

call .\build-scripts\set-vcvars.bat
call .\build-scripts\set-variables.bat %DEBUG_FLAG%

set APP_DIR=%OUTPUT_DIR%\%NAME%

rmdir /S /Q "%APP_DIR%"
mkdir "%APP_DIR%"

REM build the given cpp file
cl.exe %COMPILER_FLAGS% "%CODE_DIR%\%ENTRY_POINT_REL_PATH_FROM_CODE_DIR%" /link %COMMON_LINKER_FLAGS% "%OUTPUT_DIR%\%TL_PLATFORM%.lib" "%OUTPUT_DIR%\%TL_WIN32%.lib" /OUT:"%APP_DIR%\%NAME%.exe"

