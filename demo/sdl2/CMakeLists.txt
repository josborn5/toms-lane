cmake_minimum_required(VERSION 3.28.0)

project(demo-app-sdl2 VERSION 1.0)

add_subdirectory(../../application/src/sdl2 ./application-output)

add_subdirectory(../lib ./lib-output)

add_executable(
	demo-linux 
	demo-sdl2.cpp
)

target_link_libraries(
	demo-linux
	demo
	toms-lane-platform
	toms-lane-sdl2-application
)

