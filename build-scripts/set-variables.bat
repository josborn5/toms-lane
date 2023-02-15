@echo off

set CODE_DIR=src
set OUTPUT_DIR=bin

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set DEBUG_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc -Fm

if "%1"=="-d" (
	@echo on
	echo DEBUG MODE ENABLED!
	@echo off
	set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS%
) else (
	set COMPILER_FLAGS="%DEBUG_COMPILER_FLAGS% /O2"
)

set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS% /O2

set TL_PLATFORM=toms-lane-platform
set TL_WIN32=toms-lane-win32

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib
