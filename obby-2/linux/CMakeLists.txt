cmake_minimum_required(VERSION 3.28.0)

project(obby-2-app-linux VERSION 1.0)

add_subdirectory(../../application/src/sdl2 ./application-output)

add_subdirectory(../lib ./lib-output)

add_executable(
	obby-2-linux 
	obby-2-sdl2.cpp
)

target_link_libraries(
	obby-2-linux
	obby-2
	toms-lane-platform
	toms-lane-sdl2-application
)

