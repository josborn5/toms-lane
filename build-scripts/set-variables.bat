@echo off

set CODE_DIR=src
set OUTPUT_DIR=bin

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc /O2 -Fm

set TL_PLATFORM=toms-lane-platform
set TL_WIN32=toms-lane-win32

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib
