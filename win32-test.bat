@echo off

call ./build-scripts/set-variables.bat %1
call ./build-scripts/set-vcvars.bat

REM clear the output folder
rmdir /S /Q %OUTPUT_DIR%
mkdir %OUTPUT_DIR%
pushd %OUTPUT_DIR%

set NAME=toms-lane-win32.tests

REM build the cpp file
REM note no linking takes place here because linking to the win32 means
REM hooking into the win32 entry point instead of the test entrypoint
cl.exe %COMPILER_FLAGS% "..\%CODE_DIR%\win32\%NAME%.cpp"

popd

call .\build-scripts\run-project.bat %NAME%  %1
