@echo off

set ENTRY_POINT=%1
set BIN_DIR=%2
set DEBUG_FLAG=%3

call "%~dp0\set-vcvars.bat"

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set DEBUG_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc -Fm

REM treat '-dt' or '-d' flags as the same thing
SET DEBUG=%DEBUG_FLAG%
if "%DEBUG_FLAG%"=="-dt" (
	SET DEBUG=-d 
)

if "%DEBUG%"=="-d" (
	@echo on
	echo Compiling in debug mode
	@echo off
	set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS%
) else (
	set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS% /O2
)

rmdir /S /Q "%BIN_DIR%"
mkdir "%BIN_DIR%"

REM build the given cpp file
cl.exe %COMPILER_FLAGS% /Fo"%BIN_DIR%\\" /c "%ENTRY_POINT%"
