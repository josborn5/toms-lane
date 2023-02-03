@echo off

set CODE_DIR=src
set OUTPUT_DIR=bin

call ./build-scripts/set-vcvars.bat

set NAME=%1
set ENTRY_POINT_REL_PATH_FROM_CODE_DIR=%2
set FLAG=%3

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc /O2 -Fm

set PLATFORM=toms-lane-platform
set WIN32=toms-lane-win32

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib

if "%FLAG%" == "-f" (
	call ./platform.bat
) else (
	pushd %OUTPUT_DIR%
	del "%NAME%.exe"
	del "%NAME%.obj"
	del "%NAME%.map"
	del "%NAME%.pdb"
)

REM build the given cpp file
cl.exe %COMMON_COMPILER_FLAGS% "..\%CODE_DIR%\%ENTRY_POINT_REL_PATH_FROM_CODE_DIR%" /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib

call "%NAME%.exe"

popd
