@echo off

REM https://youtu.be/HXSuNxpCzdM?t=3199
REM https://www.youtube.com/watch?v=HyVc0X9JKpg
REM https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html

set CODE_DIR=src
set OUTPUT_DIR=bin

REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc /O2 -Fm

set PLATFORM=toms-lane-platform
set WIN32=toms-lane-win32

call ./platform.bat

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
