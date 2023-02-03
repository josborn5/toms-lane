@echo off

set CODE_DIR=src
set OUTPUT_DIR=bin

set COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -FC -Z7 /EHsc /O2 -Fm

set TL_PLATFORM=toms-lane-platform
set TL_WIN32=toms-lane-win32

call ./build-scripts/set-vcvars.bat
call ./build-scripts/clear-output-folder.bat %OUTPUT_DIR%

REM Build the libary .obj files
REM using the '/c' flag to skip linking and create only the '.obj' file
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\platform\%TL_PLATFORM%.cpp
cl.exe %COMMON_COMPILER_FLAGS% /c ..\%CODE_DIR%\win32\%TL_WIN32%.cpp

REM use the 'lib.exe' tool to create lib files from the obj files
lib.exe -nologo %TL_PLATFORM%.obj
lib.exe -nologo %TL_WIN32%.obj