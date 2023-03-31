@echo off

REM https://youtu.be/HXSuNxpCzdM?t=3199
REM https://www.youtube.com/watch?v=HyVc0X9JKpg
REM https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html

call ./platform.bat %1

REM Build tests
call ./build-scripts/set-variables.bat %1
pushd %OUTPUT_DIR%
cl.exe %COMPILER_FLAGS% ..\%CODE_DIR%\platform\toms-lane-platform.tests.cpp /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib
popd

call .\build-scripts\compile-project.bat demo demo\demo.cpp %1

call .\build-scripts\compile-project.bat obby-1 obby\obby-1\obby-1.cpp %1

call .\build-scripts\compile-project.bat obby-2 obby\obby-2\obby-2.cpp %1

call .\build-scripts\compile-project.bat breakout breakout\breakout.cpp %1

call .\build-scripts\compile-project.bat driving driving\driving.cpp %1

call .\build-scripts\compile-project.bat sprite-editor sprite-editor\sprite-editor.cpp %1
