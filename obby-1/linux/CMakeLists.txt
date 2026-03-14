cmake_minimum_required(VERSION 3.28.0)

project(obby-1-app-linux VERSION 1.0)

add_subdirectory(../../application/src/sdl2 ./application-output)

add_subdirectory(../lib ./lib-output)

add_executable(
	obby-1-linux 
	obby-1-sdl2.cpp
)

target_link_libraries(
	obby-1-linux
	obby-1
	toms-lane-platform
	toms-lane-sdl2-application
)

