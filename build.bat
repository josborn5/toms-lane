@echo off

REM https://youtu.be/HXSuNxpCzdM?t=3199
REM https://www.youtube.com/watch?v=HyVc0X9JKpg
REM https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html

call ./platform.bat

REM Build tests
cl.exe %RELEASE_COMPILER_FLAGS% ..\%CODE_DIR%\platform\toms-lane-platform.tests.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib

REM build the demo using the toms-lane.lib
cl.exe %RELEASE_COMPILER_FLAGS% ..\%CODE_DIR%\demo\demo.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib
xcopy ..\%CODE_DIR%\demo\teapot.obj .

REM build the first obby game
cl.exe %RELEASE_COMPILER_FLAGS% ..\%CODE_DIR%\obby\obby-1\obby-1.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib

REM build the second obby game
cl.exe %RELEASE_COMPILER_FLAGS% ..\%CODE_DIR%\obby\obby-2\obby-2.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib

REM build the breakout game
cl.exe %RELEASE_COMPILER_FLAGS% ..\%CODE_DIR%\breakout\breakout.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib

REM build the driving game
cl.exe %RELEASE_COMPILER_FLAGS% ..\%CODE_DIR%\driving\driving.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib

popd
