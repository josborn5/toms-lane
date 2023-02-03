@echo off

call ./build-scripts/set-vcvars.bat

devenv ./bin/demo.exe

