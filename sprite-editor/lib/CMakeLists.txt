cmake_minimum_required(VERSION 3.28.0)

project(sprite-editor VERSION 1.0)

add_subdirectory(../../platform ./platform-output)

add_subdirectory(test)

add_library(
	sprite-editor STATIC
	commands.cpp
	state.cpp
	render.cpp
	palettes.cpp
	sprite-editor.cpp
	utilities.cpp
	transform.cpp
	operations.cpp
)

