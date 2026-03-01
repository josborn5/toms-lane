cmake_minimum_required(VERSION 3.28.0)

project(toms-lane-sdl2-application VERSION 1.0)

add_library(
	toms-lane-sdl2-application STATIC
	sdl2-application.cpp
	sdl2-window.cpp
)

find_package(
	SDL2
	REQUIRED CONFIG
	REQUIRED COMPONENTS
	SDL2
)

target_link_libraries(
	toms-lane-sdl2-application
	SDL2::SDL2
)

