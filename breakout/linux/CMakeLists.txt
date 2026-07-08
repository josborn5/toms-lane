cmake_minimum_required(VERSION 3.28.0)

project(breakout-app-linux VERSION 1.0)

add_subdirectory(../../application/src/sdl2 ./application-output)
add_subdirectory(../lib ./lib-output)
add_subdirectory(../../std-lib-functions ./std-lib-functions-output)

add_executable(
	breakout-linux 
	breakout-sdl2.cpp
)

target_link_libraries(
	breakout-linux
	breakout
	external-math-functions
	toms-lane-platform
	toms-lane-sdl2-application
)

