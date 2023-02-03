@echo off

call ./build-scripts/set-vcvars.bat

devenv ./bin/toms-lane-platform.tests.exe

