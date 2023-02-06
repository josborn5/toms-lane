@echo off

set NAME=%1
set DEBUG_FLAG=%2

call ./build-scripts/set-variables.bat %DEBUG_FLAG%

pushd %OUTPUT_DIR%

if "%DEBUG_FLAG%"=="-d" (
	devenv "%NAME%.exe"
) else (
	call "%NAME%.exe"
)

popd
