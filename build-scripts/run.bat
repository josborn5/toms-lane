@echo off

set NAME=%1
set DEBUG_FLAG=%2

call .\build-scripts\set-variables.bat %DEBUG_FLAG%

if "%DEBUG_FLAG%"=="-d" (
	devenv "%OUTPUT_DIR%_%NAME%\%NAME%.exe"
) else (
	call "%OUTPUT_DIR%_%NAME%\%NAME%.exe"
)
