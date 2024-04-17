@echo off

set ENTRY_POINT=%1
set BIN_DIR=%2

call "%~dp0\set-vcvars.bat"

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set DEBUG_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc -Fm

REM treat '-dt' or '-d' flags as the same thing
SET DEBUG_BUILD=%3
if "-dt"=="%DEBUG_BUILD%" SET DEBUG_BUILD=-d

if "-d"=="%DEBUG_BUILD%" (
	@echo on
	echo COMPILING IN DEBUG
	@echo off
	set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS%
) else (
	set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS% /O2
)

rmdir /S /Q "%BIN_DIR%"
mkdir "%BIN_DIR%"

REM build the given cpp file
cl.exe %COMPILER_FLAGS% /Fo"%BIN_DIR%\\" /c "%ENTRY_POINT%"
