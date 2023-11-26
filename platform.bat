@echo off

set ROOT=%~dp0
call "%ROOT%/build-scripts/set-vcvars.bat"
call "%ROOT%/build-scripts/set-variables.bat" %1

REM clear the output folder
rmdir /S /Q %OUTPUT_DIR%
mkdir %OUTPUT_DIR%

REM Build the libary .obj files
REM using the '/c' flag to skip linking and create only the '.obj' file
cl.exe %COMPILER_FLAGS% /Fo"%OUTPUT_DIR%" /c "%CODE_DIR%/platform/%TL_PLATFORM%.cpp"
cl.exe %COMPILER_FLAGS% /Fo"%OUTPUT_DIR%" /c "%CODE_DIR%/win32/%TL_WIN32%.cpp"

REM use the 'lib.exe' tool to create lib files from the obj files
lib.exe -nologo "%OUTPUT_DIR%/%TL_PLATFORM%.obj"
lib.exe -nologo "%OUTPUT_DIR%/%TL_WIN32%.obj"
