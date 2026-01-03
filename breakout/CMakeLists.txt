cmake_minimum_required(VERSION 3.31.0)

project(breakout VERSION 1.0)

add_subdirectory(../platform ./platform-output)
add_subdirectory(../application ./application-output)

add_subdirectory(src)

add_executable(
	breakout-win32 WIN32
	src/breakout-win32.cpp
)

target_link_libraries(
	breakout-win32
	breakout
	toms-lane-platform
	toms-lane-win32-application
)

