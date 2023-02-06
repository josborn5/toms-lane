@echo off

set NAME=%1
set ENTRY_POINT_REL_PATH_FROM_CODE_DIR=%2
set DEBUG_FLAG=%3

call ./build-scripts/set-vcvars.bat
call ./build-scripts/set-variables.bat %DEBUG_FLAG%

pushd %OUTPUT_DIR%
del "%NAME%.exe"
del "%NAME%.obj"
del "%NAME%.map"
del "%NAME%.pdb"

REM build the given cpp file
cl.exe %COMPILER_FLAGS% "..\%CODE_DIR%\%ENTRY_POINT_REL_PATH_FROM_CODE_DIR%" /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib

popd
