@echo off

set EXE_PATH=%1
set DEBUG_FLAG=%2

if "%2"=="-d" (
	devenv "%EXE_PATH%"
) else (
	call "%EXE_PATH%"
)
