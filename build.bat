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

set PLATFORM=toms-lane-platform
set WIN32=toms-lane-win32

REM Build the libary .obj files
REM using the '/c' flag to skip linking and create only the '.obj' file
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\platform\%PLATFORM%.cpp
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\win32\%WIN32%.cpp

REM use the 'lib.exe' tool to create lib files from the obj files
lib.exe -nologo %PLATFORM%.obj
lib.exe -nologo %WIN32%.obj

set COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib
REM Build tests
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\platform\toms-lane-platform.tests.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib

REM build the demo using the toms-lane.lib
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\demo\demo.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib
xcopy ..\%CODE_DIR%\demo\teapot.obj .

REM build the first obby game
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\obby\obby-1\obby-1.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib

REM build the second obby game
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\obby\obby-2\obby-2.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib

REM build the breakout game
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\breakout\breakout.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib

REM build the driving game
cl.exe %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\driving\driving.cpp /link %COMMON_LINKER_FLAGS% %PLATFORM%.lib %WIN32%.lib

popd
