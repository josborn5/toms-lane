@echo off

call ./build-scripts/set-variables.bat %1
call ./build-scripts/set-vcvars.bat

pushd %OUTPUT_DIR%
set NAME=toms-lane-win32.tests
del "%NAME%.exe"
del "%NAME%.obj"
del "%NAME%.map"
del "%NAME%.pdb"

REM build the cpp file
REM note no linking takes place here because linking to the win32 means
REM hooking into the win32 entry point instead of the test entrypoint
cl.exe %COMPILER_FLAGS% "..\%CODE_DIR%\win32\%NAME%.cpp"

popd

call .\build-scripts\run-project.bat %NAME%  %1
