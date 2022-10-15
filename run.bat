@echo off

call "./build.bat"

REM The teapot file is only read if the exe is called from the directory it's in

pushd bin

call "./demo.exe"

popd

