set ENTRY_POINT=%1
set BIN_DIR=%2
set DEBUG_FLAG=%3

call .\build-scripts\set-vcvars.bat
call .\build-scripts\set-variables.bat %DEBUG_FLAG%

rmdir /S /Q "%BIN_DIR%"
mkdir "%BIN_DIR%"

REM build the given cpp file
cl.exe %COMPILER_FLAGS% /Fo"%BIN_DIR%\\" /c "%ENTRY_POINT%"
