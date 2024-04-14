@echo off

set EXE_PATH=%1
set DEBUG_FLAG=%2

REM Treat '-d' or '-dt' flags as the same thing
SET DEBUG=%DEBUG_FLAG%
if "%DEBUG_FLAG%"=="-dt" (
	SET DEBUG=-d
)

if "%DEBUG%"=="-d" (
	@echo on
	echo Running '%EXE_PATH%' in debug
	@echo off
	devenv %EXE_PATH%

) else (
	@echo on
	echo Running '%EXE_PATH%'
	@echo off
	call %EXE_PATH%
)

