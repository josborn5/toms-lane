cmake_minimum_required(VERSION 3.31.0)

project(win32-application VERSION 1.0)

add_subdirectory(../../ ./cmake-build/application-output)

add_executable(
	toms-lane-win32-application-tests
	win32-tests.cpp
)
target_link_libraries(
	toms-lane-win32-application-tests
	toms-lane-win32-application
)

