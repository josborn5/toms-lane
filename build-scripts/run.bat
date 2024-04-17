@echo off

set EXE_PATH=%1

REM Treat '-d' or '-dt' flags as the same thing
SET DEBUG_RUN=%2
if "-dt"=="%DEBUG_RUN%" SET DEBUG_RUN=-d

if "-d"=="%DEBUG_RUN%" (
	@echo on
	echo RUNNING '%EXE_PATH%' IN DEBUG
	@echo off
	devenv %EXE_PATH%

) else (
	@echo on
	echo Running '%EXE_PATH%'
	@echo off
	call %EXE_PATH%
)

