@echo off

call ./build-scripts/set-vcvars.bat
call ./build-scripts/set-variables.bat

set NAME=%1
set ENTRY_POINT_REL_PATH_FROM_CODE_DIR=%2
set FLAG=%3
set FLAG2=%4

if "%FLAG%" == "-f" or if "%FLAG2%" == "-f" (
	call ./platform.bat
) else (
	pushd %OUTPUT_DIR%
	del "%NAME%.exe"
	del "%NAME%.obj"
	del "%NAME%.map"
	del "%NAME%.pdb"
)

if "%FLAG%" == "-d" or if "%FLAG2%" == "-d" (
	set COMPILER_FLAGS=%DEBUG_COMPILER_FLAGS%
) else (
	set COMPILER_FLAGS=%RELEASE_COMPILER_FLAGS%
)

REM build the given cpp file
cl.exe %COMPILER_FLAGS% "..\%CODE_DIR%\%ENTRY_POINT_REL_PATH_FROM_CODE_DIR%" /link %COMMON_LINKER_FLAGS% %TL_PLATFORM%.lib %TL_WIN32%.lib

call "%NAME%.exe"

popd
