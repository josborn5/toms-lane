@echo off

REM https://youtu.be/HXSuNxpCzdM?t=3199
REM https://www.youtube.com/watch?v=HyVc0X9JKpg
REM https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html

set CODE_DIR=src
set OUTPUT_DIR=bin

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

rmdir /S /Q %OUTPUT_DIR%
mkdir %OUTPUT_DIR%
pushd %OUTPUT_DIR%

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc /O2 -Fm

REM 64-bit build

REM Build the platform independent library
REM using the '/c' flag to skip linking and create only the '.obj' file
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\platform\toms-lane-platform.cpp

REM use the 'lib.exe' tool to create a lib file from the obj files
REM !!! lib.exe -nologo toms-lane.obj

REM Build the platform dependent library
REM using the '/c' flag to skip linking and create only the '.obj' file
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\win32\toms-lane-win32.cpp

REM use the 'lib.exe' tool to create a lib file from the obj files
lib.exe -nologo toms-lane-win32.obj
lib.exe -nologo toms-lane-platform.obj

REM copy the library header files to the output directory
REM !!! xcopy ..\%CODE_DIR%\*.hpp .

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib
REM Build tests
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\platform\toms-lane-platform.tests.cpp /link %COMMON_LINKER_FLAGS% toms-lane-platform.lib

REM build the demo using the toms-lane.lib
REM !!! cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\demo\demo.cpp /link %COMMON_LINKER_FLAGS% toms-lane.lib toms-lane-win32.lib

REM !!! xcopy ..\%CODE_DIR%\demo\teapot.obj .

popd
