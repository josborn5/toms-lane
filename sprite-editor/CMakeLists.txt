cmake_minimum_required(VERSION 3.31.0)

project(sprite-editor VERSION 1.0)

add_subdirectory(../src/platform ./platform-output)
add_subdirectory(../application ./application-output)

add_subdirectory(src/test)

add_executable(
	sprite-editor-win32 WIN32
	src/sprite-editor-win32.cpp
)

add_library(
	sprite-editor STATIC
	src/commands.cpp
	src/state.cpp
	src/render.cpp
	src/palettes.cpp
	src/sprite-editor.cpp
	src/utilities.cpp
	src/transform.cpp
	src/operations.cpp
)
target_link_libraries(
	sprite-editor-win32
	sprite-editor
	toms-lane-platform
	toms-lane-win32-application
)



