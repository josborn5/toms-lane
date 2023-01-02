@echo off

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

devenv ./bin/toms-lane-platform.tests.exe

