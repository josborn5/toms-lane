cmake_minimum_required(VERSION 3.28.0)

project(breakout VERSION 1.0)

add_subdirectory(../../platform ./platform-output)

message("breakout context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

add_library(
	breakout STATIC
	breakout.cpp
	render.cpp
	update_state.cpp
)

