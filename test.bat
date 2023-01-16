@echo off

set CODE_DIR=src
set OUTPUT_DIR=bin

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

rmdir /S /Q %OUTPUT_DIR%
mkdir %OUTPUT_DIR%
pushd %OUTPUT_DIR%

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc -Fm

set PLATFORM=toms-lane-platform

REM Build the libary .obj files
REM using the '/c' flag to skip linking and create only the '.obj' file
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\platform\%PLATFORM%.cpp

REM use the 'lib.exe' tool to create lib files from the obj files
lib.exe -nologo %PLATFORM%.obj

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib
REM Build tests
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\platform\toms-lane-platform.tests.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib

popd

call "./bin/toms-lane-platform.tests.exe"
