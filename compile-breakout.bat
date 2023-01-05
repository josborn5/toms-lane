@echo off

set CODE_DIR=src
set OUTPUT_DIR=bin

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

pushd %OUTPUT_DIR%

set NAME=breakout

del "%NAME%.exe"
del "%NAME%.obj"
del "%NAME%.map"
del "%NAME%.pdb"

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc /O2 -Fm

set PLATFORM=toms-lane-platform
set WIN32=toms-lane-win32

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib

REM build the first obby game
cl.exe %COMMON_COMPILER_FLAGS% "..\%CODE_DIR%\%NAME%\%NAME%.cpp" /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib

call "%NAME%.exe"

popd
