@echo off

call ./platform.bat %1

call .\build-scripts\compile-project.bat toms-lane-platform.tests platform\toms-lane-platform.tests.cpp %1
call .\build-scripts\run-project.bat toms-lane-platform.tests  %1
